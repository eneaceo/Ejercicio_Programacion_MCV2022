#include "mcv_platform.h"
#include "entity.h"
#include "entity_parser.h"
#include "components/common/comp_name.h"

// From msgs.h
std::unordered_multimap< uint32_t, TCallbackSlot > all_registered_msgs;

uint32_t getNextUniqueMsgID() {
  static uint32_t next_msg_id = 0;
  ++next_msg_id;
  return next_msg_id;
}

DECL_OBJ_MANAGER("entity", CEntity);

CEntity::~CEntity() {
  // Comp 0 is not valid
  for (uint32_t i = 1; i < CHandleManager::getNumDefinedTypes(); ++i) {
    CHandle h = comps[i];
    if (comps[i].isValid())
      comps[i].destroy();
  }
}

void CEntity::set(uint32_t comp_type, CHandle new_comp) {
  assert(comp_type < CHandle::max_types);
  assert(!comps[comp_type].isValid());
  comps[comp_type] = new_comp;
  new_comp.setOwner(CHandle(this));
}

void CEntity::set(CHandle new_comp) {
  set(new_comp.getType(), new_comp);
}

const char* CEntity::getName() const {
  TCompName* comp_name = get<TCompName>();
  return comp_name ? comp_name->getName() : "<Unnamed>";
}

void CEntity::onEntityCreated() {
  for (uint32_t i = 0; i < CHandleManager::getNumDefinedTypes(); ++i) {
    CHandle h = comps[i];
    h.onEntityCreated();
  }
}

void CEntity::renderDebug() {
  for (uint32_t i = 0; i < CHandleManager::getNumDefinedTypes(); ++i) {
    CHandle h = comps[i];
    if (h.isValid())
      h.renderDebug();
  }
}

void CEntity::debugInMenu() {
  ImGui::PushID(this);
  if (ImGui::TreeNode(getName())) {

    // Scan all my components...
    for (int i = 0; i < CHandle::max_types; ++i) {
      CHandle h = comps[i];
      if (h.isValid()) {

        // Open a tree using the name of the component
        if (ImGui::TreeNode(h.getTypeName())) {
          // Do the real show details of the component
          h.debugInMenu();
          ImGui::TreePop();
        }
      }
    }

    ImGui::TreePop();
  }

  if (ImGui::IsItemHovered())
    renderDebug();

  ImGui::PopID();
}

void CEntity::load(const json& j, TEntityParseContext& ctx) {
  PROFILE_FUNCTION("load");
  // 'this' is the current entity being loaded
  ctx.current_entity = CHandle(this);

  // Iterate over all key/values of the json object j
  for (const auto& it : j.items()) {

    auto& comp_name = it.key();     // name
    auto& comp_value = it.value();   // "john" o { pos="..." }
    
    if( 0 ) {
      PROFILE_FUNCTION("Msg");
      dbg("Parsing comp %s with json %s\n", comp_name.c_str(), comp_value.dump().c_str());
    }

    // This is the handle manager that deatls with components of name 'comp_name'
    auto om = CHandleManager::getByName(comp_name.c_str());
    if (!om) {
      dbg("Warning. Unknown component name %s\n", comp_name.c_str());
      continue;
    }

    // Get the associated comp_id_type
    uint32_t comp_type = om->getType();

    PROFILE_FUNCTION(om->getName());

    // Access the component of that type currently associated to this entity
    CHandle h_comp = comps[comp_type];

    if (h_comp.isValid()) {

      // Let the component an opportunity to recustomize for the instanced prefab
      // with the new json
      h_comp.load(comp_value, ctx);

    }
    else {

      // Create a new fresh component of that type
      h_comp = om->createHandle();

      // bind the component to the entity (this)
      set(comp_type, h_comp);

      // Let the component configure itself from the json
      h_comp.load(comp_value, ctx);

    }


  }

}