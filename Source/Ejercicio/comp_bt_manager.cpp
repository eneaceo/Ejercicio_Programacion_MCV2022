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

	CHandle attacker;
	std::string attackerSlot;
	std::string attacker_name;
	int enemiesEngaged;
	std::vector<CHandle> whoIsEngaged;

	bool freeAttackerSlot = false;

public:

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
	}

	static void registerMsgs() {
		DECL_MSG(TCompBTmanager, TMsgEngage, registerEngage);
		DECL_MSG(TCompBTmanager, TMsgAskForAttacker, answerAttacker);
		DECL_MSG(TCompBTmanager, TMsgDying, dying);
		DECL_MSG(TCompBTmanager, TMsgChangeAttacker, changeAttacker);
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

};

DECL_OBJ_MANAGER("bt_manager", TCompBTmanager)