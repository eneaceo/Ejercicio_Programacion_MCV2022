#include "mcv_platform.h"
#include "bt_agresiveEnemy.h"

    //{ "name": "idle" },
    //{ "name": "idle_2" },
    //{ "name": "impact" },
    //{ "name": "dying" },
    //{ "name": "taunt" },
    //{ "name": "run_forward" },
    //{ "name": "run_backwards" },
    //{ "name": "walk_left" },
    //{ "name": "walk_right" },
    //{ "name": "punch_1" },
    //{ "name": "punch_2" },
    //{ "name": "punch_3" },
    //{ "name": "punch_4" },
    //{ "name": "kick_1" },
    //{ "name": "kick_2" },
    //{ "name": "special" }

void bt_agresiveEnemy::InitTree() {

	CreateRootNode("root", SELECTOR);
    //Impact
    AddDecoratorNode("root", "dec_impact", (bttask)&bt_agresiveEnemy::DecoratorImpact);
    AddTaskNode("dec_impact", "processimpact", (bttask)&bt_agresiveEnemy::TaskProcessImpact);
    //Dying
    AddDecoratorNode("root", "dec_dying", (bttask)&bt_agresiveEnemy::DecoratorDying);
    AddTaskNode("dec_dying", "die", (bttask)&bt_agresiveEnemy::TaskDie);
    //Attacker Slot
    AddDecoratorNode("root", "dec_attacker", (bttask)&bt_agresiveEnemy::DecoratorAttacker);
    AddTaskNode("dec_attacker", "attacker", (bttask)&bt_agresiveEnemy::TaskAttacker);
    //combat
    AddDecoratorNode("root", "dec_combat", (bttask)&bt_agresiveEnemy::DecoratorCombat);
	AddCompositeNode("dec_combat", "combat", SELECTOR);
	AddDecoratorNode("combat", "dec_chase", (bttask)&bt_agresiveEnemy::DecoratorChase);
	AddDecoratorNode("combat", "dec_engage", (bttask)&bt_agresiveEnemy::DecoratorEngage);
    AddTaskNode("dec_chase", "chase", (bttask)&bt_agresiveEnemy::TaskChase);
    AddCompositeNode("dec_engage", "attack", RANDOM);
    AddTaskNode("attack", "attack1", (bttask)&bt_agresiveEnemy::TaskAttack1);
    AddTaskNode("attack", "attack2", (bttask)&bt_agresiveEnemy::TaskAttack2);
    AddCompositeNode("attack", "combo", SEQUENCE);
    AddTaskNode("combo", "combo1", (bttask)&bt_agresiveEnemy::TaskCombo1);
    AddTaskNode("combo", "combo2", (bttask)&bt_agresiveEnemy::TaskCombo2);
    //idle
    AddCompositeNode("root", "idle", PARALLEL);
    AddTaskNode("idle", "mantaindistance", (bttask)&bt_agresiveEnemy::TaskMantainDistance);
    AddTaskNode("idle", "taunt", (bttask)&bt_agresiveEnemy::TaskTaunt);

}


//DECORATORS

int bt_agresiveEnemy::DecoratorImpact() {

    if (getImpact()) return SUCCESS;
    else return FAIL;
}

int bt_agresiveEnemy::DecoratorDying() {

    if (getLife() <= 0) return SUCCESS;
    else return FAIL;
}

int bt_agresiveEnemy::DecoratorAttacker() {

    if (attackerSlotFree()) return SUCCESS;
    else return FAIL;
}

int bt_agresiveEnemy::DecoratorCombat() {

    if (iAmAttacker()) return SUCCESS;
    else return FAIL;
}

int bt_agresiveEnemy::DecoratorChase() {

    VEC3 enemy_position = getEnemyPosition();
    VEC3 my_position = getMyPosition();
    float dist = VEC3::Distance(my_position, enemy_position);

    if (dist > 1) return SUCCESS;
    else return FAIL;
}

int bt_agresiveEnemy::DecoratorEngage() {

    VEC3 enemy_position = getEnemyPosition();
    VEC3 my_position = getMyPosition();
    float dist = VEC3::Distance(my_position, enemy_position);

    if (dist < 1) return SUCCESS;
    else return FAIL;
}


//TASKS
int bt_agresiveEnemy::TaskIdle() {
	setState("Idle");
	return SUCCESS;
}

int bt_agresiveEnemy::TaskProcessImpact() {
    setState("Process Impact");
    if (getDamage() > 20 || rand() % 5 == 0) changeAttacker();
    msgAnimation(2, 0.0f, 0.2f);
    return SUCCESS;
}

int bt_agresiveEnemy::TaskDie() {
    setState("Dying");

    if (!getDying()) {
        setDying();
        msgAnimation(3, 0.0f, 0.0f);
    }

    if (!getAnimEnded()) {
        return IN_PROGRESS;
    }
    else {
        msgDying();
        msgDestroyMe();
        return SUCCESS;
    }
}

int bt_agresiveEnemy::TaskAttacker() {
    setState("Attacker");
    askForAttacker();
    return SUCCESS;
}

int bt_agresiveEnemy::TaskChase() {
    setState("Chase");
    VEC3 enemy_position = getEnemyPosition();
    rotateToTarget(enemy_position);
    moveForward();
    if (getMovementDir() != 1) {
        msgAnimationMovement(5, 0.2f, 0.2f);
        setMovementDir(1);
    }
    return SUCCESS;
}

int bt_agresiveEnemy::TaskAttack1() {
    setState("Attack 1");
    VEC3 enemy_position = getEnemyPosition();
    rotateToTarget(enemy_position);

    if (!getPlayingAnim()) msgAnimation(9, 0.0f, 0.2f);

    if (!getAnimEnded()) {
        return IN_PROGRESS;
    }
    else {
        attack();
        return SUCCESS;
    }
}

int bt_agresiveEnemy::TaskAttack2() {
    setState("Attack 2");
    VEC3 enemy_position = getEnemyPosition();
    rotateToTarget(enemy_position);

    if (!getPlayingAnim()) msgAnimation(13, 0.0f, 0.2f);

    if (!getAnimEnded()) {
        return IN_PROGRESS;
    }
    else {
        attack();
        return SUCCESS;
    }
}

int bt_agresiveEnemy::TaskCombo1() {
    setState("Combo 1");
    VEC3 enemy_position = getEnemyPosition();
    rotateToTarget(enemy_position);

    if (!getPlayingAnim()) msgAnimation(14, 0.0f, 0.2f);

    if (!getAnimEnded()) {
        return IN_PROGRESS;
    }
    else {
        attack();
        return SUCCESS;
    }
}

int bt_agresiveEnemy::TaskCombo2() {
    setState("Combo 2");
    VEC3 enemy_position = getEnemyPosition();
    rotateToTarget(enemy_position);

    if (!getPlayingAnim()) msgAnimation(15, 0.0f, 0.2f);

    if (!getAnimEnded()) {
        return IN_PROGRESS;
    }
    else {
        attack();
        return SUCCESS;
    }
}

int bt_agresiveEnemy::TaskMantainDistance() {
    setState("Mantain distance");

    VEC3 enemy_position = getEnemyPosition();
    VEC3 my_position = getMyPosition();

    rotateToTarget(enemy_position);
    rotateAroundRight(enemy_position);

    if (getMovementDir() != 2) {
        msgAnimationMovement(8, 0.2f, 0.2f);
        setMovementDir(2);
    }

    //float dist = VEC3::Distance(my_position, enemy_position);
    //if (dist > 5) moveForward();
    //else moveBackwards();

    //if (rand() % 10 == 0) return IN_PROGRESS;
    return SUCCESS;
}

int bt_agresiveEnemy::TaskTaunt() {
    setState("Taunt");
    VEC3 enemy_position = getEnemyPosition();
    rotateToTarget(enemy_position);

    if (!getAnimAux()) {
        animAux(true);
        msgAnimation(4, 0.0f, 0.0f);
    }

    if (!getAnimEnded()) {
        return IN_PROGRESS;
    }
    else {
        animAux(false);
        return SUCCESS;
    }
}