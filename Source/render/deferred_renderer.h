#pragma once

class CRenderToTexture;
class ASSAO_Effect;

class CDeferredRenderer {

  int               xres = 0, yres = 0;
  CRenderToTexture* rt_albedos = nullptr;
  CRenderToTexture* rt_normals = nullptr;
  CRenderToTexture* rt_depth = nullptr;
  ASSAO_Effect*     assao_fx = nullptr;
  CHandle           h_camera;

  void renderGBuffer();
  void renderDecals();
  void renderAO();
  void renderAmbientPass();
  void renderPointLights();
  void renderSpotLights(bool with_shadows);
  void renderSkyBox();
  void renderAutoEmissives();

public:

  CDeferredRenderer();
  bool create(int xres, int yres);
  void destroy();
  void render(CRenderToTexture* out_rt, CHandle new_h_camera);
};
