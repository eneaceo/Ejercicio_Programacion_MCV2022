#pragma once

#include "texture.h"

class CRenderToTexture : public CTexture {

  DXGI_FORMAT         color_format = DXGI_FORMAT_UNKNOWN;
  DXGI_FORMAT         depth_format = DXGI_FORMAT_UNKNOWN;

  ID3D11RenderTargetView* render_target_view = nullptr;
  ID3D11DepthStencilView* depth_stencil_view = nullptr;

  // To be able to use the ZBuffer as a texture from our material system
  ID3D11Texture2D* depth_resource = nullptr;
  CTexture*        ztexture = nullptr;

  int                      xres = 0;
  int                      yres = 0;

  static CRenderToTexture* current_rt;

public:

  bool createRT(const char* name, int new_xres, int new_yres
    , DXGI_FORMAT new_color_format
    , DXGI_FORMAT new_depth_format = DXGI_FORMAT_UNKNOWN
    , bool        uses_depth_of_backbuffer = false
  );
  void destroy();

  CRenderToTexture* activateRT();
  static void deactivate(int num_render_targets = 1 );
  void activateViewport();

  void clear(VEC4 clear_color);
  void clearZ();

  ID3D11RenderTargetView* getRenderTargetView() {
    return render_target_view;
  }
  CTexture* getZTexture() { return ztexture; }
  int getWidth() const { return xres; }
  int getHeight() const { return yres; }
  bool renderInMenu() const override;

};
