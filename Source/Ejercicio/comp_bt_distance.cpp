#include "mcv_platform.h"
#include "components/common/comp_base.h"
#include "components/common/comp_transform.h"
#include "components/common/comp_name.h"
#include "components/common/comp_collider.h"
#include "entity/entity.h"
#include "render/draw_primitives.h"
#include "entity/entity_parser.h"
#include "components/messages.h"
#include "skeleton/comp_skeleton.h"
#include "cal3d/cal3d.h"
#include "skeleton/game_core_skeleton.h"
#include "render/draw_primitives.h"
#include "components/common/comp_transform.h"
#include "components/common/comp_aabb.h"
#include "skeleton/comp_skeleton_ik.h"
#include "skeleton/comp_skel_lookat.h"

#include "bt_distanceEnemy.h"

class TCompBTdistance : public TCompBase {

	// Macro to allow access from this component to other sibling components using the get<T>()
	DECL_SIBLING_ACCESS()

	bt_distanceEnemy bt;
	std::string state;
	CHandle attacker;

	std::string target_name;
	CHandle h_target;
	std::string btmanager_name;
	CHandle h_btmanager;

	CHandle myh_skel;
	float animDuration = 0.0f;
	float animTime = 0.0f;
	bool playingAnim = false;
	bool resetAnim = false;

	//Stats
	float speed;
	float turn_speed;

	bool kill = false;

	bool attacked = false;
	float life = 100;

public:

	void onEntityCreated() {

		bt.InitTree();
		// speed - turn_speed - life
		bt.setStats(speed, turn_speed, life);

		h_target = getEntityByName(target_name);
		if (!h_target.isValid()) return;
		h_btmanager = getEntityByName(btmanager_name);
		if (!h_btmanager.isValid()) return;

		CHandle myh_transform = get<TCompTransform>();
		CHandle myh_entity = CHandle(this).getOwner();

		bt.Init(h_target, myh_transform, myh_entity, h_btmanager);
		
		//Register in bt_manager
		TMsgEngage msg;
		CEntity* msg_target = h_btmanager;
		msg.me = myh_entity;
		msg_target->sendMsg(msg);

		myh_skel = get<TCompSkeleton>();
	}

	void load(const json& j, TEntityParseContext& ctx) {

		//player
		target_name = j.value("target_name", target_name);
		//btmanager
		btmanager_name = j.value("btmanager_name", btmanager_name);
		//Stats
		life = j.value("life", life);
		speed = j.value("speed", speed);
		turn_speed = j.value("turn_speed", turn_speed);
	}

	//debug
	void debugInMenu() {
		ImGui::Text("My state is %s", state.c_str());
		if (ImGui::SmallButton("Kill")) {
			bt.setLife(0);
		}
		ImGui::SameLine();
		if (ImGui::SmallButton("Impact")) {
			bt.setImpact(true);
		}
		ImGui::Text("Life: %f", life);
		ImGui::Text("Anim Duration: %f", animDuration);
		ImGui::Text("Anim Time: %f", animTime);
	}

	void renderDebug() {
		PROFILE_FUNCTION("renderDebug");
	}

	void update(float delta_time) {
		PROFILE_FUNCTION("update");

		if (attacked) {
			life = life - 50;
			bt.setLife(life);
			attacked = false;
		}

		if (kill) {
			CHandle(this).getOwner().destroy();
			return;
		}
		
		bt.updateTime(delta_time);
		bt.updateAttacker(attacker);

		if (bt.getImpact() || bt.getLife() <= 0) {
			bt.setCurrent(bt.findNode("root"));
		}

		if (playingAnim) {
			animTime += delta_time;
		}

		if (animTime > animDuration) {
			bt.setAnimEnded(true);
			resetAnim = true;
		}

		bt.recalc();
		state = bt.getState();

		//dbg("State is: %s\n", state.c_str());

		if (resetAnim) {
			animDuration = 0.0f;
			animTime = 0.0f;
			playingAnim = false;
			bt.setPLayingAnim(false);
			bt.setAnimEnded(false);
			bt.setMovementDir(0);
			resetAnim = false;
		}

		bt.setImpact(false);
	}

	//Msgs go here
	static void registerMsgs() {
		DECL_MSG(TCompBTdistance, TMsgAttacker, whoIsAttacker);
		DECL_MSG(TCompBTdistance, TMsgDestroyMe, destroyMe);
		DECL_MSG(TCompBTdistance, TMsgShoot, shoot);
		DECL_MSG(TCompBTdistance, TMsgAttack, attack);
		DECL_MSG(TCompBTdistance, TMsgAnimation, playAnimation);
		DECL_MSG(TCompBTdistance, TMsgAnimationMovement, playAnimationMovement);
		DECL_MSG(TCompBTdistance, TMsgattacked, attackme);
	}

	void whoIsAttacker(const TMsgAttacker& msg) {
		attacker = msg.attacker;
	}

	void destroyMe(const TMsgDestroyMe& msg) {
		kill = true;
	}

	void shoot(const TMsgShoot& msg) {
		TEntityParseContext ctx;
		ctx.root_transform.fromMatrix(msg.mtx);
		parseScene("data/prefabs/bullet.json", ctx);
	}

	void attack(const TMsgAttack& msg) {
		TEntityParseContext ctx;
		ctx.root_transform.fromMatrix(msg.mtx);
		parseScene("data/prefabs/attackHitbox.json", ctx);
	}

	void playAnimation(const TMsgAnimation& msg) {

		bool auto_lock = false;
		bool root_motion = false;

		TCompSkeleton* my_skel = myh_skel;
		auto mixer = my_skel->model->getMixer();

		if (msg.animNum == 2 || msg.animNum == 3) {
			for (auto a : mixer->getAnimationActionList()) {
				auto core = (CGameCoreSkeleton*)my_skel->model->getCoreModel();
				int id = core->getCoreAnimationId(a->getCoreAnimation()->getName());
				if (a->getState() == CalAnimation::State::STATE_STOPPED)
					mixer->removeAction(id, 0.f);
				else
					a->remove(msg.out_delay);
			}
		}

		my_skel->model->getMixer()->executeAction(msg.animNum, msg.in_delay, msg.out_delay, 1.0f, auto_lock, root_motion);

		for (auto a : mixer->getAnimationActionList()) {
			animDuration = a->getCoreAnimation()->getDuration();
		}

		playingAnim = true;
		bt.setPLayingAnim(true);
	}

	void playAnimationMovement(const TMsgAnimationMovement& msg) {

		bool auto_lock = false;
		bool root_motion = false;

		TCompSkeleton* my_skel = myh_skel;
		auto mixer = my_skel->model->getMixer();

		
		for (auto a : mixer->getAnimationActionList()) {
			auto core = (CGameCoreSkeleton*)my_skel->model->getCoreModel();
			int id = core->getCoreAnimationId(a->getCoreAnimation()->getName());
			if (a->getState() == CalAnimation::State::STATE_STOPPED)
				mixer->removeAction(id, 0.f);
			else
				a->remove(msg.out_delay);
		}
		

		my_skel->model->getMixer()->executeAction(msg.animNum, msg.in_delay, msg.out_delay, 1.0f, auto_lock, root_motion);

		for (auto a : mixer->getAnimationActionList()) {
			animDuration = a->getCoreAnimation()->getDuration();
		}

		playingAnim = true;
		bt.setPLayingAnim(true);

	}

	void attackme(const TMsgattacked& msg) {
		float dist = VEC3::Distance(bt.getMyPosition(), msg.pos);
		if (dist < 1) {
			attacked = true;
		}

	}

};

DECL_OBJ_MANAGER("bt_distance", TCompBTdistance)