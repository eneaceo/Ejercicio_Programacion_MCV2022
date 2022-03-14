#include "mcv_platform.h"
#include "bt_distanceEnemy.h"


void bt_distanceEnemy::InitTree() {

	CreateRootNode("root", SELECTOR);
    //Impact
    AddDecoratorNode("root", "dec_impact", (bttask)&bt_distanceEnemy::DecoratorImpact);
    AddTaskNode("dec_impact", "processimpact", (bttask)&bt_distanceEnemy::TaskProcessImpact);
    //Dying
    AddDecoratorNode("root", "dec_dying", (bttask)&bt_distanceEnemy::DecoratorDying);
    AddTaskNode("dec_dying", "die", (bttask)&bt_distanceEnemy::TaskDie);
    //Attacker Slot
    AddDecoratorNode("root", "dec_attacker", (bttask)&bt_distanceEnemy::DecoratorAttacker);
    AddTaskNode("dec_attacker", "attacker", (bttask)&bt_distanceEnemy::TaskAttacker);
    //combat
    AddDecoratorNode("root", "dec_combat", (bttask)&bt_distanceEnemy::DecoratorCombat);
	AddCompositeNode("dec_combat", "combat", SELECTOR);
	AddDecoratorNode("combat", "dec_chase", (bttask)&bt_distanceEnemy::DecoratorChase);
	AddDecoratorNode("combat", "dec_shoot", (bttask)&bt_distanceEnemy::DecoratorShoot);
    AddDecoratorNode("combat", "dec_moveback", (bttask)&bt_distanceEnemy::DecoratorMoveBack);
    AddTaskNode("combat", "attack", (bttask)&bt_distanceEnemy::TaskAttack);
    AddTaskNode("dec_chase", "chase", (bttask)&bt_distanceEnemy::TaskChase);
    AddCompositeNode("dec_shoot", "shootcombat", PARALLEL);
    AddTaskNode("shootcombat", "movearound", (bttask)&bt_distanceEnemy::TaskMoveAround);
    AddTaskNode("shootcombat", "shoot", (bttask)&bt_distanceEnemy::TaskShoot);
    AddTaskNode("dec_moveback", "moveback", (bttask)&bt_distanceEnemy::TaskMoveBack);
    //Shoot not engaging
    AddDecoratorNode("root", "dec_movebackfar", (bttask)&bt_distanceEnemy::DecoratorMoveBackFar);
    AddTaskNode("dec_movebackfar", "movebackfar", (bttask)&bt_distanceEnemy::TaskMoveBackFar);
    AddDecoratorNode("root", "dec_shootfar", (bttask)&bt_distanceEnemy::DecoratorShootFar);
    AddTaskNode("dec_shootfar", "shootfar", (bttask)&bt_distanceEnemy::TaskShootFar);
    AddDecoratorNode("root", "dec_mantaindistance", (bttask)&bt_distanceEnemy::DecoratorMantainDistance);
    AddTaskNode("dec_mantaindistance", "mantaindistance", (bttask)&bt_distanceEnemy::TaskMantainDistance);
    //idle
    AddTaskNode("root", "taunt", (bttask)&bt_distanceEnemy::TaskTaunt);
}


//DECORATORS

int bt_distanceEnemy::DecoratorImpact() {
    if (getImpact()) return SUCCESS;
    else return FAIL;
}

int bt_distanceEnemy::DecoratorDying() {
    if (getLife() <= 0) return SUCCESS;
    else return FAIL;
}

int bt_distanceEnemy::DecoratorAttacker() {
    if (attackerSlotFree()) return SUCCESS;
    else return FAIL;
}

int bt_distanceEnemy::DecoratorCombat() {
    if (iAmAttacker()) return SUCCESS;
    else return FAIL;
}

int bt_distanceEnemy::DecoratorChase() {

    VEC3 enemy_position = getEnemyPosition();
    VEC3 my_position = getMyPosition();
    float dist = VEC3::Distance(my_position, enemy_position);

    if (dist > 7) return SUCCESS;
    else return FAIL;
}

int bt_distanceEnemy::DecoratorShoot() {

    VEC3 enemy_position = getEnemyPosition();
    VEC3 my_position = getMyPosition();
    float dist = VEC3::Distance(my_position, enemy_position);

    if (dist < 7 && dist > 5 && rand() % 150 == 0) return SUCCESS;
    else return FAIL;
}

int bt_distanceEnemy::DecoratorMoveBack() {

    VEC3 enemy_position = getEnemyPosition();
    VEC3 my_position = getMyPosition();
    float dist = VEC3::Distance(my_position, enemy_position);

    if (dist < 5 && dist > 2)return SUCCESS;
    else return FAIL;
}

int bt_distanceEnemy::DecoratorMoveBackFar() {

    VEC3 enemy_position = getEnemyPosition();
    VEC3 my_position = getMyPosition();
    float dist = VEC3::Distance(my_position, enemy_position);

    if (dist < 5) return SUCCESS;
    else return FAIL;
}

int bt_distanceEnemy::DecoratorShootFar() {

    VEC3 enemy_position = getEnemyPosition();
    VEC3 my_position = getMyPosition();
    float dist = VEC3::Distance(my_position, enemy_position);

    if (rand() % 300 == 0) return SUCCESS;
    else return FAIL;
}

int bt_distanceEnemy::DecoratorMantainDistance() {

    VEC3 enemy_position = getEnemyPosition();
    VEC3 my_position = getMyPosition();
    float dist = VEC3::Distance(my_position, enemy_position);

    if (dist > 5) return SUCCESS;
}

//TASKS
int bt_distanceEnemy::TaskIdle() {
	setState("Idle");
	return SUCCESS;
}

int bt_distanceEnemy::TaskProcessImpact() {
    setState("Process Impact");
    if (getDamage() > 20 || rand() % 5 == 0) changeAttacker();
    resetTimer();
    return SUCCESS;
}

int bt_distanceEnemy::TaskDie() {
    setState("Dying");
    msgDying();
    msgDestroyMe();
    return SUCCESS;
}

int bt_distanceEnemy::TaskAttacker() {
    setState("Attacker");
    askForAttacker();
    return SUCCESS;
}

int bt_distanceEnemy::TaskChase() {
    setState("Chase");
    VEC3 enemy_position = getEnemyPosition();
    rotateToTarget(enemy_position);
    moveForward();
    return SUCCESS;
}

int bt_distanceEnemy::TaskMoveAround() {
    setState("Move Around");
    VEC3 enemy_position = getEnemyPosition();
    VEC3 my_position = getMyPosition();

    rotateToTarget(enemy_position);
    rotateAroundRight(enemy_position);

    float dist = VEC3::Distance(my_position, enemy_position);
    if (dist > 6.5f) moveForward();
    if (dist < 5.5f) moveBackwards();
    return SUCCESS;
}

int bt_distanceEnemy::TaskShoot() {
    setState("Combat Shoot");
    VEC3 enemy_position = getEnemyPosition();
    rotateToTarget(enemy_position);
    float time = getTimer();
    if (time < 0.5f) {
        setTimer(getDeltaTime() + time);
        return IN_PROGRESS;
    }
    else {
        shoot();
        resetTimer();
        return SUCCESS;
    }
}

int bt_distanceEnemy::TaskMoveBack() {
    setState("Move Back");
    moveBackwards();
    return SUCCESS;
}

int bt_distanceEnemy::TaskAttack() {
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

int bt_distanceEnemy::TaskMoveBackFar() {
    setState("Move Back Far");
    moveBackwards();
    return SUCCESS;
}

int bt_distanceEnemy::TaskShootFar() {
    setState("Shoot Far");
    VEC3 enemy_position = getEnemyPosition();
    rotateToTarget(enemy_position);
    float time = getTimer();
    if (time < 0.5f) {
        setTimer(getDeltaTime() + time);
        return IN_PROGRESS;
    }
    else {
        shoot();
        resetTimer();
        return SUCCESS;
    }
}

int bt_distanceEnemy::TaskMantainDistance() {
    setState("Mantain Distance");

    VEC3 enemy_position = getEnemyPosition();
    VEC3 my_position = getMyPosition();
    rotateToTarget(enemy_position);

    float dist = VEC3::Distance(my_position, enemy_position);
    moveForward();

    return SUCCESS;
}

int bt_distanceEnemy::TaskTaunt() {
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