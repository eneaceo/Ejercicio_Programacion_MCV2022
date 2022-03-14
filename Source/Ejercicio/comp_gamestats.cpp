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

	int life;
	int maxLife;
	bool potion = false;
	bool shootHit = false;

public:

	void load(const json& j, TEntityParseContext& ctx) {
		life = j.value("life", life);
		maxLife = j.value("maxLife", maxLife);
	}

	void update(float dt) {
		if (potion && life < 200) {
			life += 10;
			if (life > 200) life = maxLife;
			potion = false;
		}
		if (shootHit) {
			life -= 10;
			shootHit = false;
		}
	}

	void debugInMenu() {
		ImGui::Text("Life: %d", life);
		ImGui::SameLine();
		ImGui::Text("MaxLife: 200");
		if (ImGui::SmallButton("-10")) {
			life -= 10;
		}

	}

	static void registerMsgs() {
		DECL_MSG(TCompGameStats, TMsgPotion, Potion);
		DECL_MSG(TCompGameStats, TMsgShootHit, ShootHit);
	}

	void Potion(const TMsgPotion& msg) {
		potion = true;
	}

	void ShootHit(const TMsgShootHit& msg) {
		shootHit = true;
	}

};

DECL_OBJ_MANAGER("gamestats", TCompGameStats)