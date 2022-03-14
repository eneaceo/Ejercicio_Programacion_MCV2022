#pragma once

#include "components/common/comp_base.h"

class CRenderToTexture;
class CTexture;
class CBlurStep;

// ------------------------------------
class TCompBlur : public TCompBase
{

protected:
  std::vector< CBlurStep* > steps;
  VEC4       weights;
  VEC4       distance_factors;    // 1,2,3,XX
  float      global_distance = 1.0f;
  int        nactive_steps = 0;
  bool       enabled = true;

  void  setPreset1();
  void  setPreset2();

public:
  void  load(const json& j, TEntityParseContext& ctx);
  void  debugInMenu();

  CTexture* apply(CTexture* in_texture);
};
