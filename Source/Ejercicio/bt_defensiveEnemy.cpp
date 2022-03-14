#include "mcv_platform.h"
#include "bt_defensiveEnemy.h"


void bt_defensiveEnemy::InitTree() {

	CreateRootNode("root", SELECTOR);
    //Impact
    AddDecoratorNode("root", "dec_impact", (bttask)&bt_defensiveEnemy::DecoratorImpact);
    AddTaskNode("dec_impact", "processimpact", (bttask)&bt_defensiveEnemy::TaskProcessImpact);
    //Dying
    AddDecoratorNode("root", "dec_dying", (bttask)&bt_defensiveEnemy::DecoratorDying);
    AddTaskNode("dec_dying", "die", (bttask)&bt_defensiveEnemy::TaskDie);
    //Attacker Slot
    AddDecoratorNode("root", "dec_attacker", (bttask)&bt_defensiveEnemy::DecoratorAttacker);
    AddTaskNode("dec_attacker", "attacker", (bttask)&bt_defensiveEnemy::TaskAttacker);
    //combat
    AddDecoratorNode("root", "dec_combat", (bttask)&bt_defensiveEnemy::DecoratorCombat);
	AddCompositeNode("dec_combat", "combat", SELECTOR);
	AddDecoratorNode("combat", "dec_chase", (bttask)&bt_defensiveEnemy::DecoratorChase);
	AddDecoratorNode("combat", "dec_engage", (bttask)&bt_defensiveEnemy::DecoratorEngage);
    AddTaskNode("dec_chase", "chase", (bttask)&bt_defensiveEnemy::TaskChase);
    AddCompositeNode("dec_engage", "stand", SELECTOR);
    AddDecoratorNode("stand", "dec_defend", (bttask)&bt_defensiveEnemy::DecoratorDefend);
    AddTaskNode("dec_defend", "defend", (bttask)&bt_defensiveEnemy::TaskDefend);
    AddTaskNode("stand", "attack", (bttask)&bt_defensiveEnemy::TaskAttack);
    //taunt
    AddDecoratorNode("root", "dec_taunt", (bttask)&bt_defensiveEnemy::DecoratorTaunt);
    AddTaskNode("dec_taunt", "taunt", (bttask)&bt_defensiveEnemy::TaskTaunt);
    //idle
    AddCompositeNode("root", "idle", PARALLEL);
    AddTaskNode("idle", "mantaindistance", (bttask)&bt_defensiveEnemy::TaskMantainDistance);
    AddTaskNode("idle", "defendstand", (bttask)&bt_defensiveEnemy::TaskDefendStand);

}


//DECORATORS

int bt_defensiveEnemy::DecoratorImpact() {
    if (getImpact()) return SUCCESS;
    else return FAIL;
}

int bt_defensiveEnemy::DecoratorDying() {
    if (getLife() <= 0) return SUCCESS;
    else return FAIL;
}

int bt_defensiveEnemy::DecoratorAttacker() {
    if (attackerSlotFree()) return SUCCESS;
    else return FAIL;
}

int bt_defensiveEnemy::DecoratorCombat() {
    if (iAmAttacker()) return SUCCESS;
    else return FAIL;
}

int bt_defensiveEnemy::DecoratorChase() {
    VEC3 enemy_position = getEnemyPosition();
    VEC3 my_position = getMyPosition();
    float dist = VEC3::Distance(my_position, enemy_position);

    if (dist > 2) return SUCCESS;
    else return FAIL;
}

int bt_defensiveEnemy::DecoratorEngage() {
    VEC3 enemy_position = getEnemyPosition();
    VEC3 my_position = getMyPosition();
    float dist = VEC3::Distance(my_position, enemy_position);

    if (dist < 2) return SUCCESS;
    else return FAIL;
}

int bt_defensiveEnemy::DecoratorDefend() {
    if (rand() % 100 == 0) return SUCCESS;
    else return FAIL;
}

int bt_defensiveEnemy::DecoratorTaunt() {
    if (rand() % 300 == 0) return SUCCESS;
    else return FAIL;
}

//TASKS
int bt_defensiveEnemy::TaskIdle() {
	setState("Idle");
	return SUCCESS;
}

int bt_defensiveEnemy::TaskProcessImpact() {
    setState("Process Impact");
    if (getDamage() > 20 || rand() % 5 == 0) changeAttacker();
    resetTimer();
    return SUCCESS;
}

int bt_defensiveEnemy::TaskDie() {
    setState("Dying");
    msgDying();
    msgDestroyMe();
    return SUCCESS;
}

int bt_defensiveEnemy::TaskAttacker() {
    setState("Attacker");
    askForAttacker();
    return SUCCESS;
}

int bt_defensiveEnemy::TaskChase() {
    setState("Chase");
    VEC3 enemy_position = getEnemyPosition();
    rotateToTarget(enemy_position);
    moveForward();
    return SUCCESS;
}

int bt_defensiveEnemy::TaskDefend() {
    setState("Defend");
    VEC3 enemy_position = getEnemyPosition();
    rotateToTarget(enemy_position);
    float time = getTimer();
    if (time < 0.5f) {
        setTimer(getDeltaTime() + time);
        return IN_PROGRESS;
    }
    else {
        resetTimer();
        return SUCCESS;
    }
}

int bt_defensiveEnemy::TaskAttack() {
    setState("Attack");
    VEC3 enemy_position = getEnemyPosition();
    rotateToTarget(enemy_position);
    float time = getTimer();
    if (time < 0.5f) {
        setTimer(getDeltaTime() + time);
        return IN_PROGRESS;
    }
    else {
        resetTimer();
        return SUCCESS;
    }
}

int bt_defensiveEnemy::TaskTaunt() {
    setState("Taunt");
    VEC3 enemy_position = getEnemyPosition();
    rotateToTarget(enemy_position);
    float time = getTimer();
    if (time < 0.5f) {
        setTimer(getDeltaTime() + time);
        return IN_PROGRESS;
    }
    else {
        resetTimer();
        return SUCCESS;
    }
}

int bt_defensiveEnemy::TaskMantainDistance() {
    setState("Mantain Distance");

    VEC3 enemy_position = getEnemyPosition();
    VEC3 my_position = getMyPosition();

    rotateToTarget(enemy_position);
    rotateAroundRight(enemy_position);

    float dist = VEC3::Distance(my_position, enemy_position);
    if (dist > 5) moveForward();
    else moveBackwards();

    if (rand() % 3 == 0) return IN_PROGRESS;
    else return SUCCESS;
}

int bt_defensiveEnemy::TaskDefendStand() {
    setState("Defend Stance");
    VEC3 enemy_position = getEnemyPosition();
    rotateToTarget(enemy_position);
    float time = getTimer();
    if (time < 0.5f) {
        setTimer(getDeltaTime() + time);
        return IN_PROGRESS;
    }
    else {
        resetTimer();
        return SUCCESS;
    }
}
