#include "mcv_platform.h"
#include "bt_playerController.h"


void bt_playerController::Init(CHandle h_my_transform, CHandle h_collider, float speed) {

	CreateRootNode("root", SELECTOR);
	//Cinematic movement
	AddDecoratorNode("root", "dec_cinema", (bttask)&bt_playerController::DecoratorCinema);
	AddTaskNode("dec_cinema", "cinema", (bttask)&bt_playerController::TaskCinema);
	//Attack
	AddDecoratorNode("root", "dec_attack", (bttask)&bt_playerController::DecoratorAttack);
	AddTaskNode("dec_attack", "attack", (bttask)&bt_playerController::TaskAttack);
	//Movement
	AddDecoratorNode("root", "dec_movement", (bttask)&bt_playerController::DecoratorMovement);
	AddTaskNode("dec_movement", "movement", (bttask)&bt_playerController::TaskMovement);

	this->h_my_transform = h_my_transform;
	this->speed = speed;
	this->h_collider = h_collider;
}

void bt_playerController::updateTime(float dt) {
	this->dt = dt;
}

//Decorators

int bt_playerController::DecoratorMovement() {
	if (isPressed('W') || isPressed('A') || isPressed('S') || isPressed('D')) return SUCCESS;
	else return FAIL;
}

int bt_playerController::DecoratorAttack() {
	if (isPressed(' ')) return SUCCESS;
	else return FAIL;
}

//Tasks

int bt_playerController::TaskMovement() {

	TCompTransform* t = h_my_transform;
	VEC3 new_move;

	if (isPressed('W'))
		new_move = t->getForward() * speed * dt;
	if (isPressed('S'))
		new_move = -t->getForward() * speed * dt;

	t->setPosition(t->getPosition() + new_move);


	TCompCollider* compCollider = h_collider;
	if (compCollider != nullptr && compCollider->controller) {
		new_move.y += -9.81f * dt;

		static const physx::PxU32 max_shapes = 8;
		physx::PxShape* shapes[max_shapes];

		physx::PxU32 nshapes = compCollider->controller->getActor()->getNbShapes();
		assert(nshapes <= max_shapes);

		// Even when the buffer is small, it writes all the shape pointers
		physx::PxU32 shapes_read = compCollider->controller->getActor()->getShapes(shapes, sizeof(shapes), 0);

		// Make a copy of the pxFilterData because the result of getQueryFilterData is returned by copy
		physx::PxFilterData filterData = shapes[0]->getQueryFilterData();
		physx::PxControllerFilters controllerFilters(&filterData, &CEngine::get().getPhysics().customQueryFilterCallback);
		compCollider->controller->move(physx::PxVec3(new_move.x, new_move.y, new_move.z), 0.0f, dt, controllerFilters);
	}
	else {
		t->setPosition(t->getPosition() + new_move);
	}


	float rotation_speed = 3.0f;

	if (isPressed('A')) {
		QUAT delta_rotation = QUAT::CreateFromAxisAngle(VEC3(0, 1, 0), rotation_speed * dt);
		t->setRotation(t->getRotation() * delta_rotation);
	}
	if (isPressed('D')) {
		QUAT delta_rotation = QUAT::CreateFromAxisAngle(VEC3(0, 1, 0), -rotation_speed * dt);
		t->setRotation(t->getRotation() * delta_rotation);
	}

	return SUCCESS;
}

int bt_playerController::TaskAttack() {

	if (timer < 0.5f) {
		timer = dt + timer;
		return IN_PROGRESS;
	}
	else {
		timer = 0;
		return SUCCESS;
	}
}

//Cimeatic movement

void bt_playerController::setCinematicMovement(bool cinematic, VEC3 newPos) {
	this->cinematic = cinematic;
	cinematicPosTarget = newPos;
}

int bt_playerController::DecoratorCinema() {
	if (cinematic) return SUCCESS;
	else return FAIL;
}

int bt_playerController::TaskCinema() {

	//rotate to target
	TCompTransform* my_transform = h_my_transform;
	float angle_to_aim = my_transform->getYawRotationToAimTo(cinematicPosTarget);
	angle_to_aim *= turn_speed;
	QUAT my_rot = my_transform->getRotation();
	my_rot *= QUAT::CreateFromAxisAngle(VEC3::UnitY, angle_to_aim);
	my_transform->setRotation(my_rot);
	//move forward
	float amount_moved = speed * dt;
	VEC3 delta_moved = my_transform->getForward() * amount_moved;
	my_transform->setPosition(my_transform->getPosition() + delta_moved);

	TCompCollider* compCollider = h_collider;
	if (compCollider != nullptr && compCollider->controller) {
		delta_moved.y += -9.81f * dt;

		static const physx::PxU32 max_shapes = 8;
		physx::PxShape* shapes[max_shapes];

		physx::PxU32 nshapes = compCollider->controller->getActor()->getNbShapes();
		assert(nshapes <= max_shapes);

		// Even when the buffer is small, it writes all the shape pointers
		physx::PxU32 shapes_read = compCollider->controller->getActor()->getShapes(shapes, sizeof(shapes), 0);

		// Make a copy of the pxFilterData because the result of getQueryFilterData is returned by copy
		physx::PxFilterData filterData = shapes[0]->getQueryFilterData();
		physx::PxControllerFilters controllerFilters(&filterData, &CEngine::get().getPhysics().customQueryFilterCallback);
		compCollider->controller->move(physx::PxVec3(delta_moved.x, delta_moved.y, delta_moved.z), 0.0f, dt, controllerFilters);
	}


	float dist = VEC3::Distance(my_transform->getPosition(), cinematicPosTarget);

	if (dist < 0.1f) {
		cinematic = false;
		cinematicPosTarget = VEC3(0,0,0);
		return SUCCESS;
	}
	else return IN_PROGRESS;

}