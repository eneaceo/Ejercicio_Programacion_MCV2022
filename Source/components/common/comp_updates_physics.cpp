#include "mcv_platform.h"
#include "comp_updates_physics.h"
#include "comp_collider.h"
#include "comp_transform.h"
#include "modules/module_physics.h"		// toPxTransform

DECL_OBJ_MANAGER("updates_physics", TCompUpdatesPhysics)

using namespace physx;

void TCompUpdatesPhysics::onEntityCreated()
{
	// Get the handle to my components now that the entity has been created
	h_collider = get<TCompCollider>();
	h_transform = get<TCompTransform>();
	TCompCollider* c_collider = h_collider;
	assert(c_collider);
	assert(c_collider->controller == nullptr);
	bool isKinematic = c_collider->jconfig.value("kinematic", false);
	update_type = isKinematic 
	? eUpdateType::eTargetIsKinematic 
	: eUpdateType::eTargetIsRigidBody;
}

void TCompUpdatesPhysics::update(float dt)
{
	TCompCollider* c_collider = h_collider;
	TCompTransform* c_transform = h_transform;
	assert( c_collider && c_transform );
	assert(c_collider->actor);
	PxTransform pose = toPxTransform(*c_transform);
	switch (update_type)
	{
	case eUpdateType::eTargetIsRigidBody: {
		c_collider->actor->setGlobalPose(pose);
		break; }

	case eUpdateType::eTargetIsKinematic: {
		PxRigidDynamic* rigid_dynamic = (PxRigidDynamic*)(c_collider->actor);
		rigid_dynamic->setKinematicTarget(pose);
		break; }
	}

}