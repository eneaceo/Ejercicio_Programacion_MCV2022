#pragma once

#include "modules/module.h"

class TestModuleJohn : public IModule
{
public:
  TestModuleJohn(const std::string& name) : IModule(name) {}

  bool start() override;
  void render() override;
  void renderDebug() override;
  void renderInMenu() override;
  void update(float dt) override;
};