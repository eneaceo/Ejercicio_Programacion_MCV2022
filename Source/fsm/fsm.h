#pragma once

#include "resources/resources.h"
#include "fsm.fwd.h"
#include "fsm_variable.h"

namespace fsm
{
    class CFSM : public IResource
    {
    public:
        friend class CParser;

        bool renderInMenu() const override;

        const IState* getInitialState() const { return _initialState; }
        const IState* getState(const std::string& name) const;
        const MVariables& getVariables() const { return _variables; }

    private:
        VStates _states;
        VTransitions _transitions;
        const IState* _initialState = nullptr;
        MVariables _variables;
    };
}