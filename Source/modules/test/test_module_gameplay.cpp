#include "mcv_platform.h"
#include "modules/test/test_module_gameplay.h"
#include "engine.h"
#include "modules/module_manager.h"
#include "modules/module_camera_mixer.h"
#include "input/input_module.h"
//#include "handle/handle.h"
//#include "entity/entity.h"
#include "entity/entity_parser.h"

bool TestModuleGameplay::start()
{
    {
        TEntityParseContext ctx;
        bool is_ok = parseScene("data/scenes/test_cameras.json", ctx);
        assert(is_ok);
    }

    CModuleCameraMixer& mixer = CEngine::get().getCameramixer();
    mixer.setDefaultCamera(getEntityByName("camera"));
    mixer.setOutputCamera(getEntityByName("mixed_camera"));

    return true;
}

void TestModuleGameplay::update(float dt)
{
    input::CModule* input = CEngine::get().getInput();
    CModuleCameraMixer& mixer = CEngine::get().getCameramixer();

    if (input->getButton("camera_A").getsPressed())
    {
        mixer.blendCamera("camera_A", 3.f, &interpolators::backInOutInterpolator);
    }
    else if(input->getButton("camera_B").getsPressed())
    {
        mixer.blendCamera("camera_B", 3.f, &interpolators::cubicInOutInterpolator);
    }
    else if(input->getButton("camera_C").getsPressed())
    {
        mixer.blendCamera("camera_C", 3.f, &interpolators::elasticOutInterpolator);
    }
}
