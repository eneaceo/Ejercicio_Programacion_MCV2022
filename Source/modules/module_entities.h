#pragma once

#include "modules/module.h"
#include "entity/entity.h"

class CModuleEntities : public IModule
{
  std::vector< CHandleManager* > om_to_update;
  std::vector< CHandleManager* > om_to_render_debug;

  void loadListOfManagers(const json& j, std::vector< CHandleManager* >& managers);
  void renderDebugOfComponents();
  void editRenderDebug();

public:
  CModuleEntities(const std::string& name) : IModule(name) { }
  bool start() override;
  void stop() override;
  void render() override;
  void update(float delta) override;
  void renderDebug() override;
  void renderInMenu() override;
};

CHandle getEntityByName(const std::string& name);
