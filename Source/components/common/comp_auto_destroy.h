#pragma once

#include "comp_base.h"

// ----------------------------------------
struct TCompAutoDestroy : public TCompBase {
  float       remaining_time = 1.0f;
  bool        enabled = true;
  
  void load(const json& j, TEntityParseContext& ctx);
  void update( float delta );
  void debugInMenu();

};
