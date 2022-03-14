#include "mcv_platform.h"
#include "comp_parent.h"

DECL_OBJ_MANAGER("parent", TCompParent)

TCompParent::~TCompParent() {
  for (auto h : children)
    h.destroy();
}

bool TCompParent::delChild(CHandle h_curr_child) {
  int idx = 0;
  for (auto h : children) {
    if (h == h_curr_child) {
      // Remove from my vector
      children.erase(children.begin() + idx);
      // Tell the handle, I'm no longer his parent
      h_curr_child.setOwner(CHandle());

      // There should be no more copies of h_curr_child in the vector
      assert(std::find(children.begin(), children.end(), h_curr_child) == children.end());

      return true;
    }
    ++idx;
  }
  return false;
}

void TCompParent::addChild(CHandle h_new_child) {

  if (!h_new_child.isValid())
    return;

  // Confirm the handle, if valid, represents an entity
  assert(h_new_child.getType() == getObjectManager<CEntity>()->getType());

  // This is the handle to the entity owning this component
  CHandle h_my_parent = CHandle(this).getOwner();
  assert(h_my_parent.isValid());

  // unbind from current parent if exists
  CHandle h_curr_parent = h_new_child.getOwner();
  if (h_curr_parent.isValid()) {

    // I'm already your parent
    if (h_curr_parent == h_my_parent)
      return;

    CEntity* e_curr_parent = h_curr_parent;
    assert(e_curr_parent);
    TCompParent* c_curr_parent = e_curr_parent->get<TCompParent>();
    if (c_curr_parent)
      c_curr_parent->delChild(h_new_child);
  }

  // Ensure it's not already my child 
  assert(std::find(children.begin(), children.end(), h_new_child) == children.end());

  // Assigning my entity owner as parent of the child.
  h_new_child.setOwner(h_my_parent);
  children.push_back(h_new_child);
}

void TCompParent::debugInMenu() {
  for (auto h : children) 
    h.debugInMenu();
}
