#include "mcv_platform.h"
#include "components/common/comp_base.h"
#include "components/common/comp_transform.h"
#include "components/common/comp_name.h"
#include "components/common/comp_collider.h"
#include "entity/entity.h"
#include "render/draw_primitives.h"
#include "entity/entity_parser.h"
#include "components/messages.h"
#include "modules/module_physics.h"

class TCompIAJohn : public TCompBase {

  // Macro to allow access from this component to other sibling components using the get<T>()
  DECL_SIBLING_ACCESS()

  VEC3 target;
  float turn_speed = 1.0f;
  float speed = 1.0f;
  std::string target_name;
  CHandle     h_target;
  float radius_alarm = 0.5f;
  float radius_detection = 4.0f;
  bool  in_alarm = false;
  
  void turn(float amount_in_rad) {
    TCompTransform* my_transform = get<TCompTransform>();

    QUAT my_rot = my_transform->getRotation();
    my_rot *= QUAT::CreateFromAxisAngle(VEC3::UnitY, amount_in_rad);
    my_transform->setRotation(my_rot);
  }

  void aimTo(VEC3 aim_target) {
    float angle_to_aim = getAngleToAimTo(aim_target);
    turn(angle_to_aim * 0.02f);
  }

  VEC3 getMyPosition() {
    TCompTransform* my_transform = get<TCompTransform>();
    return my_transform->getPosition();
  }

  // Also implemented in the TTransform as my_transform->getYawRotationToAimTo( aim_target );
  float getAngleToAimTo(VEC3 aim_target) {
    TCompTransform* my_transform = get<TCompTransform>();
    VEC3 my_position = my_transform->getPosition();

    // Vector from my poistion to the target to aim to. Normalized
    VEC3 delta = aim_target - my_position;
    delta.Normalize();

    // Get my 'front' dir normalized
    VEC3 my_fwd = my_transform->getForward();
    VEC3 my_left = -my_transform->getRight();

    // Check the angle... This is not correct.
    float dx = my_fwd.Dot(delta);
    float dy = my_left.Dot(delta);
    float angle = atan2f(dy, dx);
    return angle;
  }

public:

  void onEntityCreated( ) {
    tryToResolveTarget();
  }

  void load(const json& j, TEntityParseContext& ctx) {
    target = loadVEC3(j, "target");
    turn_speed = j.value("turn_speed", turn_speed);
    speed = j.value( "speed", speed);
    target_name = j.value("target_name", target_name);
    radius_alarm = j.value("radius_alarm", radius_alarm);
    radius_detection = j.value("radius_detection", radius_detection);
  }

  void debugInMenu() {
    ImGui::DragFloat3("Target", &target.x, 0.02f, -10.0f, 10.0f );
    ImGui::DragFloat("Turn Speed", &turn_speed, 0.02f, 0.0f, 10.0f);
    ImGui::DragFloat("Advance Speed", &speed, 0.02f, 0.0f, 10.0f);
    ImGui::DragFloat("Radius Alarm", &radius_alarm, 0.02f, 0.0f, 4.0f);
    ImGui::DragFloat("Radius Detection", &radius_detection, 0.02f, 0.0f, 8.0f);
    ImGui::Text("Target Name is %s", target_name.c_str() );
    if( in_alarm )
      ImGui::TextColored(ImVec4(1.0f,0,0,1.0f), "Alarm!!");
    float current_delta_angle = getAngleToAimTo(target);
    ImGui::Text("Error: %f degs", rad2deg( current_delta_angle ));
    
    if (ImGui::SmallButton("Aim To Target")) {
      aimTo( target );
    }

    if (ImGui::SmallButton("Turn Left")) {
      turn(deg2rad(2.0f));
    }
    ImGui::SameLine();
    if (ImGui::SmallButton("Turn Right")) {
      turn(-deg2rad(2.0f));
    }

    static int nbullets = 4;
    ImGui::DragInt("# Bullets", &nbullets, 0.1f, 1, 20);

    if( ImGui::SmallButton("Spawn...")) {
      // Trigger the alarm!!
      // Instantiate a new entity in the world
      TCompTransform* my_transform = get<TCompTransform>();
      VEC3 my_pos = my_transform->getPosition();

      for( int i=0; i<nbullets; ++i ) {
        float angle = (float) i / (float) nbullets * M_PI * 2.0f;
        MAT44 mtx = MAT44::CreateTranslation(my_pos) * MAT44::CreateRotationY(angle);
        TEntityParseContext ctx;
        ctx.root_transform.fromMatrix( mtx );
        parseScene("data/prefabs/bullet_22.json", ctx);
      }
    }

  }

  void renderDebug() {
    PROFILE_FUNCTION("renderDebug");
    // Access to the transform of the entity
    TCompTransform* my_transform = get<TCompTransform>();
    assert(my_transform);

    VEC3 src = my_transform->getPosition();
    drawLine(src, target, Colors::White);

    drawAxis(MAT44::CreateTranslation(target));
    drawCircleXZ(my_transform->asMatrix(), radius_alarm, in_alarm ? Colors::Red : Colors::Yellow);
    drawCircleXZ(my_transform->asMatrix(), radius_detection, in_alarm ? Colors::Red : Colors::Green);
  }

  void tryToResolveTarget() {
    // Try to find in the scene using the name...
    h_target = getEntityByName(target_name);
  }


  void update(float delta_time) {
    PROFILE_FUNCTION("update");

    // Do I have a valid target entity?
    if (!h_target.isValid())
      return;

    // Resolve from handle to entity pointer
    CEntity* e_target = h_target;
    TCompTransform* c_target_transform = e_target->get<TCompTransform>();
    VEC3 enemy_position = c_target_transform->getPosition();

    // Check distance against the enemy target (the player)
    float d = (enemy_position - getMyPosition()).Length();
	if (d > radius_detection) {
		in_alarm = false;
      return;
	}

	if (!in_alarm) {

		TCompTransform* my_transform = get<TCompTransform>();
		VEC3 my_pos = my_transform->getPosition();
		VEC3 dir_to_enemy = (enemy_position - my_pos);
		dir_to_enemy.Normalize();
		dir_to_enemy *= 10.0f;
		float yaw = vectorToYaw(dir_to_enemy);
		MAT44 mtx = MAT44::CreateFromAxisAngle(VEC3(0, 1, 0), yaw) * MAT44::CreateTranslation(my_pos);
		TEntityParseContext ctx;
		ctx.root_transform.fromMatrix(mtx);
		parseScene("data/prefabs/bullet_physics.json", ctx);
		CEntity* e_bullet = ctx.entities_loaded[0];
		TCompCollider* c_collider = e_bullet->get<TCompCollider>();
		if (c_collider && c_collider->actor) {
			physx::PxRigidDynamic* DynActor = static_cast<physx::PxRigidDynamic*>(c_collider->actor);
			DynActor->addForce(VEC3_TO_PXVEC3(dir_to_enemy), physx::PxForceMode::eIMPULSE);
		}

		in_alarm = true;
	}

	/*
    {
      PROFILE_FUNCTION("rotation");
      float angle_to_aim = getAngleToAimTo(enemy_position);
      float amount_rotate = turn_speed * delta_time;
      if (angle_to_aim > 0) {
        // Do not rotate more than what we need to aim to the target
        amount_rotate = std::min(amount_rotate, angle_to_aim);
        turn(amount_rotate);
      }
      else if (angle_to_aim < 0) {
        // Do not rotate more than what we need to aim to the target
        amount_rotate = std::min(amount_rotate, -angle_to_aim);
        turn(-amount_rotate);
      }
    }

    // Move forward...
    float amount_moved = speed * delta_time;
    TCompTransform* my_transform = get<TCompTransform>();
    VEC3 delta_moved = my_transform->getForward() * amount_moved;
    my_transform->setPosition( my_transform->getPosition() + delta_moved );

    if (d < radius_alarm) {

 
    } else {
      in_alarm = false;
    }
	*/
  }

};

DECL_OBJ_MANAGER("ia_john", TCompIAJohn)


/*
// Move this to another .cpp
#include "components/common/comp_parent.h"
struct TCompIAMaster : public TCompBase {

  DECL_SIBLING_ACCESS()

  void update( float dt ) {
  
    // Example to send a msg to all my children.
    TCompParent* parent = get<TCompParent>();
    for( CHandle h : parent->children ) {
      TMsgAlarm msg;
      //h.sendMsg( msg );
    }
  
  }

};

DECL_OBJ_MANAGER("ia_master", TCompIAMaster)
*/