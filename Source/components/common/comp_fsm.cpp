#include "mcv_platform.h"
#include "handle/handle.h"
#include "comp_fsm.h"
#include "fsm/fsm.h"
#include "components/messages.h"

DECL_OBJ_MANAGER("fsm", TCompFSM)

void TCompFSM::registerMsgs()
{
    DECL_MSG(TCompFSM, TMsgFSMVariable, onSetVariable);
}

void TCompFSM::load(const json& j, TEntityParseContext& ctx)
{
    const std::string& filename = j["fsm"];
    _context.setFSM(Resources.get(filename)->as<fsm::CFSM>());

    const bool enabled = j.value("enabled", true);
    _context.setEnabled(enabled);
}

void TCompFSM::onEntityCreated()
{
    _context.setOwnerEntity(CHandle(this).getOwner());
    _context.start();
}

void TCompFSM::update(float dt)
{
    _context.update(dt);
}

void TCompFSM::debugInMenu()
{
    _context.renderInMenu();
}

void TCompFSM::onSetVariable(const TMsgFSMVariable& msg)
{
    _context.setVariableValue(msg.name, msg.value);
}
