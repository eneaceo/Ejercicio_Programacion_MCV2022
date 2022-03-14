#include "mcv_platform.h"
#include "components/common/comp_base.h"
#include "components/common/comp_transform.h"
#include "entity/entity.h"
#include "engine.h"
#include "input/input_module.h"

struct TComp3rdPersonController : public TCompBase {

  DECL_SIBLING_ACCESS();

  float       desired_distance = 3.0f;
  float       desired_pitch = deg2rad(30);
  float       min_pitch = deg2rad(20);
  float       max_pitch = deg2rad(80);
  VEC3        target_offset;
  CHandle     h_target;
  std::string target_name;
  float       ratio = 0.95f;

  void load(const json& j, TEntityParseContext& ctx) {
	  desired_distance = deg2rad(j.value("desired_distance", rad2deg(desired_distance)));
	  desired_pitch = deg2rad(j.value("desired_pitch", rad2deg(desired_pitch)));
	  min_pitch = deg2rad(j.value("min_pitch", rad2deg(min_pitch)));
	  max_pitch = deg2rad(j.value("max_pitch", rad2deg(max_pitch)));
	  target_name = j.value("target", "player");
	  target_offset = loadVEC3(j, "target_offset");
	  ratio = j.value("ratio", ratio);
  }

  bool dragAngle(const char* title, float* rad) {
	  float degrees = rad2deg(*rad);
	  bool changed = ImGui::DragFloat(title, &degrees, 0.1f, 0.0f, 90.f);
	  *rad = deg2rad(degrees);
	  return changed;
  }

  void debugInMenu() {
	  dragAngle("Min Pitch", &min_pitch);
	  dragAngle("Max Pitch", &max_pitch);
	  dragAngle("Desired Pitch", &desired_pitch);
	  ImGui::DragFloat("Distance", &desired_distance, 0.1f, 0.1f, 10.f);
	  ImGui::DragFloat("Ratio", &ratio, 0.01f, 0.0f, 1.f);
  }

  void update(float delta_unused) {
    auto input = CEngine::get().getInput(input::PLAYER_1);

	// Confirm we have the handle to requested entity
	if (!h_target.isValid()) 
		h_target = getEntityByName(target_name);
	if (!h_target.isValid())
		return;

	CEntity* e_target = h_target;
	TCompTransform* c_target_transform = e_target->get<TCompTransform>();
	
	VEC3 target_pos = c_target_transform->getPosition() + target_offset;
	VEC3 target_forward = c_target_transform->getForward();
	float yaw = vectorToYaw(target_forward);
	VEC3 my_forward = yawPitchToVector(yaw, desired_pitch);
	VEC3 my_desired_pos = target_pos - my_forward * desired_distance;

    TCompTransform* c_my_transform = get<TCompTransform>();
    if (!c_my_transform)
      return;

	VEC3 my_pos = c_my_transform->getPosition();
	VEC3 my_new_pos = my_pos * (1.0f - ratio) + my_desired_pos * ratio;

	c_my_transform->lookAt(my_new_pos, target_pos, VEC3::UnitY);
  }

};

DECL_OBJ_MANAGER("3rdperson_controller", TComp3rdPersonController)