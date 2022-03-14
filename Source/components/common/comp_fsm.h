#pragma once

#include "comp_base.h"
#include "entity/entity.h"
#include "fsm/fsm_context.h"

struct TMsgFSMVariable;

class TCompFSM : public TCompBase
{
    DECL_SIBLING_ACCESS();

public:
    static void registerMsgs();

    void load(const json& j, TEntityParseContext& ctx);
    void onEntityCreated();
    void update(float dt);
    void debugInMenu();

    void onSetVariable(const TMsgFSMVariable& msg);

private:
    fsm::CContext _context;
};
