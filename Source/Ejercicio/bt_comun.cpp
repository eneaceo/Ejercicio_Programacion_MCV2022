#include "mcv_platform.h"
#include "bt_comun.h"

void bt_comun::Init(CHandle h_target, CHandle myh_transform, CHandle myh_entity, CHandle h_btmanager) {
    this->h_target = h_target;
    this->myh_transform = myh_transform;
    this->myh_entity = myh_entity;
    this->h_btmanager = h_btmanager;
}

void bt_comun::setStats(float speed, float turn_speed, float life) {
    this->speed = speed;
    this->turn_speed = turn_speed;
    this->life = life;
    state = "Thinking";
    impact = false;
}

float bt_comun::getLife() {
    return life;
}

void bt_comun::setLife(float life) {
    this->life = life;
}

bool bt_comun::getImpact() {
    return impact;
}

void bt_comun::setImpact(bool impact) {
    this->impact = impact;
}

float bt_comun::getDamage() {
    return damage;
}

void bt_comun::setDamage(float damage) {
    this->damage = damage;
}

float bt_comun::getTimer() {
    return timer;
}

void bt_comun::setTimer(float timer) {
    this->timer = timer;
}

void bt_comun::resetTimer() {
    timer = 0;
}

float bt_comun::getDeltaTime() {
    return delta_time;
}

bool bt_comun::iAmAttacker() {
    if (attacker == myh_entity) return true;
    else return false;
}

void bt_comun::updateAttacker(CHandle attacker) {
    this->attacker = attacker;
}

void bt_comun::askForAttacker() {
    TMsgAskForAttacker msg;
    CEntity* msg_target = h_btmanager;
    msg.me = myh_entity;
    msg_target->sendMsg(msg);
}

bool bt_comun::attackerSlotFree() {
    return !attacker.isValid();
}

void bt_comun::changeAttacker() {
    TMsgChangeAttacker msg;
    CEntity* msg_target = h_btmanager;
    msg.me = myh_entity;
    msg_target->sendMsg(msg);
}

void bt_comun::msgDying() {
    TMsgDying msg;
    CEntity* msg_target = h_btmanager;
    msg.me = myh_entity;
    msg_target->sendMsg(msg);
}

void bt_comun::msgDestroyMe() {
    TMsgDestroyMe msg;
    CEntity* msg_target = myh_entity;
    msg_target->sendMsg(msg);
}

//UPDATES

void bt_comun::updateTime(float delta_time) {
    this->delta_time = delta_time;
}

std::string bt_comun::getState() {
    return state;
}

void bt_comun::setState(std::string state) {
    this->state = state;
}

//UTILITY FUNCTIONS

VEC3 bt_comun::getMyPosition() {
    TCompTransform* my_transform = myh_transform;
    return my_transform->getPosition();
}

VEC3 bt_comun::getMyForward() {
    TCompTransform* my_transform = myh_transform;
    return my_transform->getForward();
}

VEC3 bt_comun::getEnemyPosition() {
    CEntity* e_target = h_target;
    TCompTransform* target_transform = e_target->get<TCompTransform>();
    return target_transform->getPosition();
}

void bt_comun::rotateAroundRight(VEC3 enemy_position) {

    TCompTransform* my_transform = myh_transform;

    VEC3 mypos = my_transform->getPosition();
    VEC3 fwd = my_transform->getForward();
    fwd.Normalize();
    float dist = VEC3::Distance(mypos, enemy_position);
    fwd = fwd * dist;

    mypos = my_transform->getPosition() + fwd;
    my_transform->setPosition(mypos);

    float amount_rotated = deg2rad(0.2f);
    QUAT delta_rotation = QUAT::CreateFromAxisAngle(VEC3(0, 1, 0), amount_rotated);
    my_transform->setRotation(my_transform->getRotation() * delta_rotation);

    fwd = my_transform->getForward();
    fwd.Normalize();
    fwd = fwd * dist;
    mypos = my_transform->getPosition() - fwd;
    my_transform->setPosition(mypos);
}

void bt_comun::rotateAroundLeft(VEC3 enemy_position) {

    TCompTransform* my_transform = myh_transform;

    VEC3 mypos = my_transform->getPosition();
    VEC3 fwd = my_transform->getForward();
    fwd.Normalize();
    float dist = VEC3::Distance(mypos, enemy_position);
    fwd = fwd * dist;

    mypos = my_transform->getPosition() + fwd;
    my_transform->setPosition(mypos);

    float amount_rotated = -deg2rad(0.2f);
    QUAT delta_rotation = QUAT::CreateFromAxisAngle(VEC3(0, 1, 0), amount_rotated);
    my_transform->setRotation(my_transform->getRotation() * delta_rotation);

    fwd = my_transform->getForward();
    fwd.Normalize();
    fwd = fwd * dist;
    mypos = my_transform->getPosition() - fwd;
    my_transform->setPosition(mypos);
}

void bt_comun::rotateToTarget(VEC3 enemy_position) {

    TCompTransform* my_transform = myh_transform;
    float angle_to_aim = my_transform->getYawRotationToAimTo(enemy_position);
    angle_to_aim *= turn_speed;
    QUAT my_rot = my_transform->getRotation();
    my_rot *= QUAT::CreateFromAxisAngle(VEC3::UnitY, angle_to_aim);
    my_transform->setRotation(my_rot);
}

void bt_comun::snapRotateToTarget(VEC3 enemy_position) {

    TCompTransform* my_transform = myh_transform;
    float angle_to_aim = my_transform->getYawRotationToAimTo(enemy_position);
    QUAT my_rot = my_transform->getRotation();
    my_rot *= QUAT::CreateFromAxisAngle(VEC3::UnitY, angle_to_aim);
    my_transform->setRotation(my_rot);
}

void bt_comun::moveForward() {

    TCompTransform* my_transform = myh_transform;
    float amount_moved = speed * delta_time;
    VEC3 delta_moved = my_transform->getForward() * amount_moved;
    my_transform->setPosition(my_transform->getPosition() + delta_moved);
}

void bt_comun::moveBackwards() {

    TCompTransform* my_transform = myh_transform;
    float amount_moved = speed * delta_time;
    VEC3 delta_moved = my_transform->getForward() * amount_moved;
    my_transform->setPosition(my_transform->getPosition() + (-delta_moved));
}

void bt_comun::shoot() {
    TCompTransform* my_transform = myh_transform;
    VEC3 my_pos = my_transform->getPosition();
    VEC3 dir_to_enemy = (getEnemyPosition() - my_pos);
    dir_to_enemy.Normalize();
    dir_to_enemy *= 10.0f;
    float yaw = vectorToYaw(dir_to_enemy);
    MAT44 mtx = MAT44::CreateFromAxisAngle(VEC3(0, 1, 0), yaw) * MAT44::CreateTranslation(my_pos);
    TMsgShoot msg;
    CEntity* msg_target = myh_entity;
    msg.mtx = mtx;
    msg_target->sendMsg(msg);
}