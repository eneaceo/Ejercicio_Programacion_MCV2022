#pragma once

#include "components/common/comp_base.h"

class CRenderToTexture;
class CPipelineState;
class CMesh;
class CTexture;

// ------------------------------------
class TCompEdgeDetector : public TCompBase
{

  CRenderToTexture* rt = nullptr;
  const CPipelineState* pipeline = nullptr;
  const CMesh* mesh = nullptr;
  bool  enabled = true;

public:
  void  load(const json& j, TEntityParseContext& ctx);
  void  debugInMenu();

  CTexture* apply(CTexture* in_texture);
};
