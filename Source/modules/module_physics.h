#pragma once

#include "modules/module.h"
#include "components/common/comp_collider.h"
#include "PxPhysicsAPI.h"
#include "physics/comp_ragdoll.h"

#define VEC3_TO_PXVEC3(vec3) physx::PxVec3(vec3.x, vec3.y, vec3.z)
#define PXVEC3_TO_VEC3(pxvec3) VEC3(pxvec3.x, pxvec3.y, pxvec3.z)

#define QUAT_TO_PXQUAT(quat) physx::PxQuat(quat.x, quat.y, quat.z, quat.w)
#define PXQUAT_TO_QUAT(pxquat) QUAT(pxquat.x, pxquat.y, pxquat.z,pxquat.w)

extern CTransform toTransform(physx::PxTransform pxtrans);
extern physx::PxTransform toPxTransform(CTransform mcvtrans);

class CModulePhysics : public IModule
{
public:
	enum FilterGroup {
		Wall = 1 << 0,
		Floor = 1 << 1,
		Player = 1 << 2,
		Enemy = 1 << 3,
		Projectile = 1 << 4,
		Scenario = Wall | Floor,
		Characters = Player | Enemy,
		All = -1
	};

	physx::PxScene* gScene = NULL;

  FilterGroup getFilterByName(const std::string& name);

  void setupFiltering(physx::PxShape* shape, physx::PxU32 filterGroup, physx::PxU32 filterMask);
  void setupFilteringOnAllShapesOfActor(physx::PxRigidActor* actor, physx::PxU32 filterGroup, physx::PxU32 filterMask);

  CModulePhysics(const std::string& name) : IModule(name) { }
  bool start() override;
  void stop() override;
  physx::PxRigidActor* createController(TCompCollider& comp_collider);
  void createActor(TCompCollider& comp_collider);
  bool readShape(physx::PxRigidActor* actor, const json& jcfg);
  void render() override;
  void renderDebug() override;
  void update(float delta) override;
  //void renderDebug() override;
  void renderInMenu() override;

  void createRagdoll(TCompRagdoll& comp_ragdoll);
  void createRagdollJoints(TCompRagdoll& comp_ragdoll, int bone_id);

  class CustomSimulationEventCallback : public physx::PxSimulationEventCallback
  {
	  // Implements PxSimulationEventCallback
	  virtual void							onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs);
	  virtual void							onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count);
	  virtual void							onConstraintBreak(physx::PxConstraintInfo*, physx::PxU32) {}
	  virtual void							onWake(physx::PxActor**, physx::PxU32) {}
	  virtual void							onSleep(physx::PxActor**, physx::PxU32) {}
	  virtual void							onAdvance(const physx::PxRigidBody* const*, const physx::PxTransform*, const physx::PxU32) {}
  };

  CustomSimulationEventCallback customSimulationEventCallback;

  class CustomQueryFilterCallback : public physx::PxQueryFilterCallback
  {
	  // Implements PxQueryFilterCallback
	  virtual physx::PxQueryHitType::Enum preFilter(const physx::PxFilterData& filterData, const physx::PxShape* shape, const physx::PxRigidActor* actor, physx::PxHitFlags& queryFlags);
	  virtual physx::PxQueryHitType::Enum postFilter(const physx::PxFilterData& filterData, const physx::PxQueryHit& hit) { return physx::PxQueryHitType::eBLOCK; };
  };
  CustomQueryFilterCallback customQueryFilterCallback;

};
