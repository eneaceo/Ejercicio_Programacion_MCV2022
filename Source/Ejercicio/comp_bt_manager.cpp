#include "mcv_platform.h"
#include "components/common/comp_base.h"
#include "components/common/comp_transform.h"
#include "components/common/comp_name.h"
#include "entity/entity.h"
#include "render/draw_primitives.h"
#include "entity/entity_parser.h"
#include "components/messages.h"



class TCompBTmanager : public TCompBase {

	// Macro to allow access from this component to other sibling components using the get<T>()
	DECL_SIBLING_ACCESS()

	CHandle h_luaManager;
	CHandle h_gamestats;
	std::string gamestats_name;

	CHandle attacker;
	std::string attackerSlot;
	std::string attacker_name;
	int enemiesEngaged;
	int enemiesEngagedBefore;
	std::vector<CHandle> whoIsEngaged;
	bool freeAttackerSlot = false;

public:

	void onEntityCreated() {
		h_gamestats = getEntityByName(gamestats_name);
		if (!h_gamestats.isValid()) return;
	}

	void load(const json& j, TEntityParseContext& ctx) {
		gamestats_name = j.value("gamestats_name", gamestats_name);
	}

	//debug
	void debugInMenu() {

		ImGui::Text("Enemies engaged: %d", enemiesEngaged);
		
		if (!attacker.isValid())
			return;
		CEntity* e_attacker = attacker;
		attacker_name = e_attacker->getName();
		ImGui::Text("Attacker name: %s", attacker_name.c_str());
	}
	
	void update(float delta_time) {

		if (freeAttackerSlot) {
			attackerSlot.clear();
			freeAttackerSlot = false;
		}

		enemiesEngaged = whoIsEngaged.size();

		for (CHandle c : whoIsEngaged) {
			TMsgAttacker msg;
			CEntity* msg_target = c;
			msg.attacker = attacker;
			msg_target->sendMsg(msg);
		}

		if (enemiesEngaged == 0) {
			TMsgNextWave msg;
			CEntity* msg_target = h_luaManager;
			msg_target->sendMsg(msg);
		}

		if (enemiesEngaged > enemiesEngagedBefore) {
			enemiesEngagedBefore = enemiesEngaged;
		} 
		if (enemiesEngaged < enemiesEngagedBefore) {
			enemiesEngagedBefore = enemiesEngaged;
			TMsgKill msg;
			CEntity* msg_target = h_gamestats;
			msg_target->sendMsg(msg);
		}

	}

	static void registerMsgs() {
		DECL_MSG(TCompBTmanager, TMsgEngage, registerEngage);
		DECL_MSG(TCompBTmanager, TMsgAskForAttacker, answerAttacker);
		DECL_MSG(TCompBTmanager, TMsgDying, dying);
		DECL_MSG(TCompBTmanager, TMsgChangeAttacker, changeAttacker);
		DECL_MSG(TCompBTmanager, TMsgSetLuaManager, setLuaManager);
	}

	void registerEngage(const TMsgEngage& msg) {
		whoIsEngaged.push_back(msg.me);
	}

	void answerAttacker(const TMsgAskForAttacker& msg) {
		if (attackerSlot.empty()) {
			CEntity* e_attacker = msg.me;
			attackerSlot = e_attacker->getName();
			attacker = msg.me;
		}
	}

	void dying(const TMsgDying& msg) {
		whoIsEngaged.erase(std::remove(whoIsEngaged.begin(), whoIsEngaged.end(), msg.me), whoIsEngaged.end());
		if (attacker == msg.me) freeAttackerSlot = true;
	}

	void changeAttacker(const TMsgChangeAttacker& msg) {
			CEntity* e_attacker = msg.me;
			attackerSlot = e_attacker->getName();
			attacker = msg.me;
	}

	void setLuaManager(const TMsgSetLuaManager& msg) {
		h_luaManager = msg.h_luaManager;
	}

};

DECL_OBJ_MANAGER("bt_manager", TCompBTmanager)