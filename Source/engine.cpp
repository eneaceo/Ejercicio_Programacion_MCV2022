#include "mcv_platform.h"
#include "engine.h"
#include "render/textures/material.h"
#include "geometry/curve.h"
#include "fsm/fsm.h"
#include "fsm/fsm_parser.h"
#include "resources/animation_data.h"
#include "particles/particle_emitter.h"

#include "render/render_module.h"
#include "render/gpu_culling_module.h"
#include "modules/module_entities.h"
#include "modules/module_physics.h"
#include "modules/module_camera_mixer.h"
#include "modules/module_boot.h"
#include "input/input_module.h"
#include "input/devices/device_keyboard_windows.h"
#include "input/devices/device_mouse_windows.h"
#include "input/devices/device_pad_xbox_windows.h"
#include "ui/ui_module.h"
#include "render/textures/video/video_texture.h"

#include "modules/test/test_module_splash_screen.h"
#include "modules/test/test_module_main_menu.h"
#include "modules/test/test_module_gameplay.h"
#include "modules/test/test_module_john.h"

#include "utils/directory_watcher.h"
#include "utils/resource_json.h"
#include "skeleton/game_core_skeleton.h"

#pragma comment(lib, "winmm.lib")

CDirectoyWatcher dir_watcher_data;
class CComputeShader;

CEngine& CEngine::get()
{
  static CEngine instance;
  return instance;
}

void CEngine::registerResourceTypes()
{
  Resources.registerResourceType(getClassResourceType<CMesh>());
  Resources.registerResourceType(getClassResourceType<CTexture>());
  Resources.registerResourceType(getClassResourceType<CPipelineState>());
  Resources.registerResourceType(getClassResourceType<CMaterial>());
  Resources.registerResourceType(getClassResourceType<CCurve>());
  Resources.registerResourceType(getClassResourceType<CJson>());
  Resources.registerResourceType(getClassResourceType<CGameCoreSkeleton>());
  Resources.registerResourceType(getClassResourceType<fsm::CFSM>());
  Resources.registerResourceType(getClassResourceType<particles::CEmitter>());
  Resources.registerResourceType(getClassResourceType<CComputeShader>());
  Resources.registerResourceType(getClassResourceType<TCoreAnimationData>());
}

void CEngine::init()
{
  PROFILE_FUNCTION("Engine::init");
  static TestModuleSplashScreen test_module_splash_screen("splash_screen");
  static TestModuleMainMenu test_module_main_menu("main_menu");
  static TestModuleGameplay test_module_gameplay("gameplay");
  static TestModuleJohn test_module_john("john");
  static CModuleBoot boot("boot");

  registerResourceTypes();

  bool is_ok = VideoTexture::createAPI();

  input::CModule::loadDefinitions("data/input/definitions.json");
  fsm::CParser::registerTypes();

  // services modules
  _render = new CRenderModule("render");
  _entities = new CModuleEntities("entities");
  _physics = new CModulePhysics("physics");
  _cameraMixer = new CModuleCameraMixer("camera_mixer");
  _input.push_back(new input::CModule("input_1", input::PLAYER_1));
  _ui = new ui::CModule("ui");
  _gpu_culling = new CGPUCullingModule("gpu_culling");

  _moduleManager.registerServiceModule(_render);
  _moduleManager.registerServiceModule(_entities);
  _moduleManager.registerServiceModule(_physics);
  _moduleManager.registerServiceModule(_cameraMixer);
  for (auto input : _input) {
    _moduleManager.registerServiceModule(input);
    initInput(input);
  }
  _moduleManager.registerServiceModule(_ui);
  _moduleManager.registerServiceModule(_gpu_culling);

  // game modules
  _moduleManager.registerGameModule(&test_module_splash_screen);
  _moduleManager.registerGameModule(&test_module_main_menu);
  _moduleManager.registerGameModule(&test_module_gameplay);
  _moduleManager.registerGameModule(&test_module_john);
  _moduleManager.registerGameModule(&boot);

  // boot
  _moduleManager.boot();

  dir_watcher_data.start("data", CApplication::get().getHandle());

  timer.reset();
}

void CEngine::destroy()
{
  VideoTexture::destroyAPI();
  _moduleManager.clear();
  Resources.destroy();
  delete _render;
}

void CEngine::doFrame()
{
  PROFILE_FRAME_BEGINS();
  PROFILE_FUNCTION("Engine::doFrame");
  Time.set(timer.elapsed());
  update(Time.delta);
  // Let the renderModule generate the frame
  _render->generateFrame();
}

void CEngine::update(float dt)
{
  _moduleManager.update(dt);
}

void CEngine::render()
{
  _moduleManager.render();
}

CRenderModule& CEngine::getRender()
{
  assert(_render != nullptr);
  return *_render;
}

input::CModule* CEngine::getInput(int id)
{
  input::CModule* requiredInput = nullptr;

  for (auto input : _input)
  {
    if (input->getId() == id)
    {
      requiredInput = input;
    }
  }

  assert(requiredInput != nullptr);
  return requiredInput;
}

void CEngine::initInput(input::CModule* input)
{
  assert(input);

    input->registerDevice(new input::CDeviceKeyboardWindows());
    input->registerDevice(new input::CDeviceMouseWindows(CApplication::get().getHandle()));
    input->registerDevice(new input::CDevicePadXboxWindows(input->getId()));

    input->loadMapping("data/input/mapping.json");
}

ui::CModule& CEngine::getUI()
{
    return *_ui;
}
