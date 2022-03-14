#pragma once

#include "comp_blur.h"
class CRenderToTexture;
class CPipelineState;

// ------------------------------------
struct TCompBloom : public TCompBlur
{
  CShaderCte< CtesBloom >         cte_bloom;
  CRenderToTexture* rt_highlights = nullptr;

  const CPipelineState* pipeline_filter = nullptr;
  const CPipelineState* pipeline_add = nullptr;
  const CMesh* mesh = nullptr;

  VEC4                            add_weights;
  float                           threshold_min = 0.8f;
  float                           multiplier = 1.0f;

  TCompBloom();
  ~TCompBloom();

  void load(const json& j, TEntityParseContext& ctx);
  void debugInMenu();

  void generateHighlights(CTexture* in_texture);
  void addBloom();
};