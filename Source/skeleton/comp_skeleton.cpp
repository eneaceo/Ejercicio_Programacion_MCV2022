#include "mcv_platform.h"
#include "comp_skeleton.h"
#include "cal3d/cal3d.h"
#include "game_core_skeleton.h"
#include "render/draw_primitives.h"
#include "components/common/comp_transform.h"
#include "components/common/comp_aabb.h"
#include "comp_skeleton_ik.h"
#include "comp_skel_lookat.h"

// Changed name from skeleton to force to be parsed before the comp_mesh
DECL_OBJ_MANAGER("armature", TCompSkeleton);

// ---------------------------------------------------------------------------------------
// Cal2DX conversions, VEC3 are the same, QUAT must change the sign of w
CalVector DX2Cal(VEC3 p) {
  return CalVector(p.x, p.y, p.z);
}
CalQuaternion DX2Cal(QUAT q) {
  return CalQuaternion(q.x, q.y, q.z, -q.w);
}
VEC3 Cal2DX(CalVector p) {
  return VEC3(p.x, p.y, p.z);
}
QUAT Cal2DX(CalQuaternion q) {
  return QUAT(q.x, q.y, q.z, -q.w);
}
MAT44 Cal2DX(CalVector trans, CalQuaternion rot) {
  return
    MAT44::CreateFromQuaternion(Cal2DX(rot))
    * MAT44::CreateTranslation(Cal2DX(trans))
    ;
}


TCompSkeleton::TCompSkeleton() {
  model = nullptr;
}

TCompSkeleton::~TCompSkeleton() {
  cb_bones.destroy();
  if (model)
    delete model;
  model = nullptr;
}

void TCompSkeleton::onEntityCreated() {
  bool is_ok = cb_bones.create(CB_SLOT_SKIN, "SkinBones");
  assert(is_ok);
}

void TCompSkeleton::load(const json& j, TEntityParseContext& ctx) {
  std::string src = j["src"];
  auto core = Resources.get(src)->as<CGameCoreSkeleton>();
  // Cal3d
  model = new CalModel((CalCoreModel*)core);
  model->getMixer()->blendCycle(0, 1.0f, 0.0f);

  // Do a time zero update just to have the bones in a correct place
  model->update(0.f);
}

void TCompSkeleton::updateLookAt(float delta) {
  PROFILE_FUNCTION("updatePostSkeleton");
  TCompSkelLookAt* lookat = get< TCompSkelLookAt>();
  if (lookat)
    lookat->update(delta);
}

void TCompSkeleton::updateIKs(float delta) {
  PROFILE_FUNCTION("updateIKs");
  TCompSkeletonIK* c_iks = get<TCompSkeletonIK>();
  if (c_iks)
    c_iks->update(delta);
}

void TCompSkeleton::update(float delta) {
  PROFILE_FUNCTION("Skeleton");
  TCompTransform* c_trans = get<TCompTransform>();
  model->getMixer()->setWorldTransform(DX2Cal(c_trans->getPosition()), DX2Cal(c_trans->getRotation()));
  {
    PROFILE_FUNCTION("Cal3d");
    model->update(delta);
  }

  updateLookAt(delta);
  updateIKs(delta);

  //updateCteBones();
  {
    PROFILE_FUNCTION("Motion");
    // Transfer the root motion back to the entity
    VEC3 root_motion_delta_local = Cal2DX(model->getMixer()->getAndClearDeltaLocalRootMotion());
    VEC3 root_motion_delta_abs = VEC3::Transform(root_motion_delta_local, c_trans->getRotation());

    // Use root_motion_delta_abs to collide with physics for example
    c_trans->setPosition(c_trans->getPosition() + root_motion_delta_abs);
  }

  updateAABB();
}

void TCompSkeleton::renderDebug() {
  int num_bones = (int)model->getSkeleton()->getCoreSkeleton()->getVectorCoreBone().size();
  std::vector<VEC3> lines;
  lines.resize(num_bones * 2);
  model->getSkeleton()->getBoneLines(&lines.data()->x);
  for (int i = 0; i < num_bones; ++i) {
    drawLine(lines[i * 2], lines[i * 2 + 1], Colors::White);
  }

  if (show_bone_axis) {
    for (int i = 0; i < num_bones; ++i) {
      CTransform transform = getWorldTransformOfBone(i);
      transform.setScale(VEC3(show_bone_axis_scale));
      drawAxis(transform.asMatrix());
    }
  }

  // Show list of bones
  //auto mesh = Resources.get("axis.mesh")->as<CMesh>();
  auto core = (CGameCoreSkeleton*)model->getCoreModel();
  auto& bones_to_debug = core->bone_ids_to_debug;
  for (auto it : bones_to_debug) {
    CalBone* cal_bone = model->getSkeleton()->getBone(it);
    QUAT rot = Cal2DX(cal_bone->getRotationAbsolute());
    VEC3 pos = Cal2DX(cal_bone->getTranslationAbsolute());
    MAT44 mtx;
    mtx = MAT44::CreateFromQuaternion(rot) * MAT44::CreateTranslation(pos);
    drawAxis(mtx);
    //drawPrimitive(mesh, mtx, Colors::White);
  } 

}

void TCompSkeleton::updateCteBones() {
  PROFILE_FUNCTION("updateCteBones");
  // Pointer to the first float of the array of matrices
   //float* fout = &cb_bones.bones[0]._11;
  float* fout = &cb_bones.bones[0]._11;

  assert(model);
  CalSkeleton* skel = model->getSkeleton();
  auto& cal_bones = skel->getVectorBone();
  assert(cal_bones.size() < MAX_SUPPORTED_BONES);

  // For each bone from the cal model
  for (size_t bone_idx = 0; bone_idx < cal_bones.size(); ++bone_idx) {
    CalBone* bone = cal_bones[bone_idx];

    const CalMatrix& cal_mtx = bone->getTransformMatrix();
    const CalVector& cal_pos = bone->getTranslationBoneSpace();

    *fout++ = cal_mtx.dxdx;
    *fout++ = cal_mtx.dydx;
    *fout++ = cal_mtx.dzdx;
    *fout++ = 0.f;
    *fout++ = cal_mtx.dxdy;
    *fout++ = cal_mtx.dydy;
    *fout++ = cal_mtx.dzdy;
    *fout++ = 0.f;
    *fout++ = cal_mtx.dxdz;
    *fout++ = cal_mtx.dydz;
    *fout++ = cal_mtx.dzdz;
    *fout++ = 0.f;
    *fout++ = cal_pos.x;
    *fout++ = cal_pos.y;
    *fout++ = cal_pos.z;
    *fout++ = 1.f;
  }

  cb_bones.updateFromCPU();
}

CTransform TCompSkeleton::getWorldTransformOfBone(int bone_id) const {
  assert(bone_id >= 0);
  assert(bone_id < model->getCoreModel()->getCoreSkeleton()->getVectorCoreBone().size());
  CalBone* bone = model->getSkeleton()->getBone(bone_id);
  assert(bone);
  CalVector cal_pos = bone->getTranslationAbsolute();
  CalQuaternion cal_quat = bone->getRotationAbsolute();
  CTransform transform;
  transform.setPosition(Cal2DX(cal_pos));
  transform.setRotation(Cal2DX(cal_quat));
  return transform;
}

int TCompSkeleton::getBoneIdByName(const std::string& bone_name) const {
  return model->getCoreModel()->getCoreSkeleton()->getCoreBoneId(bone_name);
}

const CGameCoreSkeleton* TCompSkeleton::getGameCore() const {
  return (CGameCoreSkeleton*)model->getCoreModel();
}

void TCompSkeleton::updateAABB() {
  PROFILE_FUNCTION("updateAABB");
  TCompAbsAABB* aabb = get<TCompAbsAABB>();
  if (!aabb)
    return;

  VEC3 points[MAX_SUPPORTED_BONES];
  CalSkeleton* skel = model->getSkeleton();
  auto& cal_bones = skel->getVectorBone();
  assert(cal_bones.size() < MAX_SUPPORTED_BONES);

  auto& bone_ids_for_aabb = getGameCore()->bone_ids_for_aabb;
  int out_idx = 0;
  for (int bone_idx : bone_ids_for_aabb) {
    CalBone* bone = cal_bones[bone_idx];
    points[out_idx] = Cal2DX(bone->getTranslationAbsolute());
    ++out_idx;
  }
  {
    PROFILE_FUNCTION("CreateFromPoints");
    AABB::CreateFromPoints(*aabb, out_idx, points, sizeof(VEC3));
  }
  aabb->Extents = getGameCore()->aabb_extra_factor * VEC3(aabb->Extents);
}


void TCompSkeleton::debugInMenu() {

  static float in_delay = 0.3f;
  static float out_delay = 0.3f;
  static int anim_id = 0;
  static bool auto_lock = false;
  static bool root_motion = false;

  // Play actions/cycle from the menu
  ImGui::DragInt("Anim Id", &anim_id, 0.1f, 0, model->getCoreModel()->getCoreAnimationCount() - 1);
  auto core_anim = model->getCoreModel()->getCoreAnimation(anim_id);
  if (core_anim)
    ImGui::Text("%s", core_anim->getName().c_str());
  ImGui::DragFloat("In Delay", &in_delay, 0.01f, 0, 1.f);
  ImGui::DragFloat("Out Delay", &out_delay, 0.01f, 0, 1.f);
  ImGui::Checkbox("Auto lock", &auto_lock);
  ImGui::Checkbox("Extract Root Motion", &root_motion);
  if (ImGui::SmallButton("As Cycle")) {
    model->getMixer()->blendCycle(anim_id, 1.0f, in_delay);
  }
  if (ImGui::SmallButton("As Action")) {
    model->getMixer()->executeAction(anim_id, in_delay, out_delay, 1.0f, auto_lock, root_motion);
  }

  // Dump Mixer
  auto mixer = model->getMixer();
  for (auto a : mixer->getAnimationActionList()) {
    ImGui::PushID(a);
    ImGui::Text("Action %s S:%d W:%1.2f Time:%1.4f/%1.4f"
      , a->getCoreAnimation()->getName().c_str()
      , a->getState()
      , a->getWeight()
      , a->getTime()
      , a->getCoreAnimation()->getDuration()
    );
    ImGui::SameLine();
    if (ImGui::SmallButton("X")) {
      auto core = (CGameCoreSkeleton*)model->getCoreModel();
      int id = core->getCoreAnimationId(a->getCoreAnimation()->getName());
      if (a->getState() == CalAnimation::State::STATE_STOPPED)
        mixer->removeAction(id, 0.f);
      else
        a->remove(out_delay);
      ImGui::PopID();
      break;
    }
    ImGui::PopID();
  }

  ImGui::Text("Mixer Time: %f/%f", mixer->getAnimationTime(), mixer->getAnimationDuration());
  for (auto a : mixer->getAnimationCycle()) {
    ImGui::PushID(a);
    ImGui::Text("Cycle %s S:%d W:%1.2f Time:%1.4f"
      , a->getCoreAnimation()->getName().c_str()
      , a->getState()
      , a->getWeight()
      , a->getCoreAnimation()->getDuration()
    );
    ImGui::SameLine();
    if (ImGui::SmallButton("X")) {
      auto core = (CGameCoreSkeleton*)model->getCoreModel();
      int id = core->getCoreAnimationId(a->getCoreAnimation()->getName());
      mixer->clearCycle(id, out_delay);
    }
    ImGui::PopID();
  }

  // Show Skeleton Resource
  if (ImGui::TreeNode("Core")) {
    auto core_skel = (CGameCoreSkeleton*)model->getCoreModel();
    if (core_skel)
      core_skel->renderInMenu();
    ImGui::TreePop();
  }


  ImGui::Checkbox("Show All Bones", &show_bone_axis);
  if (show_bone_axis)
    ImGui::DragFloat("Axis Scale", &show_bone_axis_scale, 0.02f, 0.f, 1.0f);

}

