#include "mcv_platform.h"
#include "components/common/comp_base.h"
#include "components/common/comp_transform.h"
#include "components/common/comp_name.h"
#include "entity/entity.h"
#include "render/draw_primitives.h"
#include "entity/entity_parser.h"
#include "components/messages.h"

#include "../Tools/SLB/include/SLB/SLB.hpp"

//BUSCAR SOLUCION A ESTO!!!!!!
CHandle h_gamestats;
CHandle h_player;
CHandle h_spawner;
CHandle h_btmanager;

class TCompLuaManager : public TCompBase {

	// Macro to allow access from this component to other sibling components using the get<T>()
	DECL_SIBLING_ACCESS()

	std::string player_name;
	std::string gamestats_name;
	std::string spawner_name;
	std::string btmanager_name;

	bool bnextWave = false;

public:

	class LogicManager {

		float playerLife;

	public:

		LogicManager() {
			playerLife = 0.0f;
		}

		void SetPlayerLife(float playerLife) {
			TMsgSetLife msg;
			CEntity* msg_target = h_gamestats;
			msg.life = playerLife;
			msg_target->sendMsg(msg);
		}

		void SetPotionHealing(float potionHeal) {
			TMsgSetPotionHeal msg;
			CEntity* msg_target = h_gamestats;
			msg.potionHeal = potionHeal;
			msg_target->sendMsg(msg);
		}

		void SetPlayerHitDamage(float playerHitDamage) {
			TMsgSetHitDamage msg;
			CEntity* msg_target = h_gamestats;
			msg.playerHitDamage = playerHitDamage;
			msg_target->sendMsg(msg);
		}

		float GetPlayerLife() {
			return playerLife;
		}

		void SetPlayerMaxLife(float playerMaxLife) {
			TMsgSetMaxLife msg;
			CEntity* msg_target = h_gamestats;
			msg.maxLife = playerMaxLife;
			msg_target->sendMsg(msg);
		}

		void setSpawnPosition(float px, float py, float pz) {
			TMsgSpawnPosition msg;
			CEntity* msg_target = h_spawner;
			msg.px = px;
			msg.py = py;
			msg.pz = pz;
			msg_target->sendMsg(msg);
		}

		void SpawnRandomEnemies(int numEnemies) {
			TMsgSpawnRandomEnemies msg;
			CEntity* msg_target = h_spawner;
			msg.numEnemies = numEnemies;
			msg_target->sendMsg(msg);
		}

	};


	void BootLuaSLB(SLB::Manager* m) {
		SLB::Class< LogicManager >("LogicManager", m)
			.constructor()
			.set("SetPlayerLife", &LogicManager::SetPlayerLife)
			.set("SetPotionHealing", &LogicManager::SetPotionHealing)
			.set("SetPlayerHitDamage", &LogicManager::SetPlayerHitDamage)
			.set("GetPlayerLife", &LogicManager::GetPlayerLife)
			.set("SetPlayerMaxLife", &LogicManager::SetPlayerMaxLife)
			.set("setSpawnPosition", &LogicManager::setSpawnPosition)
			.set("SpawnRandomEnemies", &LogicManager::SpawnRandomEnemies)
			;

	}

	void onEntityCreated() {

		//SETUP
		h_player = getEntityByName(player_name);
		if (!h_player.isValid()) return;
		h_gamestats = getEntityByName(gamestats_name);
		if (!h_gamestats.isValid()) return;
		h_spawner = getEntityByName(spawner_name);
		if (!h_spawner.isValid()) return;
		h_btmanager = getEntityByName(btmanager_name);
		if (!h_btmanager.isValid()) return;


		//LUA
		SLB::Manager m;
		BootLuaSLB(&m);
		SLB::Script s(&m);

		try {
			//BUSCAR SOLUCION A ESTO!!!!!!!
			s.doFile("C:/Users/eneac/Desktop/EjercicioAbril/Source/test.lua");
		} catch (std::exception x) {
			printf("error %s\n", x.what());
		}

		if (s.exists("OnGameStart")) {
			s.doString("OnGameStart()");
		}

		TMsgSetLuaManager msg;
		CEntity* msg_target = h_btmanager;
		msg.h_luaManager = CHandle(this).getOwner();;
		msg_target->sendMsg(msg);
	}

	void load(const json& j, TEntityParseContext& ctx) {
		//player
		player_name = j.value("player_name", player_name);
		gamestats_name = j.value("gamestats_name", gamestats_name);
		spawner_name = j.value("spawner_name", spawner_name);
		btmanager_name = j.value("btmanager_name", btmanager_name);
	}

	static void registerMsgs() {
		DECL_MSG(TCompLuaManager, TMsgNextWave, nextWave);
	}

	void nextWave(const TMsgNextWave& msg) {
		SLB::Manager m;
		BootLuaSLB(&m);
		SLB::Script s(&m);
		try {
			//BUSCAR SOLUCION A ESTO!!!!!!!
			s.doFile("C:/Users/eneac/Desktop/EjercicioAbril/Source/test.lua");
		} catch (std::exception x) {
			printf("error %s\n", x.what());
		}
		if (s.exists("NextWave")) {
			s.doString("NextWave()");
		}
	}

};

DECL_OBJ_MANAGER("luamanager", TCompLuaManager)