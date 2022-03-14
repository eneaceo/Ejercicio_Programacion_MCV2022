#include "mcv_platform.h"
#include "comp_light_spot.h"
#include "comp_transform.h"
#include "render/draw_primitives.h"
#include "render/render_manager.h"
#include "render/render.h"

DECL_OBJ_MANAGER("light_spot", TCompLightSpot)

extern CShaderCte<CtesLight> cte_light;

void TCompLightSpot::load(const json& j, TEntityParseContext& ctx) {
  TCompCamera::load(j, ctx);
  color = loadColor(j, "color");
  intensity = j.value("intensity", intensity);
  enabled = j.value("enabled", enabled);
  shadows_step = j.value("shadows_step", shadows_step);

  if (j.count("pattern")) 
    pattern = Resources.get(j["pattern"])->as<CTexture>();

  // Check if we need to allocate a shadow map
  casts_shadows = j.value("casts_shadows", casts_shadows);
  if (casts_shadows) {
    auto shadowmap_fmt = readFormat(j, "shadows_fmt");    
    shadows_resolution = j.value("shadows_resolution", shadows_resolution);

    assert(shadows_resolution > 0);
    shadows_rt = new CRenderToTexture;

    // Make a unique name to have the Resource Manager happy with the unique names for each resource
    char my_name[64];
    sprintf(my_name, "shadow_map_%08x", CHandle(this).asUnsigned());

    bool is_ok = shadows_rt->createRT(my_name, shadows_resolution, shadows_resolution, DXGI_FORMAT_UNKNOWN, shadowmap_fmt);
    assert(is_ok);
  }

  shadows_enabled = casts_shadows;
}

void TCompLightSpot::update(float dt) {
  TCompTransform* c_transform = get<TCompTransform>();
  assert(c_transform);
  lookAt(c_transform->getPosition(), c_transform->getPosition() + c_transform->getForward());
}

void TCompLightSpot::debugInMenu() {
  TCompCamera::debugInMenu();
  ImGui::ColorEdit4("Color", &color.x);
  ImGui::DragFloat("Intensity", &intensity, 0.02f, 0.f, 50.f);
  ImGui::DragFloat("Shadow Step", &shadows_step, 0.0f, 0.f, 5.f);
  ImGui::DragFloat("Bias", &shadows_bias, 0.0001f, -0.01f, 0.01f);
  if(shadows_rt)
    ImGui::Checkbox("Shadows", &shadows_enabled);
  ImGui::Checkbox("Enabled", &enabled);
  
  
  Resources.edit(&pattern);
}

void TCompLightSpot::renderDebug() {
  MAT44 inv_view_proj = getWorld();
  const CMesh* mesh = Resources.get("view_volume_wired.mesh")->as<CMesh>();
  for (int i = 1; i <= 10; ++i) {
    float scale_factor = (float)i / 10.f;
    MAT44 scale = MAT44::CreateScale(VEC3(1.0f, 1.0f, scale_factor));
    drawPrimitive( mesh, scale * inv_view_proj, Colors::Yellow);
  }
}

MAT44 TCompLightSpot::getWorld() const {
  return getViewProjection().Invert();
}

bool TCompLightSpot::activate() {

  if (intensity == 0.0f || !enabled)
    return false;

  cte_light.light_color = color;
  cte_light.light_intensity = intensity;
  cte_light.light_position = getEye();
  cte_light.light_forward = getForward();
  cte_light.light_max_radius = getFar() * 0.9f;
  cte_light.light_shadows_step = shadows_step;
  cte_light.light_shadows_inv_resolution = 1.0f / (float)shadows_resolution;
  cte_light.light_shadows_step_over_resolution = shadows_step / (float)shadows_resolution;
  cte_light.light_shadows_bias = shadows_bias;
  cte_light.light_view_projection = getViewProjection();
  cte_light.light_view_projection_offset = cte_light.light_view_projection * MAT44::CreateScale(VEC3(0.5f, -0.5f, 1.0f))  * MAT44::CreateTranslation(VEC3(0.5f, 0.5f, 0));

  cte_light.updateFromCPU();
  
  if(pattern)
    pattern->activate(TS_LIGHT_PATTERN);

  if (shadows_rt) {
    assert(shadows_rt->getZTexture());
    shadows_rt->getZTexture()->activate(TS_LIGHT_SHADOW_MAP);
  }

  return true;
}

void TCompLightSpot::generateShadowMap() {
  if (!shadows_rt || !shadows_enabled)
    return;
  CGpuScope gpu_scope("shadowMap");
  CTexture::deactivate(TS_LIGHT_SHADOW_MAP);
  shadows_rt->activateRT();
  shadows_rt->clearZ();
  activateCamera(*this);
  RenderManager.renderAll(eRenderChannel::RC_SHADOW_CASTERS, CHandle(this).getOwner());
}

