#pragma once

#include "handle/handle.h"
#include "components/common/comp_base.h"

// Before the CEntity because CEntity::sendMsg template needs to know about the all_registered_msgs
#include "callback.h"

class CEntity : public TCompBase {

  CHandle comps[CHandle::max_types];

public:
  ~CEntity();

  CHandle get(uint32_t comp_type) const {
    assert(comp_type < CHandle::max_types);
    return comps[comp_type];
  }

  template< typename TComp >
  CHandle get() const {
    auto om = getObjectManager<TComp>();
    assert(om);
    return comps[om->getType()];
  }

  void debugInMenu();
  void renderDebug();

  void set(uint32_t comp_type, CHandle new_comp);
  void set(CHandle new_comp);
  void load(const json& j, TEntityParseContext& ctx);
  void onEntityCreated();

  const char* getName() const;

  template< class TMsg >
  void sendMsg(const TMsg& msg) {

    // Get access to all handlers of that msg type, using the msg as identifier
    auto range = all_registered_msgs.equal_range(TMsg::getMsgID());
    while (range.first != range.second) {
      const auto& slot = range.first->second;

      // If I own a valid component of that type (int)...
      CHandle h_comp = comps[slot.comp_type];
      if (h_comp.isValid())
        slot.callback->sendMsg(h_comp, &msg);

      ++range.first;
    }
  }

};

// Forward declaring 
template<>
CObjectManager< CEntity >* getObjectManager<CEntity>();

extern CHandle getEntityByName(const std::string& name);

// After the entity is declared, because it uses the CEntity class
#include "entity/msgs.h"