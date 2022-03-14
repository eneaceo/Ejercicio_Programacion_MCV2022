#include "mcv_platform.h"
#include "components/common/comp_base.h"
#include "components/common/comp_transform.h"
#include "components/common/comp_name.h"
#include "entity/entity.h"
#include "render/draw_primitives.h"
#include "entity/entity_parser.h"
#include "components/messages.h"

#include "../Tools/SLB/include/SLB/SLB.hpp"

class TCompLuaManager : public TCompBase {

	// Macro to allow access from this component to other sibling components using the get<T>()
	DECL_SIBLING_ACCESS()
	

public:

	void onEntityCreated() {
		SLB::Manager m;
		
	}


	static void registerMsgs() {
		
	}

};

DECL_OBJ_MANAGER("luamanager", TCompLuaManager)