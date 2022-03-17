#pragma once

#include "entity/entity.h"
#include "fsm/fsm.fwd.h"

// Declare a struct/class to hold the msg arguments
// Inside add the macro DECL_MSG_ID();
// Inside each component that wants to receive that msg, add the following:
//
// static void registerMsgs() {
//   DECL_MSG(TCompName, TMsgName, myMethodName);
// }
//
// The component must implement a method with the signature:
// void myMethodName( const TMsg& msg ) { ... do whatever with msg... }
//
// The msg 'dies' after the sendMsg finishes.

struct TMsgExplosion {
  DECL_MSG_ID();
  VEC3 center;
  float radius = 5.0f;
  float damage = 10.0f;
};

struct TMsgEnemyDies {
  DECL_MSG_ID();
  CHandle h_enemy;
};

// ------------------------------
struct TMsgDefineLocalAABB {
  DECL_MSG_ID();
  AABB* aabb = nullptr;
};

struct TMsgFSMVariable {
    std::string name;
    fsm::TVariableValue value;
    DECL_MSG_ID();
};

struct TMsgShake {
    float amount = 0.f;
    bool enabled = false;
    DECL_MSG_ID();
};

struct TMsgAlarm {
  DECL_MSG_ID();
  CHandle who_fired_the_alarm;
  VEC3    alarm_position;
};

//Mensajes propios

struct TMsgEngage {
    DECL_MSG_ID();
    CHandle me;
};

struct TMsgAskForAttacker {
    DECL_MSG_ID();
    CHandle me;
};

struct TMsgAttacker {
    DECL_MSG_ID();
    CHandle attacker;
};

struct TMsgDying {
    DECL_MSG_ID();
    CHandle me;
};

struct TMsgDestroyMe {
    DECL_MSG_ID();
};

struct TMsgChangeAttacker {
    DECL_MSG_ID();
    CHandle me;
};

struct TMsgShoot {
    DECL_MSG_ID();
    MAT44 mtx;
};

struct TMsgPotion {
    DECL_MSG_ID();
}; 

struct TMsgPowerUpDefense {
    DECL_MSG_ID();
};

struct TMsgShootHit {
    DECL_MSG_ID();
};

struct TMsgKill {
    DECL_MSG_ID();
};

//LUA Msg

struct TMsgSetLuaManager {
    DECL_MSG_ID();
    CHandle h_luaManager;
};

struct TMsgNextWave {
    DECL_MSG_ID();
};

struct TMsgSetLife {
    DECL_MSG_ID();
    float life;
};

struct TMsgSetPotionHeal {
    DECL_MSG_ID();
    int potionHeal;
};

struct TMsgSetHitDamage {
    DECL_MSG_ID();
    int playerHitDamage;
};

struct TMsgSetMaxLife {
    DECL_MSG_ID();
    float maxLife;
};

struct TMsgSpawnRandomEnemies {
    DECL_MSG_ID();
    int numEnemies;
};

struct TMsgSpawnPosition {
    DECL_MSG_ID();
    float px;
    float py;
    float pz;
};