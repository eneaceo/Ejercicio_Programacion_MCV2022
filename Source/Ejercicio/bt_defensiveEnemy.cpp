#include "mcv_platform.h"
#include "bt_defensiveEnemy.h"

    //{ "name": "idle" },
    //{ "name": "impact" },
    //{ "name": "impact_block" },
    //{ "name": "dying" },
    //{ "name": "block" },
    //{ "name": "taunt_1" },
    //{ "name": "taunt_2" },
    //{ "name": "walk_forward" },
    //{ "name": "walk_back" },
    //{ "name": "walk_left" },
    //{ "name": "walk_right" },
    //{ "name": "attack_1" },
    //{ "name": "attack_2" },
    //{ "name": "attack_3" }

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
    //AddCompositeNode("root", "idle", PARALLEL);
    AddTaskNode("root", "mantaindistance", (bttask)&bt_defensiveEnemy::TaskMantainDistance);
    //AddTaskNode("idle", "defendstand", (bttask)&bt_defensiveEnemy::TaskDefendStand);

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
    if (rand() % 15 == 0) return SUCCESS;
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
    msgAnimation(1, 0.0f, 0.2f);
    return SUCCESS;
}

int bt_defensiveEnemy::TaskDie() {
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
    if (getMovementDir() != 1) {
        msgAnimationMovement(7, 0.2f, 0.2f);
        setMovementDir(1);
    }
    return SUCCESS;
}

int bt_defensiveEnemy::TaskDefend() {
    setState("Defend");
    VEC3 enemy_position = getEnemyPosition();
    rotateToTarget(enemy_position);

    if (!getPlayingAnim()) msgAnimation(4, 0.0f, 0.2f);

    if (!getAnimEnded()) {
        return IN_PROGRESS;
    }
    else {
        return SUCCESS;
    }
}

int bt_defensiveEnemy::TaskAttack() {
    setState("Attack");
    VEC3 enemy_position = getEnemyPosition();
    rotateToTarget(enemy_position);

    if (!getPlayingAnim()) msgAnimation(11, 0.0f, 0.2f);

    if (!getAnimEnded()) {
        return IN_PROGRESS;
    }
    else {
        attack();
        return SUCCESS;
    }
}

int bt_defensiveEnemy::TaskTaunt() {
    setState("Taunt");
    VEC3 enemy_position = getEnemyPosition();
    rotateToTarget(enemy_position);

    if (!getAnimAux()) {
        animAux(true);
        msgAnimation(5, 0.0f, 0.0f);
    }

    if (!getAnimEnded()) {
        return IN_PROGRESS;
    }
    else {
        animAux(false);
        return SUCCESS;
    }
}

int bt_defensiveEnemy::TaskMantainDistance() {
    setState("Mantain Distance");

    VEC3 enemy_position = getEnemyPosition();
    VEC3 my_position = getMyPosition();

    rotateToTarget(enemy_position);
    rotateAroundRight(enemy_position);

    if (getMovementDir() != 2) {
        msgAnimationMovement(10, 0.2f, 0.2f);
        setMovementDir(2);
    }

    //float dist = VEC3::Distance(my_position, enemy_position);
    //if (dist > 5) {
    //    moveForward();
    //    if (getMovementDir() != 1) {
    //        msgAnimationMovement(7, 0.2f, 0.2f);
    //        setMovementDir(1);
    //    }
    //}
    //else {
    //    moveBackwards();
    //    if (getMovementDir() != 4) {
    //        msgAnimationMovement(8, 0.2f, 0.2f);
    //        setMovementDir(4);
    //    }
    //}

    //if (rand() % 3 == 0) return IN_PROGRESS;
    //else 
        return SUCCESS;
}

int bt_defensiveEnemy::TaskDefendStand() {
    setState("Defend Stance");
    VEC3 enemy_position = getEnemyPosition();
    rotateToTarget(enemy_position);

    if (!getPlayingAnim()) msgAnimation(4, 0.0f, 0.2f);

    if (!getAnimEnded()) {
        return IN_PROGRESS;
    }
    else {
        return SUCCESS;
    }
}
