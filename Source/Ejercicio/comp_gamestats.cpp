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

	int life = 0;
	int maxLife = 0;
	int potionHeal = 0;
	int hitDamage = 0;
	bool potion = false;
	bool defensePowerUp = false;
	bool shootHit = false;
	bool scoreUp = false;
	int score = 0;

public:

	void load(const json& j, TEntityParseContext& ctx) {
		//life = j.value("life", life);
		//maxLife = j.value("maxLife", maxLife);
	}

	void update(float dt) {
		if (potion && life < maxLife) {
			life += potionHeal;
			if (life > maxLife) life = maxLife;
			potion = false;
		}
		if (shootHit) {
			life -= hitDamage;
			shootHit = false;
		}
		if (defensePowerUp) {
			hitDamage = hitDamage / 2;
			defensePowerUp = false;
		}
		if (scoreUp) {
			score += 1000;
			scoreUp = false;
		}
	}

	void debugInMenu() {
		ImGui::Text("Life: %d", life);
		ImGui::SameLine();
		ImGui::Text("MaxLife: %d", maxLife);
		ImGui::Text("Score: %d", score);
		if (ImGui::SmallButton("-10")) {
			life -= 10;
		}

	}

	static void registerMsgs() {
		DECL_MSG(TCompGameStats, TMsgPotion, Potion);
		DECL_MSG(TCompGameStats, TMsgPowerUpDefense, DefensePowerUp);
		DECL_MSG(TCompGameStats, TMsgShootHit, ShootHit);
		DECL_MSG(TCompGameStats, TMsgSetLife, SetLife);
		DECL_MSG(TCompGameStats, TMsgSetMaxLife, SetMaxLife);
		DECL_MSG(TCompGameStats, TMsgSetPotionHeal, SetPotionHeal);
		DECL_MSG(TCompGameStats, TMsgSetHitDamage, SetHitDamage);
		DECL_MSG(TCompGameStats, TMsgKill, enemieKilled);
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

	void enemieKilled(const TMsgKill& msg) {
		scoreUp = true;
	}

};

DECL_OBJ_MANAGER("gamestats", TCompGameStats)