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
#include "components/messages.h"
#include "BehaviorTree/bt.h"
#include "skeleton/comp_skeleton.h"
#include "cal3d/cal3d.h"
#include "skeleton/game_core_skeleton.h"
#include "render/draw_primitives.h"
#include "components/common/comp_transform.h"
#include "components/common/comp_aabb.h"
#include "skeleton/comp_skeleton_ik.h"
#include "skeleton/comp_skel_lookat.h"
#include "Ejercicio/bt_playerController.h"


class bt_playerController :public bt {

	CHandle h_my_transform;
	CHandle h_collider;
	CHandle h_skel;

	float speed;
	float turn_speed = 0.2f;
	float dt;

	float timer;
	bool cinematic = false;
	VEC3 cinematicPosTarget;

public:

	void Init(CHandle,CHandle, float, CHandle);
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
