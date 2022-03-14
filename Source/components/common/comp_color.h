#pragma once

#include "comp_base.h"
#include "render/render.h"

// ----------------------------------------
struct TCompColor : public TCompBase {
  VEC4 color = Colors::Red;

  void load(const json& j, TEntityParseContext& ctx) {
	  const std::string& str = j.get<std::string>();
	  color = loadVEC4(str);
  }
  void debugInMenu() {
	  ImGui::ColorEdit4("Color", &color.x);
  }

};

