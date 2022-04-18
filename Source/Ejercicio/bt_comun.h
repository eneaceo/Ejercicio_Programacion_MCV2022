#ifndef _BT_COMUN_INC
#define _BT_COMUN_INC

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


class bt_comun {

	CHandle h_target;

	CHandle myh_transform;
	CHandle myh_entity;

	float delta_time;
	float timer;

	//stats
	float speed;
	float turn_speed;
	float life;

	//comunication
	CHandle h_btmanager;
	CHandle attacker;
	std::string state;
	bool impact;
	float damage = 0;

	bool dying = false;
	bool animEnded = false;
	bool playingAnim = false;
	int movementDir = 0;
	bool anim = false;

public:

	void Init(CHandle, CHandle, CHandle, CHandle);
	void setStats(float, float, float);

	float getLife();
	void setLife(float);
	bool getImpact();
	void setImpact(bool);
	float getDamage();
	void setDamage(float);
	float getTimer();
	void setTimer(float);
	void resetTimer();
	float getDeltaTime();
	
	void updateTime(float);
	bool iAmAttacker();
	void updateAttacker(CHandle);
	void askForAttacker();
	bool attackerSlotFree();
	void changeAttacker();
	void msgDying();
	void msgDestroyMe();
	void msgAnimation(int, float, float);
	void msgAnimationMovement(int, float, float);
	void setAnimEnded(bool);
	bool getAnimEnded();
	void setPLayingAnim(bool);
	bool getPlayingAnim();
	void setMovementDir(int);
	int getMovementDir();
	void setDying();
	bool getDying();
	void animAux(bool);
	bool getAnimAux();

	VEC3 getMyPosition();
	VEC3 getMyForward();
	VEC3 getEnemyPosition();
	void rotateAroundRight(VEC3);
	void rotateAroundLeft(VEC3);
	void rotateToTarget(VEC3);
	void snapRotateToTarget(VEC3);
	void moveForward();
	void moveBackwards();

	std::string getState();
	void setState(std::string);

	void shoot();
	void attack();

};

#endif