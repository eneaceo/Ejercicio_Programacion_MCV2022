#include "mcv_platform.h"
#include "fsm/fsm_context.h"
#include "fsm/fsm.h"

namespace fsm
{
    void CContext::setEnabled(bool enabled)
    {
        _enabled = enabled;
    }

    void CContext::update(float dt)
    {
        if (!_enabled || !_active || !_currentState) return;

        _timeInState += dt;
        _currentState->onUpdate(*this, dt);

        // check transitions as many times as need as long as we keep
        // changing the current state
        bool stateChanged = true;
        while (stateChanged)
        {
            stateChanged = checkTransitions();
        }
    }

    bool CContext::checkTransitions()
    {
        if(!_currentState) return false;

        for (const ITransition* transition : _currentState->transitions)
        {
            const bool finished = transition->onCheck(*this);
            if (finished)
            {
                changeState(transition->target);
                return true;
            }
        }

        return false;
    }

    void CContext::start()
    {
        if(!_enabled || !_fsm) return;

        _variables = _fsm->getVariables();
        const IState* state = _fsm->getInitialState();
        changeState(state);
        _active = true;
    }

    void CContext::stop()
    {
        changeState(nullptr);
        _active = false;
    }

    void CContext::reset()
    {
        stop();

        if (_fsm)
        {
            _variables = _fsm->getVariables();
        }
    }

    void CContext::changeState(const IState* newState)
    {
        if (_currentState)
        {
            _currentState->onExit(*this);
            _currentState = nullptr;
        }

        _timeInState = 0.f;
        _stateFinished = false;

        if (newState)
        {
            _currentState = newState;
            _currentState->onEnter(*this);
        }
    }

    TVariableValue CContext::getVariableValue(const std::string& name) const
    {
        const auto it = _variables.find(name);
        return it != _variables.cend() ? it->second.getValue() : TVariableValue();
    }

    void CContext::setVariableValue(const std::string& name, const TVariableValue& value)
    {
        const auto it = _variables.find(name);
        if(it != _variables.cend())
        {
            it->second.setValue(value);
        }
    }

    CVariable* CContext::getVariable(const std::string& name)
    {
        const auto it = _variables.find(name);
        return it != _variables.cend() ? &it->second : nullptr;
    }

    void CContext::renderInMenu()
    {
        ImGui::Checkbox("Enabled", &_enabled);

        if (_currentState)
        {
            _currentState->renderInMenu("Current State: ");
        }
        else
        {
            ImGui::Text("Current state: ...");
        }
        ImGui::Text("Time In State: %.2f", _timeInState);

        if (ImGui::TreeNode("Variables"))
        {
            for (auto& var : _variables)
            {
                var.second.renderInMenu();
            }

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("FSM Resource"))
        {
            if (_fsm)
            {
                _fsm->renderInMenu();
            }
            ImGui::TreePop();
        }
    }
}