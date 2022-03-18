#include "mcv_platform.h"
#include "components/common/comp_base.h"
#include "components/common/comp_transform.h"
#include "components/common/comp_collider.h"
#include "engine.h"
#include "modules/module_physics.h"
#include "entity/entity.h"
#include "entity/entity_parser.h"
#include "input/input_module.h"
#include "components/messages.h"

#include "Ejercicio/bt_playerController.h"

struct TCompWASDController : public TCompBase {

  DECL_SIBLING_ACCESS();

	CHandle h_my_transform;
	float speed = 15.0f;
	bool enabled = false;
	int key_toggle_enable = 0;

	bt_playerController bt;

  void onEntityCreated() {
    h_my_transform = get<TCompTransform>();
	CHandle h_collider = get<TCompCollider>();
	bt.Init(h_my_transform,h_collider, speed);

  }

  void load(const json& j, TEntityParseContext& ctx) {
    speed = j.value("speed", speed);
	enabled = j.value("enabled", enabled);
	key_toggle_enable = j.value("key_toggle_enable", key_toggle_enable);
  }

  void debugInMenu() {
    ImGui::DragFloat("Speed", &speed, 0.02f, 0.1f, 15.0f);
	ImGui::Checkbox("Enabled", &enabled);
  }

  void update(float dt) {
	
	bt.updateTime(dt);
	bt.recalc();

  }

  static void registerMsgs() {
	  DECL_MSG(TCompWASDController, TMsgMove, move);
  }

  void move(const TMsgMove& msg) {
	  bt.setCinematicMovement(true, msg.newPos);
  }

};

DECL_OBJ_MANAGER("wasd_controller", TCompWASDController)