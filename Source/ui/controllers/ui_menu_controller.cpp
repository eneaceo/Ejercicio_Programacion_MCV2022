#include "mcv_platform.h"
#include "ui/controllers/ui_menu_controller.h"
#include "ui/ui_module.h"
#include "ui/widgets/ui_button.h"
#include "input/input_module.h"
#include "engine.h"

namespace ui
{
    void CMenuController::reset()
    {
        for (TOption& option : _options)
        {
            option.button->changeToState("default");
        }
    }

    void CMenuController::update(float elapsed)
    {
        input::CModule* input = CEngine::get().getInput();
        assert(input);

        if (input->getButton("menu_down").getsPressed())
        {
            nextOption();
        }
        else if (input->getButton("menu_up").getsPressed())
        {
            prevOption();
        }
        else if (input->getButton("menu_confirm").getsPressed())
        {
            highlightOption();
        }
        else if (input->getButton("menu_confirm").getsReleased())
        {
            confirmOption();
        }
    }

    void CMenuController::bind(const std::string& buttonName, Callback callback)
    {
        CButton* button = dynamic_cast<CButton*>(CEngine::get().getUI().getWidget(buttonName));
        assert(button);
        if(!button) return;

        TOption option;
        option.button = button;
        option.callback = callback;
        _options.push_back(option);
    }

    void CMenuController::nextOption()
    {
        selectOption(std::clamp<int>(_currentOption + 1, 0, static_cast<int>(_options.size()) - 1));
    }

    void CMenuController::prevOption()
    {
        selectOption(std::clamp<int>(_currentOption - 1, 0, static_cast<int>(_options.size()) - 1));
    }

    void CMenuController::selectOption(int idx)
    {
        if(idx < 0 || idx >= _options.size())
            return;

        if (_currentOption != kUndefinedOption)
        {
            _options.at(_currentOption).button->changeToState("default");
        }

        _options.at(idx).button->changeToState("selected");
        _currentOption = idx;
    }

    void CMenuController::highlightOption()
    {
        if (_currentOption < 0 || _currentOption >= _options.size())
            return;

        _options.at(_currentOption).button->changeToState("pressed");
    }

    void CMenuController::confirmOption()
    {
        if (_currentOption < 0 || _currentOption >= _options.size())
            return;

        TOption& option = _options.at(_currentOption);
        option.button->changeToState("selected");
        option.callback();
    }
}
