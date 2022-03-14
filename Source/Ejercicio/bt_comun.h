#ifndef _BT_COMUN_INC
#define _BT_COMUN_INC

#include "components/common/comp_base.h"
#include "components/common/comp_transform.h"
#include "components/common/comp_name.h"
#include "components/messages.h"


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

};

#endif