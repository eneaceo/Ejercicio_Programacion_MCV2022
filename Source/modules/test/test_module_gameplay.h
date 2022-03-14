#pragma once

#include "modules/module.h"

class TestModuleGameplay : public IModule
{
  public:
      TestModuleGameplay(const std::string& name) : IModule(name) {}

    bool start() override;
    void update(float dt) override;
};