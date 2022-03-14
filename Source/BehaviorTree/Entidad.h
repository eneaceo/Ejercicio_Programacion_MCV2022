#ifndef _ENTIDAD_INC
#define _ENTIDAD_INC

#include "geometry/transform.h"


struct Entidad {
  CTransform transform;
  const CMesh* mesh = nullptr;

  // For parent/child relation ship
  int        parent_id = -1;
  CTransform delta;

  void renderInMenu() {
    ImGui::PushID(this);
    if (Resources.edit((const IResource**) &mesh)) {
      // The user has changed the mesh we use for this enemey
    }
    transform.renderInMenu();
    ImGui::DragInt("Parent", &parent_id, 0.02f, -1, 100);
    if (parent_id != -1) {
      if (ImGui::TreeNode("Delta...")) {
        delta.renderInMenu();
        ImGui::TreePop();
      }
    }
    ImGui::PopID();
    ImGui::Separator();
  }
};


#endif