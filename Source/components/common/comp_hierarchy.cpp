#include "mcv_platform.h"
#include "comp_hierarchy.h"
#include "comp_transform.h"
#include "entity/entity.h"
#include "entity/entity_parser.h"

DECL_OBJ_MANAGER("hierarchy", TCompHierarchy)

void TCompHierarchy::debugInMenu() {
  CTransform::renderInMenu();
}

void TCompHierarchy::load(const json& j, TEntityParseContext& ctx) {
  CTransform::fromJson(j);
  
  assert(j.count("parent"));
  parent_name = j.value("parent", "");

  CHandle h_entity_parent = ctx.findEntityByName(parent_name);
  CEntity* e_entity_parent = h_entity_parent;
  assert(e_entity_parent);

  h_parent_transform = e_entity_parent->get<TCompTransform>();
}

void TCompHierarchy::onEntityCreated() {
  assert(!h_my_transform.isValid());
  h_my_transform = get<TCompTransform>();
}

void TCompHierarchy::update(float dt) {
  
  // The target transform
  TCompTransform* c_parent_transform = h_parent_transform;
  if (!c_parent_transform)
    return;

  TCompTransform* c_my_transform = h_my_transform;
  assert(c_my_transform || fatal( "Entity %s comp hierarchy is missing the TCompTransform", getEntity() ? getEntity()->getName() : "Unknown" ) );

  // Apply Delta(me) to the parent to set the transform of my entity
  c_my_transform->set(c_parent_transform->combinedWith(*this));

}

