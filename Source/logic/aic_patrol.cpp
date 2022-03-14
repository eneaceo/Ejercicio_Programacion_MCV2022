#include <windows.h>
#include "mcv_platform.h"
#include "aic_patrol.h"

void aic_patrol::Init()
{
// insert all states in the map
AddState("resetwpt",(statehandler)&aic_patrol::ResetWptState);
AddState("followwpt",(statehandler)&aic_patrol::FollowWptState);
AddState("changewpt", (statehandler)&aic_patrol::ChangeWptState);
AddState("chase",(statehandler)&aic_patrol::ChaseState);
AddState("attack", (statehandler)&aic_patrol::AttackState);
AddState("idlewar", (statehandler)&aic_patrol::IdleWarState);
AddState("selectside", (statehandler)&aic_patrol::SelectSideState);
AddState("orbitleft", (statehandler)&aic_patrol::OrbitLeftState);
AddState("orbitright", (statehandler)&aic_patrol::OrbitRightState);

// reset the state
ChangeState("resetwpt");

/// initialize data structure 
wpts = new VEC3[4];
nwpts = 4;
for (int i = 0; i < 4; i++)
  {
  int dx = (((float)(rand() % 100)) - 50) / 3.0;
  int dz = (((float)(rand() % 100)) - 50) / 3.0;
  VEC3 delta;
  delta.x = dx;
  delta.y = 0;
  delta.z = dz;
  wpts[i] = delta;
  }
}



void aic_patrol::ResetWptState()
{
VEC3 mypos=e->transform.getPosition();

int curwpt = 0;
float dist = 100000;
for (int i = 0; i < 4; i++)
{
  float newdist = VEC3::Distance(mypos, wpts[i]);
  if (newdist < dist) curwpt = i;
}
ChangeState("followwpt");
}


void aic_patrol::FollowWptState()
{/*
  float deltayaw=e->transform.getYawRotationToAimTo(wpts[curwpt]);
  
  float amount_rotated = std::min(deg2rad(10.0f), fabsf(deltayaw));
  if (deltayaw < 0)
    amount_rotated = -amount_rotated;
  QUAT delta_rotation = QUAT::CreateFromAxisAngle(VEC3(0, 1, 0), amount_rotated);
  e->transform.setRotation(e->transform.getRotation() * delta_rotation);

  VEC3 fwd = e->transform.getForward();
  fwd = fwd * 0.1;
  VEC3 mypos = e->transform.getPosition() + fwd;
  e->transform.setPosition(mypos);

  float dist=VEC3::Distance(mypos, wpts[curwpt]);

  if (dist<0.5) 
    ChangeState("changewpt");

  //dist = VEC3::Distance(mypos, player.transform.getPosition());
  //  bool inside_cone = player.transform.isInsideCone(e->transform.getPosition(), deg2rad(90),15.0);
  
//if (inside_cone) ChangeState("chase");
*/
}


void aic_patrol::ChangeWptState()
{
  curwpt = (curwpt + 1) % nwpts;
  ChangeState("followwpt");
}



void aic_patrol::ChaseState()
{
  /*
float deltayaw = e->transform.getYawRotationToAimTo(getTargetPosition());

float amount_rotated = std::min(deg2rad(10.0f), fabsf(deltayaw));
if (deltayaw < 0)
  amount_rotated = -amount_rotated;
QUAT delta_rotation = QUAT::CreateFromAxisAngle(VEC3(0, 1, 0), amount_rotated);
e->transform.setRotation(e->transform.getRotation() * delta_rotation);

VEC3 fwd = e->transform.getForward();
fwd = fwd * 0.1;
e->transform.setPosition(e->transform.getPosition() + fwd);

VEC3 mypos = e->transform.getPosition() + fwd;
float dist = VEC3::Distance(mypos, getTargetTransform.getPosition());
bool inside_cone = getTargetTransform().isInsideCone(e->transform.getPosition(), deg2rad(90),15);
if (!inside_cone) ChangeState("resetwpt");

if (dist < 2) ChangeState("idlewar");
*/
}


unsigned long idlewar_in = -1;

void aic_patrol::IdleWarState()
{/*
  // inicializo timer para salir en 1 segundo
  if (idlewar_in == -1) idlewar_in = timeGetTime();

  unsigned long now = timeGetTime();

  if (now - idlewar_in > 1000) // 1 segundo
  {
    int r = rand() % 100;   // elijo con la tabla qué quiero hacer
    if (r<aggressive) ChangeState("attack");
    else if (r < aggressive+mobile) ChangeState("selectside");
    idlewar_in = -1;
  }

  // y ya puestos me mantengo encarado para micro-movimientos del player
  float deltayaw = e->transform.getYawRotationToAimTo(player.transform.getPosition());

  float amount_rotated = std::min(deg2rad(10.0f), fabsf(deltayaw));
  if (deltayaw < 0)
    amount_rotated = -amount_rotated;
  QUAT delta_rotation = QUAT::CreateFromAxisAngle(VEC3(0, 1, 0), amount_rotated);
  e->transform.setRotation(e->transform.getRotation() * delta_rotation);

  // si el player huye, le persigo
  float dist = VEC3::Distance(e->transform.getPosition(), player.transform.getPosition());
  if (dist > 2.5) ChangeState("chase");
  */
}


unsigned long attack_in = -1;

void aic_patrol::AttackState()
{
  /*
  if (attack_in == -1) attack_in = timeGetTime();

  unsigned long now= timeGetTime();

  if (now - attack_in > 1000) // 1 segundo
  {
    ChangeState("idlewar");
    attack_in = -1;
  }

  // y ya puestos me mantengo encarado para micro-movimientos del player
  float deltayaw = e->transform.getYawRotationToAimTo(player.transform.getPosition());

  float amount_rotated = std::min(deg2rad(10.0f), fabsf(deltayaw));
  if (deltayaw < 0)
    amount_rotated = -amount_rotated;
  QUAT delta_rotation = QUAT::CreateFromAxisAngle(VEC3(0, 1, 0), amount_rotated);
  e->transform.setRotation(e->transform.getRotation() * delta_rotation);
  */
}



void aic_patrol::SelectSideState()
{
// random: 0 left 1 right
  int r = rand() % 2;
  if (r) ChangeState("orbitleft"); else ChangeState("orbitright");
}


unsigned long orbitleft_in = -1;

void aic_patrol::OrbitLeftState()
{
  if (orbitleft_in == -1) orbitleft_in = timeGetTime();

  unsigned long now = timeGetTime();

  if (now - orbitleft_in > 300) // 
  {
    ChangeState("idlewar");
    orbitleft_in = -1;
  }

  float dist = VEC3::Distance(e->transform.getPosition(), player.transform.getPosition());
  if (dist > 2.5) ChangeState("chase");

VEC3 mypos = e->transform.getPosition();
VEC3 fwd = e->transform.getForward();
fwd.Normalize();
dist = VEC3::Distance(mypos, player.transform.getPosition());
fwd = fwd * dist;

// paso 1: translate to player
mypos = e->transform.getPosition() + fwd;
e->transform.setPosition(mypos);

// paso 2: rotate yaw
float amount_rotated = deg2rad(10.0f);
QUAT delta_rotation = QUAT::CreateFromAxisAngle(VEC3(0, 1, 0), amount_rotated);
e->transform.setRotation(e->transform.getRotation() * delta_rotation);

// paso 3: translate inversa
fwd = e->transform.getForward();
fwd.Normalize();
fwd = fwd * dist; 
mypos = e->transform.getPosition() - fwd; // inversa!!!
e->transform.setPosition(mypos);
}



unsigned long orbitright_in = -1;

void aic_patrol::OrbitRightState()
{
  if (orbitright_in == -1) orbitright_in = timeGetTime();

  unsigned long now = timeGetTime();

  if (now - orbitright_in > 300) 
  {
    ChangeState("idlewar");
    orbitright_in = -1;
  }

  float dist = VEC3::Distance(e->transform.getPosition(), player.transform.getPosition());
  if (dist > 2.5) ChangeState("chase");

  VEC3 mypos = e->transform.getPosition();
  VEC3 fwd = e->transform.getForward();
  fwd.Normalize();
  dist = VEC3::Distance(mypos, player.transform.getPosition());
  fwd = fwd * dist;
  // paso 1: translate to player
  mypos = e->transform.getPosition() + fwd;
  e->transform.setPosition(mypos);

  // paso 2: rotate yaw
  float amount_rotated = -deg2rad(10.0f);
  QUAT delta_rotation = QUAT::CreateFromAxisAngle(VEC3(0, 1, 0), amount_rotated);
  e->transform.setRotation(e->transform.getRotation() * delta_rotation);

  // paso 3: translate inversa
  fwd = e->transform.getForward();
  fwd.Normalize();
  fwd = fwd * dist;
  mypos = e->transform.getPosition() - fwd; // inversa!!!
  e->transform.setPosition(mypos);
}





