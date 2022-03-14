#pragma once

#include "mcv_platform.h"
#include "fsm.fwd.h"

namespace fsm
{
    class IState
    {
    public:
        virtual void onEnter(CContext& ctx) const { printf("OnEnter %s", name.c_str()); }
        virtual void onUpdate(CContext& ctx, float dt) const {}
        virtual void onExit(CContext& ctx) const { printf("OnExit %s", name.c_str()); }
        virtual void onLoad(const json& params) {}
        virtual void renderInMenu(const std::string& prefix = "") const;

        std::string_view type;
        std::string name;
        VTransitions transitions;
    };

    using CStateDummy = IState;
}
