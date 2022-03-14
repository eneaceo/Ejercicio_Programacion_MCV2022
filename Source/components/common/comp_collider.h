#pragma once

#include "entity/entity.h"
#include "comp_base.h"
#include "PxPhysicsAPI.h"

struct TMsgEntityOnContact {
	CHandle h_entity;
	DECL_MSG_ID();
};

struct TMsgEntityTriggerEnter {
	CHandle h_entity;
	DECL_MSG_ID();
};

struct TMsgEntityTriggerExit {
	CHandle h_entity;
	DECL_MSG_ID();
};

class TCompCollider : public TCompBase {
public:
DECL_SIBLING_ACCESS();

  json jconfig;
  physx::PxCapsuleController* controller = nullptr;
  physx::PxRigidActor* actor = nullptr;

  ~TCompCollider();
  void load(const json& j, TEntityParseContext& ctx);
  void onEntityCreated();
  void debugInMenu();
  void renderDebug();

  void debugInMenuShape(physx::PxShape* shape, physx::PxGeometryType::Enum geometry_type, const void* geom, MAT44 world);
  void renderDebugShape(physx::PxShape* shape, physx::PxGeometryType::Enum geometry_type, const void* geom, MAT44 world);

  typedef void (TCompCollider::* TShapeFn)(physx::PxShape* shape, physx::PxGeometryType::Enum geometry_type, const void* geom, MAT44 world);
  void onEachShape(TShapeFn fn);
};
