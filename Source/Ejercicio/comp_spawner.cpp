#include "mcv_platform.h"
#include "components/common/comp_base.h"
#include "components/common/comp_transform.h"
#include "components/common/comp_name.h"
#include "entity/entity.h"
#include "render/draw_primitives.h"
#include "entity/entity_parser.h"
#include "components/messages.h"


class TCompSpawner: public TCompBase {

	// Macro to allow access from this component to other sibling components using the get<T>()
	DECL_SIBLING_ACCESS()
	
	int numEnemies;
	bool spawn = false;

	VEC3 newPos;
	bool changePos = false;

	CHandle myh_transform;

public:

	void update(float dt) {

		if (changePos) {
			TCompTransform* my_transform = myh_transform;
			my_transform->setPosition(newPos);
			changePos = false;
		}

		if (spawn) {
			srand(time(NULL));
			for (int i = 0; i < numEnemies; i++) {
				TEntityParseContext ctx;
				TCompTransform* my_transform = myh_transform;
				MAT44 posAsMat = my_transform->asMatrix();
				ctx.root_transform.fromMatrix(posAsMat);
				switch (rand() % 3) {
				case 0:
					parseScene("data/scenes/prefab_enemy_agresive.json", ctx);
					break;
				case 1:
					parseScene("data/scenes/prefab_enemy_defensive.json", ctx);
					break;
				case 2:
					parseScene("data/scenes/prefab_enemy_distance.json", ctx);
					break;
				default:
					break;
				}
			}
			numEnemies = 0;
			spawn = false;
		}
	
	}
	
	void onEntityCreated() {
		myh_transform = get<TCompTransform>();
	}

	static void registerMsgs() {
		DECL_MSG(TCompSpawner, TMsgSpawnRandomEnemies, spawnRandonEnemies);
		DECL_MSG(TCompSpawner, TMsgSpawnPosition, setSpawnPosition);
	}

	void spawnRandonEnemies(const TMsgSpawnRandomEnemies& msg) {
		numEnemies = msg.numEnemies;
		spawn = true;
	}

	void setSpawnPosition(const TMsgSpawnPosition& msg) {
		newPos.x = msg.px;
		newPos.y = msg.py;
		newPos.z = msg.pz;
		changePos = true;
	}

};

DECL_OBJ_MANAGER("spawner", TCompSpawner)