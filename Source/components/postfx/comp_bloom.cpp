#include "mcv_platform.h"
#include "comp_bloom.h"
#include "render/textures/render_to_texture.h"
#include "render/blur_step.h"

DECL_OBJ_MANAGER("bloom", TCompBloom);

// Missing optiimizations:
// Change the format of the render target to single channel 8 bits
// Not calibrated
// Limited to 4 steps in the blur steps

// ---------------------
TCompBloom::TCompBloom()
{
  bool is_ok = cte_bloom.create(CB_SLOT_BLOOM, "bloom");
  assert(is_ok);
  // How we mix each downsampled scale
  add_weights = VEC4(3.0f, 1.0, 0.6f, 0.4f);
}

TCompBloom::~TCompBloom()
{
  cte_bloom.destroy();
}

void TCompBloom::debugInMenu()
{
  ImGui::DragFloat("Threshold Min", &threshold_min, 0.01f, 0.f, 1.f);
  ImGui::DragFloat("Multiplier", &multiplier, 0.01f, 0.f, 3.f);
  ImGui::DragFloat4("Add Weights", &add_weights.x, 0.02f, 0.f, 3.f);
  TCompBlur::debugInMenu();
}

void TCompBloom::load(const json& j, TEntityParseContext& ctx)
{
  TCompBlur::load(j, ctx);

  if (j.count("weights"))
    add_weights = loadVEC4(j, "weights");

  threshold_min = j.value("threshold_min", threshold_min);
  multiplier = j.value("multiplier", multiplier);

  rt_highlights = new CRenderToTexture();
  char rt_name[64];
  sprintf(rt_name, "BloomFiltered_%08x", CHandle(this).asUnsigned());
  bool is_ok = rt_highlights->createRT(rt_name, Render.getWidth(), Render.getHeight(), DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN);
  assert(is_ok);

  pipeline_filter = Resources.get("bloom_filter.pipeline")->as<CPipelineState>();
  pipeline_add = Resources.get("bloom_add.pipeline")->as<CPipelineState>();
  mesh = Resources.get("unit_quad_xy.mesh")->as<CMesh>();
}

void TCompBloom::generateHighlights(CTexture* in_texture)
{
  if (!enabled)
    return;

  cte_bloom.bloom_weights = add_weights;
  cte_bloom.bloom_threshold_min = threshold_min;
  cte_bloom.bloom_multiplier = multiplier;
  cte_bloom.bloom_dummy1 = 0.f;
  cte_bloom.bloom_dummy2 = 0.f;
  cte_bloom.updateFromCPU();
  cte_bloom.activate();

  // Filter input image to gather only the highlights
  auto prev_rt = rt_highlights->activateRT();
  assert(prev_rt);
  
  in_texture->activate(TS_ALBEDO);
  pipeline_filter->activate();
  mesh->activate();
  mesh->render();
  
  // Blur the highlights
  CTexture* blurred = TCompBlur::apply(rt_highlights);

  // Reactivate the render target that was active when they call us
  prev_rt->activateRT();
  pipeline_add->activate();

  assert(steps.size() == 4);

  // Activate each blur step in the slots 0, 1, 2, ...
  for (int i = 0; i < nactive_steps; ++i)
    steps[i]->getRenderTarget()->activate(i);
  
  // To execute the pipeline_add over all pixels
  mesh->render();
}



