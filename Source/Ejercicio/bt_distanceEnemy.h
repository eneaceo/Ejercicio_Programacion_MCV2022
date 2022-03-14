#ifndef _BT_DISTANCE_INC
#define _BT_DISTANCE_INC

#include "components/common/comp_base.h"
#include "components/common/comp_transform.h"
#include "components/common/comp_name.h"
#include "components/messages.h"
#include "BehaviorTree/bt.h"
#include "bt_comun.h"


class bt_distanceEnemy:public bt, public bt_comun {

	public:

		void InitTree();

		//bt decorators
		int DecoratorImpact();
		int DecoratorDying();
		int DecoratorAttacker();
		int DecoratorCombat();
		int DecoratorChase();
		int DecoratorShoot();
		int DecoratorMoveBack();
		int DecoratorMoveBackFar();
		int DecoratorShootFar();
		int DecoratorMantainDistance();

		//bt tasks
		int TaskIdle();
		int TaskProcessImpact();
		int TaskDie();
		int TaskAttacker();
		int TaskChase();
		int TaskMoveAround();
		int TaskShoot();
		int TaskMoveBack();
		int TaskAttack();
		int TaskMoveBackFar();
		int TaskShootFar();
		int TaskMantainDistance();
		int TaskTaunt();
		};

#endif