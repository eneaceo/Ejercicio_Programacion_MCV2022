#ifndef _AIC_PATROL
#define _AIC_PATROL

#include "aicontroller.h"



class aic_patrol : public aicontroller
	{
	CHandle h_target_transform;
	VEC3* wpts;
	int curwpt;
	int nwpts;
	int aggressive = 60;
	int mobile = 20;

	public:
		void ResetWptState();
		void FollowWptState();	
		void ChangeWptState();
		void ChaseState();
		void IdleWarState();
		void AttackState();

		void SelectSideState();
		void OrbitLeftState();
		void OrbitRightState();

		void Init();					
	};

#endif