#include "mcv_platform.h"
#include "comp_light_point.h"
#include "comp_transform.h"
#include "render/render.h"
#include "render/draw_primitives.h"

DECL_OBJ_MANAGER("light_point", TCompLightPoint)

extern CShaderCte<CtesLight> cte_light;

void TCompLightPoint::load(const json& j, TEntityParseContext& ctx) {
  color = loadColor(j, "color");
  intensity = j.value("intensity", intensity);
  enabled = j.value("enabled", enabled);
  radius = j.value("radius", radius);;
}


void TCompLightPoint::debugInMenu() {
  ImGui::ColorEdit4("Color", &color.x);
  ImGui::DragFloat("Intensity", &intensity, 0.02f, 0.f, 50.f);
  ImGui::DragFloat("Radius", &radius, 0.02f, 0.f, 20.f);
  ImGui::Checkbox("Enabled", &enabled);
}

void TCompLightPoint::renderDebug() {
  TCompTransform* c_transform = get<TCompTransform>();
  drawWiredSphere(c_transform->asMatrix(), radius, color);
}

MAT44 TCompLightPoint::getWorld() const {
  const float extra_factor = 1.05f;   // Because the unit_sphere is an approximation
  return MAT44::CreateScale(radius * extra_factor)* MAT44::CreateTranslation(position);
}

bool TCompLightPoint::activate() {

  if (intensity == 0 || !enabled)
    return false;

  TCompTransform* c_transform = get<TCompTransform>();
  position = c_transform->getPosition();
  cte_light.light_color = color;
  cte_light.light_intensity = intensity;
  cte_light.light_position = c_transform->getPosition();
  cte_light.light_forward = VEC3(0, 0, 0);
  cte_light.light_max_radius = radius;
  cte_light.light_view_projection = MAT44::Identity;
  cte_light.light_view_projection_offset = MAT44::Identity;
  cte_light.updateFromCPU();
  return true;
}

