#pragma once

#include "entity/entity.h"
#include "comp_base.h"

class TCompUpdatesPhysics : public TCompBase {
public:
  DECL_SIBLING_ACCESS();

  CHandle h_collider;
  CHandle h_transform;

  enum class eUpdateType
  {
    eTargetIsRigidBody,
    eTargetIsKinematic,
  };
  eUpdateType update_type = eUpdateType::eTargetIsRigidBody;

  void onEntityCreated();
  void update(float dt);
};
