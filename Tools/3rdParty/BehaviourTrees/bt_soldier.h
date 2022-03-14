#ifndef _BT_SOLDIER_INC
#define _BT_SOLDIER_INC


#include "bt.h"


class bt_soldier:public bt
	{
	public:
		void Init();

		int TaskIdle();
		int TaskScream();
		int TaskShout();
		int TaskPart1();
		int TaskPart2();
		int DecoratorEscape();
		};

#endif