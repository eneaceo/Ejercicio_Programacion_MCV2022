#ifndef _BT_AGRESIVE_INC
#define _BT_AGRESIVE_INC

#include "components/common/comp_base.h"
#include "components/common/comp_transform.h"
#include "components/common/comp_name.h"
#include "components/messages.h"
#include "BehaviorTree/bt.h"
#include "bt_comun.h"


class bt_agresiveEnemy:public bt, public bt_comun {

	

	public:

		void InitTree();

		//bt decorators
		int DecoratorImpact();
		int DecoratorDying();
		int DecoratorAttacker();
		int DecoratorCombat();
		int DecoratorChase();
		int DecoratorEngage();

		//bt tasks
		int TaskIdle();
		int TaskProcessImpact();
		int TaskDie();
		int TaskAttacker();
		int TaskChase();
		int TaskAttack1();
		int TaskAttack2();
		int TaskCombo1();
		int TaskCombo2();
		int TaskMantainDistance();
		int TaskTaunt();
		};

#endif