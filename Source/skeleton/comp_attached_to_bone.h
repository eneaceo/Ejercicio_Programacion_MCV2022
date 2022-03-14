#pragma once

#include "components/common/comp_base.h"
#include "entity/entity.h"

struct TCompAttachedToBone : public TCompBase {

  DECL_SIBLING_ACCESS();

  CHandle     h_target_skeleton;    // TCompSkeleton
  CHandle     h_my_transform;
  int         bone_id = -1;
  std::string target_entity_name;
  std::string bone_name;

  bool resolveTarget(CHandle h_entity_parent);
  void load(const json& j, TEntityParseContext& ctx);
  void update(float);
  void debugInMenu();
  void onEntityCreated();

};

