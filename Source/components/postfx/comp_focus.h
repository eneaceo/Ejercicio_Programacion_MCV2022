#pragma once

#include "components/common/comp_base.h"
#include "render/render.h"

class CRenderToTexture;
class CTexture;
class CMesh;
class CPipelineState;

// ------------------------------------
class TCompFocus : public TCompBase
{
  bool                   enabled = true;
  CRenderToTexture*      rt = nullptr;
  const CPipelineState*  pipeline = nullptr;
  const CMesh*           mesh = nullptr;
  CShaderCte<CtesFocus>  cte_focus;

public:
  TCompFocus();
  ~TCompFocus();
  void  load(const json& j, TEntityParseContext& ctx);
  void  debugInMenu();

  CTexture* apply(CTexture* focus_texture, CTexture* blur_texture);
};
