#include "mcv_platform.h"
#include "components/common/comp_base.h"
#include "components/common/comp_transform.h"
#include "components/common/comp_name.h"
#include "entity/entity.h"
#include "render/draw_primitives.h"
#include "entity/entity_parser.h"
#include "components/messages.h"

#include "bt_defensiveEnemy.h"

class TCompBTdefensive : public TCompBase {

	// Macro to allow access from this component to other sibling components using the get<T>()
	DECL_SIBLING_ACCESS()

	bt_defensiveEnemy bt;
	std::string state;
	CHandle attacker;

	std::string target_name;
	CHandle h_target;
	std::string btmanager_name;
	CHandle h_btmanager;

	//Stats
	float speed;
	float turn_speed;
	float life;

	bool kill = false;

public:

	void onEntityCreated() {

		bt.InitTree();
		// speed - turn_speed - life
		bt.setStats(speed, turn_speed, life);

		h_target = getEntityByName(target_name);
		if (!h_target.isValid()) return;
		h_btmanager = getEntityByName(btmanager_name);
		if (!h_btmanager.isValid()) return;

		CHandle myh_transform = get<TCompTransform>();
		CHandle myh_entity = CHandle(this).getOwner();

		bt.Init(h_target, myh_transform, myh_entity, h_btmanager);
		
		//Register in bt_manager
		TMsgEngage msg;
		CEntity* msg_target = h_btmanager;
		msg.me = myh_entity;
		msg_target->sendMsg(msg);

	}

	void load(const json& j, TEntityParseContext& ctx) {

		//player
		target_name = j.value("target_name", target_name);
		//btmanager
		btmanager_name = j.value("btmanager_name", btmanager_name);
		//Stats
		life = j.value("life", life);
		speed = j.value("speed", speed);
		turn_speed = j.value("turn_speed", turn_speed);
	}

	//debug
	void debugInMenu() {
		ImGui::Text("My state is %s", state.c_str());
		if (ImGui::SmallButton("Kill")) {
			bt.setLife(0);
		}
		ImGui::SameLine();
		if (ImGui::SmallButton("Impact")) {
			bt.setImpact(true);
		}
	}

	void renderDebug() {
		PROFILE_FUNCTION("renderDebug");
	}

	void update(float delta_time) {
		PROFILE_FUNCTION("update");

		if (kill) {
			CHandle(this).getOwner().destroy();
			return;
		}
		
		bt.updateTime(delta_time);
		bt.updateAttacker(attacker);

		if (bt.getImpact()) {
			bt.setCurrent(bt.findNode("root"));
		}

		bt.recalc();
		state = bt.getState();

		bt.setImpact(false);
	}

	//Msgs go here
	static void registerMsgs() {
		DECL_MSG(TCompBTdefensive, TMsgAttacker, whoIsAttacker);
		DECL_MSG(TCompBTdefensive, TMsgDestroyMe, destroyMe);
	}

	void whoIsAttacker(const TMsgAttacker& msg) {
		attacker = msg.attacker;
	}

	void destroyMe(const TMsgDestroyMe& msg) {
		kill = true;
	}

};

DECL_OBJ_MANAGER("bt_defensive", TCompBTdefensive)