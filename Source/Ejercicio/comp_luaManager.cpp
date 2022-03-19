#include "mcv_platform.h"
#include "components/common/comp_base.h"
#include "components/common/comp_transform.h"
#include "components/common/comp_name.h"
#include "entity/entity.h"
#include "render/draw_primitives.h"
#include "entity/entity_parser.h"
#include "components/messages.h"

#include "../Tools/SLB/include/SLB/SLB.hpp"

//BUSCAR SOLUCION A ESTO!!!!
CHandle h_gamestats;
CHandle h_player;
CHandle h_spawner;
CHandle h_btmanager;
CHandle h_player_controller;

bool moveForward = false;
VEC3 targetPoint;

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
		float playerMaxLife;
		float playerHitDamage;
		float playerShootHitDamage;
		float potionHeal;
		bool spawnState;

	public:


		LogicManager() {
			playerLife = 0.0f;
			playerMaxLife = 0.0f;
			playerHitDamage = 0.0f;
			playerShootHitDamage = 0.0f;
			potionHeal = 0.0f;
			spawnState = false;
		}

		void SetPlayerLife(float playerLife) {
			this->playerLife = playerLife;
			TMsgSetLife msg;
			CEntity* msg_target = h_gamestats;
			msg.life = this->playerLife;
			msg_target->sendMsg(msg);
		}

		void SetPlayerMaxLife(float playerMaxLife) {
			this->playerMaxLife = playerMaxLife;
			TMsgSetMaxLife msg;
			CEntity* msg_target = h_gamestats;
			msg.maxLife = this->playerMaxLife;
			msg_target->sendMsg(msg);
		}

		float GetPlayerLife() {
			return playerLife;
		}

		float GetPlayerMaxLife() {
			return playerMaxLife;
		}

		void SetPotionHealing(float potionHeal) {
			this->potionHeal = potionHeal;
			TMsgSetPotionHeal msg;
			CEntity* msg_target = h_gamestats;
			msg.potionHeal = potionHeal;
			msg_target->sendMsg(msg);
		}

		void SetPlayerShootHitDamage(float playerShootHitDamage) {
			this->playerShootHitDamage = playerShootHitDamage;
			TMsgSetShootHitDamage msg;
			CEntity* msg_target = h_gamestats;
			msg.playerShootHitDamage = playerShootHitDamage;
			msg_target->sendMsg(msg);
		}

		void SetPlayerHitDamage(float playerHitDamage) {
			this->playerHitDamage = playerHitDamage;
			TMsgSetHitDamage msg;
			CEntity* msg_target = h_gamestats;
			msg.playerHitDamage = playerHitDamage;
			msg_target->sendMsg(msg);
		}

		void setSpawnState(bool state) {
			spawnState = state;
			TMsgSpawnState msg;
			CEntity* msg_target = h_spawner;
			msg.state = state;
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

		void MovePlayer(float px, float py, float pz) {
			TMsgMove msg;
			CEntity* msg_target = h_player;
			msg.newPos = VEC3(px,py,pz);
			msg_target->sendMsg(msg);
		}

		void DespawnEnemies() {
			TMsgDespawnEnemies msg;
			CEntity* msg_target = h_btmanager;
			msg_target->sendMsg(msg);
		}

	};

	void BootLuaSLB(SLB::Manager* m) {
		SLB::Class< LogicManager >("LogicManager", m)
			.constructor()
			.set("SetPlayerLife", &LogicManager::SetPlayerLife)
			.set("GetPlayerLife", &LogicManager::GetPlayerLife)
			.set("SetPlayerMaxLife", &LogicManager::SetPlayerMaxLife)
			.set("GetPlayerMaxLife", &LogicManager::GetPlayerMaxLife)
			.set("SetPlayerHitDamage", &LogicManager::SetPlayerHitDamage)
			.set("SetPlayerShootHitDamage", &LogicManager::SetPlayerShootHitDamage)
			.set("setSpawnPosition", &LogicManager::setSpawnPosition)
			.set("setSpawnState", &LogicManager::setSpawnState)
			.set("SpawnRandomEnemies", &LogicManager::SpawnRandomEnemies)
			.set("MovePlayer", &LogicManager::MovePlayer)
			.set("SetPotionHealing", &LogicManager::SetPotionHealing)
			.set("DespawnEnemies", &LogicManager::DespawnEnemies)
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
			s.doFile("D:/Universidad/EjercicioAbril/Source/test.lua");
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

		msg_target = h_gamestats;
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
		DECL_MSG(TCompLuaManager, TMsgPlayerDead, playerDead);
	}

	void playerDead(const TMsgPlayerDead& msg) {

		SLB::Manager m;
		BootLuaSLB(&m);
		SLB::Script s(&m);

		try {
			//BUSCAR SOLUCION A ESTO!!!!!!!
			s.doFile("D:/Universidad/EjercicioAbril/Source/test.lua");
		} catch (std::exception x) {
			printf("error %s\n", x.what());
		}

		if (s.exists("PlayerDead")) {
			s.doString("PlayerDead()");
		}
	}

	void nextWave(const TMsgNextWave& msg) {

		SLB::Manager m;
		BootLuaSLB(&m);
		SLB::Script s(&m);

		try {
			//BUSCAR SOLUCION A ESTO!!!!!!!
			s.doFile("D:/Universidad/EjercicioAbril/Source/test.lua");
		} catch (std::exception x) {
			printf("error %s\n", x.what());
		}

		if (s.exists("NextWave")) {
			s.doString("NextWave()");
		}
	}

};

DECL_OBJ_MANAGER("luamanager", TCompLuaManager)