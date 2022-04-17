#include "mcv_platform.h"
#include "components/common/comp_base.h"
#include "components/common/comp_transform.h"
#include "components/common/comp_name.h"
#include "entity/entity.h"
#include "render/draw_primitives.h"
#include "entity/entity_parser.h"
#include "components/messages.h"
#include "components/common/comp_collider.h"


class TCompGameStats : public TCompBase {

	// Macro to allow access from this component to other sibling components using the get<T>()
	DECL_SIBLING_ACCESS()

	//STATS
	//Life
	int life = 0;
	int maxLife = 0;

	//Defense
	int shootHitDamage = 0;
	int hitDamage = 0;

	//Items
	int potionHeal = 0;
	
	bool potion = false;
	bool defensePowerUp = false;
	bool shootHit = false;
	
	bool respawnPotion = false;
	float timerRespawnPotion = 0.0f;

	bool respawnPowerUp = false;
	bool powerUpActive = false;
	float timerPowerUp = 0.0f;
	float timerPowerUpTime = 30.0f;
	float timerRespawnPowerUp = 0.0f;

	float timerRespawn = 20.0f;

	//Score
	int score = 0;
	bool scoreUp = false;

	CHandle h_my_transform;
	CHandle h_luaManager;

public:

	void update(float dt) {

		if (potion && life < maxLife) {
			life += potionHeal;
			if (life > maxLife) life = maxLife;
			potion = false;
			respawnPotion = true;
		}

		if (respawnPotion) {
			timerRespawnPotion += dt;
		}
		if (timerRespawnPotion > timerRespawn) {
			TEntityParseContext ctx;
			parseScene("data/scenes/potion.json", ctx);
			respawnPotion = false;
			timerRespawnPotion = 0.0f;
		}

		if (shootHit) {
			life -= shootHitDamage;
			shootHit = false;
		}
		
		if (defensePowerUp) {
			hitDamage = hitDamage / 2;
			shootHitDamage = shootHitDamage / 2;
			defensePowerUp = false;
			powerUpActive = true;
		}
		if (powerUpActive) {
			timerPowerUp += dt;
		}
		if (timerPowerUp > timerPowerUpTime) {
			hitDamage = hitDamage * 2;
			shootHitDamage = shootHitDamage * 2;
			timerPowerUp = 0.0f;
			powerUpActive = false;
			respawnPowerUp = true;
		}

		if (respawnPowerUp) {
			timerRespawnPowerUp += dt;
		}
		if (timerRespawnPowerUp > timerRespawn) {
			TEntityParseContext ctx;
			parseScene("data/scenes/powerup_defense.json", ctx);
			respawnPowerUp = false;
			timerRespawnPowerUp = 0.0f;
		}

		if (scoreUp) {
			score += 100;
			scoreUp = false;
		}

		if (life <= 0) {
			TMsgPlayerDead msg;
			CEntity* msg_target = h_luaManager;
			msg_target->sendMsg(msg);
		}

	}

	void debugInMenu() {
		ImGui::Text("Life: %d", life);
		ImGui::SameLine();
		ImGui::Text("MaxLife: %d", maxLife);
		ImGui::Text("Hit damage: %d", hitDamage);
		ImGui::SameLine();
		ImGui::Text("Shoot hit damage: %d", shootHitDamage);
		ImGui::Text("Potion heal: %d", potionHeal);
		ImGui::Text("Score: %d", score);
		if (ImGui::SmallButton("-10")) {
			life -= 10;
		}

	}

	static void registerMsgs() {
		DECL_MSG(TCompGameStats, TMsgSetLuaManager, setLuaManager);
		DECL_MSG(TCompGameStats, TMsgPotion, Potion);
		DECL_MSG(TCompGameStats, TMsgPowerUpDefense, DefensePowerUp);
		DECL_MSG(TCompGameStats, TMsgShootHit, ShootHit);
		DECL_MSG(TCompGameStats, TMsgSetLife, SetLife);
		DECL_MSG(TCompGameStats, TMsgSetMaxLife, SetMaxLife);
		DECL_MSG(TCompGameStats, TMsgSetPotionHeal, SetPotionHeal);
		DECL_MSG(TCompGameStats, TMsgSetShootHitDamage, SetShootHitDamage);
		DECL_MSG(TCompGameStats, TMsgSetHitDamage, SetHitDamage);
		DECL_MSG(TCompGameStats, TMsgKill, enemieKilled);
	}

	void setLuaManager(const TMsgSetLuaManager& msg) {
		h_luaManager = msg.h_luaManager;
	}

	void Potion(const TMsgPotion& msg) {
		potion = true;
	}

	void DefensePowerUp(const TMsgPowerUpDefense& msg) {
		defensePowerUp = true;
	}

	void ShootHit(const TMsgShootHit& msg) {
		shootHit = true;
	}

	void SetLife(const TMsgSetLife& msg) {
		life = msg.life;
	}

	void SetMaxLife(const TMsgSetMaxLife& msg) {
		maxLife = msg.maxLife;
	}

	void SetPotionHeal(const TMsgSetPotionHeal& msg) {
		potionHeal = msg.potionHeal;
	}

	void SetHitDamage(const TMsgSetHitDamage& msg) {
		hitDamage = msg.playerHitDamage;
	}

	void SetShootHitDamage(const TMsgSetShootHitDamage& msg) {
		shootHitDamage = msg.playerShootHitDamage;
	}

	void enemieKilled(const TMsgKill& msg) {
		scoreUp = true;
	}

};

DECL_OBJ_MANAGER("gamestats", TCompGameStats)