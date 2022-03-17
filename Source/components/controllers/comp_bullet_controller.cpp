#include "mcv_platform.h"
#include "components/controllers/comp_bullet_controller.h"
#include "modules/module_physics.h"

DECL_OBJ_MANAGER("bullet_controller", TCompBulletController)

void TCompBulletController::registerMsgs() {
	DECL_MSG(TCompBulletController, TMsgEntityTriggerEnter, OnTriggerEnter);
	//DECL_MSG(TCompBulletController, TMsgEntityTriggerExit, OnTriggerExit);
}

void TCompBulletController::OnTriggerEnter(const TMsgEntityTriggerEnter& msg) {
	//dbg("Hit\n");
	TMsgShootHit hit;
	CEntity* msg_target = h_gamestats;
	msg_target->sendMsg(hit);
	destroy = true;
}

//void TCompBulletController::OnTriggerExit(const TMsgEntityTriggerExit& msg) {}

void TCompBulletController::load(const json& j, TEntityParseContext& ctx)
{
    speed = j.value("speed", speed);
	//player life
	gamestats_name = j.value("gamestats_name", gamestats_name);
}

void TCompBulletController::onEntityCreated() {
	h_gamestats = getEntityByName(gamestats_name);
	if (!h_gamestats.isValid()) return;
}

void TCompBulletController::update(float dt)
{
	// Update the comp
    TCompTransform* c_trans = get<TCompTransform>();
	VEC3 new_pos = c_trans->getPosition() + speed * c_trans->getForward() * dt;
	c_trans->setPosition(new_pos);

	// Do we have a collider
	TCompCollider* c_collider = get<TCompCollider>();
	if (c_collider && c_collider->actor) {
		physx::PxRigidDynamic* rigid_dynamic = (physx::PxRigidDynamic*)c_collider->actor;
		rigid_dynamic->setGlobalPose(toPxTransform(*c_trans));
	}

	if (destroy) {
		CHandle(this).getOwner().destroy();
		return;
	}
}

void TCompBulletController::debugInMenu()
{
    ImGui::DragFloat("Speed", &speed);
}

