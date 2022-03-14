#pragma once

#include "modules/module_manager.h"
#include "input/input.fwd.h"

class CRenderModule;
class CModuleEntities;
class CModulePhysics;
class CModuleCameraMixer;
class CGPUCullingModule;
namespace input { class CModule; }
namespace ui { class CModule; }

class CEngine
{
public:
  static CEngine& get();

  void init();
  void destroy();
  void doFrame();

  CModuleManager& getModuleManager() { return _moduleManager; }
  CRenderModule& getRender();
  input::CModule* getInput(int id = input::PLAYER_1);
  ui::CModule& getUI();
  CModuleEntities& getEntities() { return *_entities; }
  CModulePhysics& getPhysics() { return *_physics; }
  CModuleCameraMixer& getCameramixer() { return *_cameraMixer; }
  CGPUCullingModule& getGPUCulling() { return *_gpu_culling; }

private:
  void update(float dt);
  void render();
  void registerResourceTypes();
  void initInput(input::CModule* input);

  CTimer         timer;
  CModuleManager _moduleManager;

  CRenderModule* _render = nullptr;
  CModuleEntities* _entities = nullptr;
  CModulePhysics* _physics = nullptr;
  CModuleCameraMixer* _cameraMixer = nullptr;
  CGPUCullingModule* _gpu_culling = nullptr;
  std::vector<input::CModule*> _input;
  ui::CModule* _ui = nullptr;
};
