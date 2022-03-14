#pragma once

#include "modules/module.h"
#include "ui/controllers/ui_menu_controller.h"

namespace input { class CModule; }

class TestModuleMainMenu : public IModule
{
  public:
    TestModuleMainMenu(const std::string& name) : IModule(name) {}

    bool start() override;
    void stop() override;
    void update(float dt) override;

  private:
    void onNewGame();
    void onContinue();
    void onExit();

    input::CModule* _player1 = nullptr;
    ui::CMenuController _menuController;
};