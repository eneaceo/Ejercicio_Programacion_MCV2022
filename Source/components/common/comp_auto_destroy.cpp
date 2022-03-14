#include "mcv_platform.h"
#include "comp_auto_destroy.h"

DECL_OBJ_MANAGER("auto_destroy", TCompAutoDestroy);

void TCompAutoDestroy::load(const json& j, TEntityParseContext& ctx) {
  enabled = j.value("enabled", enabled);
  remaining_time = j.value("time", remaining_time);
}

void TCompAutoDestroy::debugInMenu() {
  ImGui::Text("Remaining Time: %f", remaining_time);
  ImGui::Checkbox("Enabled", &enabled);
}


void TCompAutoDestroy::update(float delta) {
  if (!enabled)
    return;
  remaining_time -= delta;
  if (remaining_time <= 0.0f) {
    CHandle h_entity_owner = CHandle(this).getOwner();
    h_entity_owner.destroy();
  }
}

