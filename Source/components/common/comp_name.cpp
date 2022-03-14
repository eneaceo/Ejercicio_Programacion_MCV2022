#include "mcv_platform.h"
#include "handle/handle.h"
#include "comp_name.h"
#include "comp_transform.h"
#include "render/draw_primitives.h"

DECL_OBJ_MANAGER("name", TCompName)

std::unordered_map< std::string, CHandle > TCompName::all_names;

void TCompName::setName(const char* new_name) {
  strcpy(name, new_name);

  // Store the handle of the CompName, not the Entity, 
  // because during load, I still don't have an owner
  all_names[name] = CHandle(this);
}

void TCompName::debugInMenu() {
  ImGui::Text(name);

}

void TCompName::load(const json& j, TEntityParseContext& ctx) {
  assert(j.is_string());
  setName(j.get<std::string>().c_str());
}

CHandle getEntityByName(const std::string& name) {

  auto it = TCompName::all_names.find(name);
  if (it == TCompName::all_names.end())
    return CHandle();

  // We are storing the handle of the component, but we want to return
  // the handle of the entity owner of that component
  CHandle h_name = it->second;
  return h_name.getOwner();
}

void TCompName::renderDebug() {
  TCompTransform* trans = get<TCompTransform>();
  if (!trans)
    return;
  VEC3 pos = trans->getPosition();
  drawText3D(pos, Colors::White, name);
}