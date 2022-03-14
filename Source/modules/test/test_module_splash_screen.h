#pragma once

#include "modules/module.h"

class TestModuleSplashScreen : public IModule
{
  public:
      TestModuleSplashScreen(const std::string& name) : IModule(name) {}

    bool start() override;
    void update(float delta) override;

  private:
    float _timer = 0.f;
};