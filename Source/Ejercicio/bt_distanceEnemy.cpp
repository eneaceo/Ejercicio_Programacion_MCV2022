#include "mcv_platform.h"
#include "bt_distanceEnemy.h"

//{ "name": "idle" },
//{ "name": "aim_idle" },
//{ "name": "impact" },
//{ "name": "dying" },
//{ "name": "taunt" },
//{ "name": "walk_forward" },
//{ "name": "walk_back" },
//{ "name": "walk_left" },
//{ "name": "walk_right" },
//{ "name": "draw_arrow" },
//{ "name": "shoot_arrow" },
//{ "name": "mele_punch" },
//{ "name": "mele_kick" }

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

    AddDecoratorNode("combat", "dec_attack", (bttask)&bt_distanceEnemy::DecoratorAttack);
	AddDecoratorNode("combat", "dec_chase", (bttask)&bt_distanceEnemy::DecoratorChase);
    AddDecoratorNode("combat", "dec_moveback", (bttask)&bt_distanceEnemy::DecoratorMoveBack);
	AddDecoratorNode("combat", "dec_shoot", (bttask)&bt_distanceEnemy::DecoratorShoot);
    AddTaskNode("dec_chase", "chase", (bttask)&bt_distanceEnemy::TaskChase);
    AddTaskNode("dec_moveback", "moveback", (bttask)&bt_distanceEnemy::TaskMoveBack);
    //AddCompositeNode("dec_shoot", "shootcombat", PARALLEL);
    //AddTaskNode("shootcombat", "movearound", (bttask)&bt_distanceEnemy::TaskMoveAround);
    AddTaskNode("dec_shoot", "shoot", (bttask)&bt_distanceEnemy::TaskShoot);
    AddTaskNode("dec_attack", "attack", (bttask)&bt_distanceEnemy::TaskAttack);
    AddTaskNode("combat", "combatIdle", (bttask)&bt_distanceEnemy::TaskCombatIdle);

    //Shoot not engaging
    AddDecoratorNode("root", "dec_movebackfar", (bttask)&bt_distanceEnemy::DecoratorMoveBackFar);
    AddTaskNode("dec_movebackfar", "movebackfar", (bttask)&bt_distanceEnemy::TaskMoveBackFar);
    AddDecoratorNode("root", "dec_mantaindistance", (bttask)&bt_distanceEnemy::DecoratorMantainDistance);
    AddTaskNode("dec_mantaindistance", "mantaindistance", (bttask)&bt_distanceEnemy::TaskMantainDistance);
    AddDecoratorNode("root", "dec_shootfar", (bttask)&bt_distanceEnemy::DecoratorShootFar);
    AddTaskNode("dec_shootfar", "shootfar", (bttask)&bt_distanceEnemy::TaskShootFar);
    //idle
    AddDecoratorNode("root", "dec_taunt", (bttask)&bt_distanceEnemy::DecoratorTaunt);
    AddTaskNode("dec_taunt", "taunt", (bttask)&bt_distanceEnemy::TaskTaunt);
    AddTaskNode("root", "idle", (bttask)&bt_distanceEnemy::TaskIdle);
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

    if (rand() % 4 == 0) return SUCCESS;
    else return FAIL;
}

int bt_distanceEnemy::DecoratorMoveBack() {

    VEC3 enemy_position = getEnemyPosition();
    VEC3 my_position = getMyPosition();
    float dist = VEC3::Distance(my_position, enemy_position);

    if (dist < 3)return SUCCESS;
    else return FAIL;
}

int bt_distanceEnemy::DecoratorAttack() {

    VEC3 enemy_position = getEnemyPosition();
    VEC3 my_position = getMyPosition();
    float dist = VEC3::Distance(my_position, enemy_position);

    if (dist < 2) return SUCCESS;
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

    if (rand() % 30 == 0) return SUCCESS;
    else return FAIL;
}

int bt_distanceEnemy::DecoratorMantainDistance() {

    VEC3 enemy_position = getEnemyPosition();
    VEC3 my_position = getMyPosition();
    float dist = VEC3::Distance(my_position, enemy_position);

    if (dist > 9) return SUCCESS;
    else return FAIL;
}

int bt_distanceEnemy::DecoratorTaunt() {

    if (rand() % 60 == 0) return SUCCESS;
    else return FAIL;
}

//TASKS
int bt_distanceEnemy::TaskIdle() {
	setState("Idle");
    
	return SUCCESS;
}

int bt_distanceEnemy::TaskCombatIdle() {
    setState("Combat Idle");
    VEC3 enemy_position = getEnemyPosition();
    rotateToTarget(enemy_position);

    if (!getPlayingAnim()) msgAnimation(1, 0.0f, 0.2f);

    if (!getAnimEnded()) {
        return IN_PROGRESS;
    }
    else {
        return SUCCESS;
    }

}

int bt_distanceEnemy::TaskProcessImpact() {
   setState("Process Impact");
   if (getDamage() > 20 || rand() % 5 == 0) changeAttacker();

   msgAnimation(2, 0.0f, 0.2f);

   return SUCCESS;
}

int bt_distanceEnemy::TaskDie() {
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
    if (getMovementDir() != 1) {
        msgAnimationMovement(6, 0.2f, 0.2f);
        setMovementDir(1);
    }

    return SUCCESS;
}

int bt_distanceEnemy::TaskMoveAround() {
    setState("Move Around");
    VEC3 enemy_position = getEnemyPosition();
    VEC3 my_position = getMyPosition();

    rotateToTarget(enemy_position);
    rotateAroundRight(enemy_position);

    float dist = VEC3::Distance(my_position, enemy_position);
    if (dist > 6.5f) {
        moveForward();
        if (getMovementDir() != 1) {
            msgAnimation(6, 0.2f, 0.2f);
            setMovementDir(1);
        }
    }

    if (dist < 5.5f) {
        moveBackwards();
        if (getMovementDir() != 4) {
            msgAnimation(7, 0.2f, 0.2f);
            setMovementDir(4);
        }
    }
    return SUCCESS;
}

int bt_distanceEnemy::TaskShoot() {
    setState("Combat Shoot");
    VEC3 enemy_position = getEnemyPosition();
    rotateToTarget(enemy_position);

    if (!getPlayingAnim()) msgAnimation(10, 0.0f, 0.2f);

    if (!getAnimEnded()) {
        return IN_PROGRESS;
    }
    else {
        shoot();
        return SUCCESS;
    }
}

int bt_distanceEnemy::TaskMoveBack() {
    setState("Move Back");
    moveBackwards();
    if (getMovementDir() != 4) {
        msgAnimation(7, 0.2f, 0.2f);
        setMovementDir(4);
    }
    return SUCCESS;
}

int bt_distanceEnemy::TaskAttack() {
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

int bt_distanceEnemy::TaskMoveBackFar() {
    setState("Move Back Far");
    moveBackwards();
    if (getMovementDir() != 4) {
        msgAnimation(7, 0.2f, 0.2f);
        setMovementDir(4);
    }
    return SUCCESS;
}

int bt_distanceEnemy::TaskShootFar() {
    setState("Shoot Far");
    VEC3 enemy_position = getEnemyPosition();
    rotateToTarget(enemy_position);

    if (!getPlayingAnim()) msgAnimation(10, 0.0f, 0.2f);

    if (!getAnimEnded()) {
        return IN_PROGRESS;
    }
    else {
        shoot();
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
    if (getMovementDir() != 1) {
            msgAnimation(6, 0.2f, 0.2f);
            setMovementDir(1);
    }
    return SUCCESS;
}

int bt_distanceEnemy::TaskTaunt() {
    setState("Taunt");
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