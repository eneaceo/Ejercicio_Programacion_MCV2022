#include "mcv_platform.h"
#include "comp_edge_detector.h"
#include "render/textures/render_to_texture.h"

DECL_OBJ_MANAGER("edge_detector", TCompEdgeDetector);

void  TCompEdgeDetector::load(const json& j, TEntityParseContext& ctx) {
  enabled = j.value("enabled", enabled);
  pipeline = Resources.get("edge_detector.pipeline")->as<CPipelineState>();
  mesh = Resources.get("unit_quad_xy.mesh")->as<CMesh>();

  // with the first use, init with the input resolution
  rt = new CRenderToTexture;
  bool is_ok = rt->createRT("RT_EdgeDetector", Render.getWidth(), Render.getHeight(), DXGI_FORMAT_R8G8B8A8_UNORM);
  assert(is_ok);
}

void  TCompEdgeDetector::debugInMenu()
{
  ImGui::Checkbox("Enabled", &enabled);
}

CTexture* TCompEdgeDetector::apply(CTexture* in_texture) {
  if (!enabled)
    return in_texture;
  CGpuScope gpu_scope("EdgeDetector");
  rt->activateRT();
  in_texture->activate(TS_ALBEDO);
  pipeline->activate();
  mesh->activate();
  mesh->render();
  return rt;
}