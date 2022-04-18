#pragma once

#include "components/common/comp_base.h"
#include "components/common/comp_transform.h"
#include "components/common/comp_collider.h"
#include "components/messages.h"
#include "entity/entity.h"

class TCompBulletControllerPlayer : public TCompBase
{
	DECL_SIBLING_ACCESS();
	

	std::string btmanager_name;
	CHandle h_btmanager;
	CHandle my_transform;

	bool destroy = false;
    float speed = 1.f;
	void OnTriggerEnter(const TMsgEntityTriggerEnter& msg);
	//void OnTriggerExit(const TMsgEntityTriggerExit& msg);
public:
	static void registerMsgs();
	void load(const json& j, TEntityParseContext& ctx);
	void update(float dt);
	void onEntityCreated();
	void debugInMenu();
};

