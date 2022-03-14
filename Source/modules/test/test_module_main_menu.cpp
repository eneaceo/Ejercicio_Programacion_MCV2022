#include "mcv_platform.h"
#include "modules/test/test_module_main_menu.h"
#include "engine.h"
#include "modules/module_manager.h"
#include "render/render_module.h"
#include "input/input_module.h"
#include "ui/ui_module.h"

bool TestModuleMainMenu::start()
{
    CEngine::get().getRender().setClearColor({ 0.5f, 0.5f, 0.f, 1.f });
    _player1 = CEngine::get().getInput(input::PLAYER_1);
    assert(_player1);

    CEngine::get().getUI().activateWidget("main_menu");

    _menuController.bind("bt_new_game", std::bind(&TestModuleMainMenu::onNewGame, this));
    _menuController.bind("bt_continue", std::bind(&TestModuleMainMenu::onContinue, this));
    _menuController.bind("bt_exit", std::bind(&TestModuleMainMenu::onExit, this));

    _menuController.reset();
    _menuController.selectOption(0);

    return true;
}
void TestModuleMainMenu::stop()
{
    CEngine::get().getUI().deactivateWidget("main_menu");
}

void TestModuleMainMenu::update(float dt)
{
    const bool pressed = _player1->getButton("jump").getsPressed();
    if (pressed)
    {
        dbg("Ready player one");
    }
    
    _menuController.update(dt);

/*  //RUMBLE TEST
    TVibrationData data;
    data.leftRatio = _player1->getPadButton(input::EPadButton::L2).value;
    data.rightRatio = _player1->getPadButton(input::EPadButton::R2).value;
    _player1->rumble(data);
*/
    //const bool spacePressed = CEngine::get().getInput(PLAYER_1).getKey(VK_SPACE).isPressed();
    //const bool spacePressed = CEngine::get().getInput(PLAYER_1)["jump"];
}


void TestModuleMainMenu::onNewGame()
{
    dbg("NEW GAME\n");
}

void TestModuleMainMenu::onContinue()
{
    dbg("CONTINUE\n");
}

void TestModuleMainMenu::onExit()
{
    dbg("EXIT\n");
}
