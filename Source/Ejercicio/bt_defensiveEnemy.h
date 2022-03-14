#ifndef _BT_DEFENSIVE_INC
#define _BT_DEFENSIVE_INC

#include "components/common/comp_base.h"
#include "components/common/comp_transform.h"
#include "components/common/comp_name.h"
#include "components/messages.h"
#include "BehaviorTree/bt.h"
#include "bt_comun.h"


class bt_defensiveEnemy:public bt, public bt_comun {

	

	public:

		void InitTree();

		//bt decorators
		int DecoratorImpact();
		int DecoratorDying();
		int DecoratorAttacker();
		int DecoratorCombat();
		int DecoratorChase();
		int DecoratorEngage();
		int DecoratorDefend();
		int DecoratorTaunt();

		//bt tasks
		int TaskIdle();
		int TaskProcessImpact();
		int TaskDie();
		int TaskAttacker();
		int TaskChase();
		int TaskDefend();
		int TaskAttack();
		int TaskTaunt();
		int TaskMantainDistance();
		int TaskDefendStand();
		};

#endif