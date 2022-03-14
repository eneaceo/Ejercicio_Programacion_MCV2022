#pragma once

#include "comp_base.h"
#include "entity/entity.h"

// 'this' contains the delta transform between the entity I'm following
// and the entity I'm attach to.
// This component has a penalty as it needs to be updated every frame
// to be consistent...
// Do not use it for static meshes!!!!!
struct TCompHierarchy : public TCompBase, public CTransform {
  CHandle     h_parent_transform;
  CHandle     h_my_transform;
  std::string parent_name;

  DECL_SIBLING_ACCESS();

  void onEntityCreated();
  void load(const json& j, TEntityParseContext& ctx);
  void debugInMenu();
  void update(float dt);
};
