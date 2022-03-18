#ifndef _BT_PLAYERCONTROLLER_INC
#define _BT_PLAYERCONTROLLER_INC

#include "components/common/comp_base.h"
#include "components/common/comp_transform.h"
#include "components/common/comp_collider.h"
#include "engine.h"
#include "modules/module_physics.h"
#include "entity/entity.h"
#include "entity/entity_parser.h"
#include "input/input_module.h"
#include "BehaviorTree/bt.h"


class bt_playerController :public bt {

	CHandle h_my_transform;
	CHandle h_collider;

	float speed;
	float turn_speed = 0.2f;
	float dt;

	float timer;
	bool cinematic = false;
	VEC3 cinematicPosTarget;

public:

	void Init(CHandle,CHandle, float);
	void updateTime(float);

	void setCinematicMovement(bool, VEC3);

	//bt decorators
	int DecoratorCinema();
	int DecoratorMovement();
	int DecoratorAttack();

	//bt tasks
	int TaskCinema();
	int TaskMovement();
	int TaskAttack();

};

#endif
