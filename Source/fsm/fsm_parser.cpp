#include "mcv_platform.h"
#include "fsm/fsm_parser.h"

#include "fsm/fsm.h"
#include "fsm/states/state_shake.h"
#include "fsm/states/state_move.h"
#include "fsm/transitions/transition_wait_time.h"
#include "fsm/transitions/transition_wait_state_finished.h"
#include "fsm/transitions/transition_check_variable.h"

namespace fsm
{
    std::map<std::string_view, CParser::IStateFactory*> CParser::_stateTypes;
    std::map<std::string_view, CParser::ITransitionFactory*> CParser::_transitionTypes;

    void CParser::registerTypes()
    {
        _stateTypes[""] = new CStateFactory<CStateDummy>();
        _stateTypes["shake"] = new CStateFactory<CStateShake>();
        _stateTypes["move"] = new CStateFactory<CStateMove>();

        _transitionTypes["wait_time"] = new CTransitionFactory<CTransitionWaitTime>();
        _transitionTypes["wait_state_finished"] = new CTransitionFactory<CTransitionWaitStateFinished>();
        _transitionTypes["check_variable"] = new CTransitionFactory<CTransitionCheckVariable>();
    }

    IState* CParser::createState(const std::string& type)
    {
        auto it = _stateTypes.find(type);
        if (it == _stateTypes.cend())
        {
            fatal("Undefined FSM state type %s", type.c_str());
            return nullptr;
        }

        IState* st = it->second->create();
        st->type = it->first;
        return st;
    }

    ITransition* CParser::createTransition(const std::string& type)
    {
        auto it = _transitionTypes.find(type);
        if (it == _transitionTypes.cend())
        {
            fatal("Undefined FSM transition type %s", type.c_str());
            return nullptr;
        }

        ITransition* tr = it->second->create();
        tr->type = it->first;
        return tr;
    }

    bool CParser::parse(CFSM* fsm, const json& jFile)
    {
        // states
        const json& jStates = jFile["states"];
        for (const auto& jState : jStates)
        {
            const std::string type = jState.value("type", std::string());

            IState* newState = CParser::createState(type);

            newState->name = jState.value("name", std::string());
            newState->onLoad(jState);

            fsm->_states.push_back(newState);
        }

        // transitions
        const json& jTransitions = jFile["transitions"];
        for (const auto& jTransition : jTransitions)
        {
            const std::string& sourceName = jTransition["source"];
            const std::string& targetName = jTransition["target"];

            IState* sourceState = const_cast<IState*>(fsm->getState(sourceName));
            IState* targetState = const_cast<IState*>(fsm->getState(targetName));

            assert(sourceState && targetState);
            if (!sourceState || !targetState) continue;

            const std::string type = jTransition.value("type", std::string());

            ITransition* newTransition = CParser::createTransition(type);

            newTransition->onLoad(jTransition);

            newTransition->source = sourceState;
            newTransition->target = targetState;

            sourceState->transitions.push_back(newTransition);

            fsm->_transitions.push_back(newTransition);
        }

        // variables
        const json& jVariables = jFile["variables"];
        for (const auto& jVariable : jVariables)
        {
            CVariable var;
            var.load(jVariable);
            fsm->_variables[var.getName()] = var;
        }

        // initial state
        fsm->_initialState = fsm->getState(jFile.value("initial_state", std::string()));

        return true;
    }
}