#include "mcv_platform.h"
#include "comp_attached_to_bone.h"
#include "comp_skeleton.h"
#include "components/common/comp_transform.h"
#include "entity/entity_parser.h"

// Changed name from skeleton to force to be parsed before the comp_mesh
DECL_OBJ_MANAGER("attached_to_bone", TCompAttachedToBone);

void TCompAttachedToBone::onEntityCreated()
{
  h_my_transform = get<TCompTransform>();
}

bool TCompAttachedToBone::resolveTarget(CHandle h_entity_parent)
{
  // Unassign current values
  bone_id = -1;
  h_target_skeleton = CHandle();

  // An handle to entity has been given, confirm it's valid and get the TCompSkeleton handle
  CEntity* e_parent = h_entity_parent;
  if (!e_parent)
    return false;

  // Get the handle of the TCompSkeleton
  h_target_skeleton = e_parent->get<TCompSkeleton>();
  TCompSkeleton* c_skel = h_target_skeleton;
  assert(c_skel);

  // Resolve bone_name to bone_id
  bone_id = c_skel->getBoneIdByName(bone_name);
  return bone_id != -1;
}

void TCompAttachedToBone::load(const json& j, TEntityParseContext& ctx) {
  bone_name = j["bone"];
  target_entity_name = j["target"];
  CHandle h_entity_parent = ctx.findEntityByName(target_entity_name);
  resolveTarget(h_entity_parent);
}

void TCompAttachedToBone::update(float delta) {
  if( bone_id == -1 ) 
    return;
  TCompSkeleton* c_skel = h_target_skeleton;
  if( !c_skel  )
    return;
  TCompTransform* c_trans = h_my_transform;
  assert(c_trans);
  c_trans->set(c_skel->getWorldTransformOfBone(bone_id));
}

void TCompAttachedToBone::debugInMenu() {
  ImGui::Text( "Bone ID(%s) = %d", bone_name.c_str(), bone_id );
}

