#include "mcv_platform.h"
#include "modules/test/test_module_splash_screen.h"
#include "engine.h"
#include "modules/module_manager.h"
#include "render/render_module.h"

bool TestModuleSplashScreen::start()
{
    _timer = 3.f;
    CEngine::get().getRender().setClearColor({0.5f, 0.f, 0.f, 1.f});
    return true;
}

void TestModuleSplashScreen::update(float dt)
{
    _timer -= dt;
    
    if (_timer <= 0.f)
    {
        CEngine::get().getModuleManager().changeToGamestate("main_menu");
    }
}
