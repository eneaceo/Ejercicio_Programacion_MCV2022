#pragma once

#include "comp_base.h"
#include "comp_camera.h"
#include "render/textures/render_to_texture.h"

class TCompLightSpot : public TCompCamera {
  DECL_SIBLING_ACCESS();
public:
  
  VEC4 color = Colors::White;
  const CTexture* pattern = nullptr;
  CRenderToTexture* shadows_rt = nullptr;
  bool casts_shadows = false;
  bool shadows_enabled = false;
  bool enabled = true;
  int  shadows_resolution = 256;
  float shadows_step = 1.0f;
  float shadows_bias = .001f;

  float intensity = 1.0f;
  void load(const json& j, TEntityParseContext& ctx);
  void update(float dt);
  void debugInMenu();
  void renderDebug();
  bool activate();
  void generateShadowMap();
  MAT44 getWorld() const;
};