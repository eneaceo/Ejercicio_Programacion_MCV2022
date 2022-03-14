#pragma once

#include "components/common/comp_base.h"
#include "entity/entity.h"
#include "render/shaders/shader_cte.h"

class CalModel;
class CGameCoreSkeleton;

struct TCompSkeleton : public TCompBase {

  DECL_SIBLING_ACCESS();

  CalModel* model = nullptr;
  CShaderCte<CtesSkin> cb_bones;
  bool show_bone_axis = false;
  float show_bone_axis_scale = 0.1f;

  TCompSkeleton();
  ~TCompSkeleton();

  void load(const json& j, TEntityParseContext& ctx);
  void update(float);
  void updateMultithread(float dt) { update(dt); }
  void renderDebug();
  void debugInMenu();
  void onEntityCreated();
  void updateCteBones();
  void updateLookAt(float dt);
  void updateAABB();
  void updateIKs(float dt);
  
  CTransform getWorldTransformOfBone( int bone_id ) const;
  int getBoneIdByName( const std::string& bone_name ) const;
  const CGameCoreSkeleton* getGameCore() const;

};

