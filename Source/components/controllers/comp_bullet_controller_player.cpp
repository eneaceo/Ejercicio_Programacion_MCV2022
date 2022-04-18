#include "mcv_platform.h"
#include "components/controllers/comp_bullet_controller_player.h"
#include "modules/module_physics.h"

DECL_OBJ_MANAGER("bullet_controller_player", TCompBulletControllerPlayer)

void TCompBulletControllerPlayer::registerMsgs() {
	DECL_MSG(TCompBulletControllerPlayer, TMsgEntityTriggerEnter, OnTriggerEnter);
	//DECL_MSG(TCompBulletController, TMsgEntityTriggerExit, OnTriggerExit);
}

void TCompBulletControllerPlayer::OnTriggerEnter(const TMsgEntityTriggerEnter& msg) {
	//dbg("Hit\n");
	TMsgShootHitPlayer hit;
	TCompTransform* c_trans = get<TCompTransform>();
	hit.pos = c_trans->getPosition();
	CEntity* msg_target = h_btmanager;
	msg_target->sendMsg(hit);
	destroy = true;
}

//void TCompBulletController::OnTriggerExit(const TMsgEntityTriggerExit& msg) {}

void TCompBulletControllerPlayer::load(const json& j, TEntityParseContext& ctx)
{
    speed = j.value("speed", speed);
	btmanager_name = j.value("btmanager_name", btmanager_name);

}

void TCompBulletControllerPlayer::onEntityCreated() {

	h_btmanager = getEntityByName(btmanager_name);
	if (!h_btmanager.isValid()) return;
}

void TCompBulletControllerPlayer::update(float dt)
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

void TCompBulletControllerPlayer::debugInMenu()
{
    ImGui::DragFloat("Speed", &speed);
}

