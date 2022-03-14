#pragma once

#include "resources/resources.h"

namespace fsm
{
    class CFSM;
    class IState;
    class ITransition;

    class CParser
    {
    public:
        // states
        class IStateFactory
        {
        public:
            virtual IState* create() = 0;
        };

        template <typename T>
        class CStateFactory : public IStateFactory
        {
        public:
            IState* create() override { return new T(); }
        };

        // transitions
        class ITransitionFactory
        {
        public:
            virtual ITransition* create() = 0;
        };

        template <typename T>
        class CTransitionFactory : public ITransitionFactory
        {
        public:
            ITransition* create() override { return new T(); }
        };

        static void registerTypes();
        static IState* createState(const std::string& type);
        static ITransition* createTransition(const std::string& type);

        static bool parse(CFSM* fsm, const json& jFile);

    private:
        static std::map<std::string_view, IStateFactory*> _stateTypes;
        static std::map<std::string_view, ITransitionFactory*> _transitionTypes;
    };
}