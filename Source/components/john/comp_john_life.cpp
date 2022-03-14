#include "mcv_platform.h"
#include "entity/entity.h"
#include "components/common/comp_base.h"
#include "components/messages.h"
#include "components/common/comp_collider.h"
#include "components/controllers/comp_bullet_controller.h"

class TCompJohnLife : public TCompBase {

  DECL_SIBLING_ACCESS();

  float life = 100.0f;

public:

  static void registerMsgs() {
    DECL_MSG(TCompJohnLife, TMsgExplosion, onExplosion);
    DECL_MSG(TCompJohnLife, TMsgAlarm, onAlarm);
	DECL_MSG(TCompJohnLife, TMsgEntityTriggerEnter, OnTriggerEnter);
	DECL_MSG(TCompJohnLife, TMsgEntityTriggerExit, OnTriggerExit);
  }

  void OnTriggerEnter(const TMsgEntityTriggerEnter& msg) {
	  life += 10.0f;
  }

  void OnTriggerExit(const TMsgEntityTriggerExit& msg) {
	  life -= 10.0f;
  }

  void load(const json& j, TEntityParseContext& ctx) {
    life = j.value("life", life);
  }

  void onExplosion(const TMsgExplosion& msg) {
    float new_life = std::max(life - msg.damage, 0.0f);
    dbg("Recv explosion %f. New life %f", msg.damage, new_life);
    life = new_life;
    // if( life < 0 ) ... killed
  }

  void onAlarm(const TMsgAlarm& msg) {
    // Translate entity handle to entity pointer
    CEntity* sender = msg.who_fired_the_alarm;
    dbg( "Hey.. detected by %s\n", sender->getName());
    CHandle detected_by = msg.who_fired_the_alarm;
    detected_by.destroy();
  }

  void debugInMenu() {
    ImGui::Text("Life: %f", life);
  }

};

DECL_OBJ_MANAGER("john_life", TCompJohnLife)