#include "mcv_platform.h"
#include "components/common/comp_base.h"
#include "components/common/comp_transform.h"
#include "components/common/comp_name.h"
#include "entity/entity.h"
#include "render/draw_primitives.h"
#include "entity/entity_parser.h"
#include "components/messages.h"
#include "components/common/comp_collider.h"


class TCompPowerUpDefense : public TCompBase {

	// Macro to allow access from this component to other sibling components using the get<T>()
	DECL_SIBLING_ACCESS()

	std::string gamestats_name;
	CHandle h_gamestats;
	bool powerup = false;

public:

	void onEntityCreated() {
		h_gamestats = getEntityByName(gamestats_name);
		if (!h_gamestats.isValid()) return;
	}

	void load(const json& j, TEntityParseContext& ctx) {
		//player life
		gamestats_name = j.value("gamestats_name", gamestats_name);
	}

	static void registerMsgs() {
		DECL_MSG(TCompPowerUpDefense, TMsgEntityTriggerEnter, OnTriggerEnter);
	}

	void OnTriggerEnter(const TMsgEntityTriggerEnter& msg) {
		TMsgPowerUpDefense powerup;
		CEntity* msg_target = h_gamestats;
		msg_target->sendMsg(powerup);
		CHandle(this).getOwner().destroy();
	}

};

DECL_OBJ_MANAGER("powerup_defense", TCompPowerUpDefense)