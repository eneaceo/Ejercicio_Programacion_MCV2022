#include "mcv_platform.h"
#include "engine.h"


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {

  PROFILE_SET_NFRAMES(3);
  PROFILE_FRAME_BEGINS();

  Remotery* rmt;
  rmt_CreateGlobalInstance(&rmt);

  CApplication app;

  if (!app.init(hInstance))
    return -1;

  if (!Render.create(app.getHandle()))
    return -2;

  CEngine::get().init();

  app.run();

  CEngine::get().destroy();
  Render.destroy();

  return 0;
}