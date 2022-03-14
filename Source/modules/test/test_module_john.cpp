#include "mcv_platform.h"
#include "test_module_john.h"
#include "render/draw_primitives.h"
#include "geometry/transform.h"
#include "render/meshes/mesh_io.h"
#include "render/textures/material.h"
#include "render/shaders/pipeline_state.h"
#include "utils/data_provider.h"
#include "logic/aic_patrol.h"

#include "engine.h"
#include "render/render_module.h"
#include "entity/entity.h"
#include "entity/entity_parser.h"
#include "components/common/comp_name.h"
#include "components/common/comp_transform.h"
#include "components/common/comp_camera.h"
#include "components/messages.h"
#include "skeleton/ik_handler.h"

float curr_time = 0.0f;

//const CMesh* cube = nullptr;
//const CMesh* teapot = nullptr;
//const CTexture* texture = nullptr;
//const CMaterial* material = nullptr;

TIKHandle ik;

bool TestModuleJohn::start() {

  ik.A = VEC3(0, 0, 0);
  ik.C = VEC3(2,0,0);
  ik.AB = 2.f;
  ik.BC = 2.f;
  ik.normal = VEC3(0,1,0);

  return true;
}


void TestModuleJohn::update(float dt) {
  curr_time += 0.0001f * 60.f;

}

void TestModuleJohn::renderInMenu() {
  ImGui::DragFloat3("A", &ik.A.x, 0.01f, -5.0f, 5.0f);
  ImGui::DragFloat3("C", &ik.C.x, 0.01f, -5.0f, 5.0f);
  ImGui::DragFloat("AB", &ik.AB, 0.01f, 0.1f, 3.0f);
  ImGui::DragFloat("BC", &ik.BC, 0.01f, 0.1f, 3.0f );
}

void TestModuleJohn::renderDebug() {
  //VEC4 clr = ik.solveB() ? Colors::Green : Colors::Red;
  //drawLine(ik.A, ik.B, clr);
  //drawLine(ik.B, ik.C, clr);
  //drawText3D(ik.A, Colors::White, "A");
  //drawText3D(ik.B, Colors::White, "B");
  //drawText3D(ik.C, Colors::White, "C");
}

void TestModuleJohn::render() {

  activateObject(MAT44::Identity);

  if (ImGui::SmallButton("Explosion...")) {
    CHandle h_player = getEntityByName("player");
    TMsgExplosion msg;
    msg.damage = 10.f;
    msg.radius = 3.0f;
    h_player.sendMsg(msg);
  }

  //auto pipeline_debug = Resources.get("debug.pipeline")->as<CPipelineState>();
  //pipeline_debug->activate();aw

  //grid.activate();
  //grid.render();
  //axis.activate();
  //axis.render();

  //activateObject(aicp.e->transform.asMatrix());
  //if (aicp.getState()=="attack") aicp.e->mesh = cube; else aicp.e->mesh = teapot ;
  //aicp.e->mesh->activate();
  //aicp.e->mesh->render();


  //CTransform& e0trans = aicp.e->transform;
  //static float cone_deg = 90;
  //static float cone_length = 15.0;

  //QUAT qleft = QUAT::CreateFromAxisAngle(VEC3(0, 1, 0), deg2rad(cone_deg) * 0.5f);
  //VEC3 e0forward = e0trans.getForward() * cone_length;
  //VEC3 e0pos = e0trans.getPosition();
  //VEC3 e0cone_left = VEC3::Transform(e0forward, qleft);
  //drawLine(e0pos, e0pos + e0cone_left, VEC4(1, 1, 1, 1));
  //QUAT qright = QUAT::CreateFromAxisAngle(VEC3(0, 1, 0), deg2rad(-cone_deg) * 0.5f);
  //VEC3 e0cone_right = VEC3::Transform(e0forward, qright);
  //drawLine(e0pos, e0pos + e0cone_right, VEC4(1, 1, 1, 1));

  // Now render the objects which have textures

  //auto pipeline_objs = Resources.get("objs.pipeline")->as<CPipelineState>();
  //pipeline_objs->activate();

  //activateObject(player.transform.asMatrix());
  //material->activate();
  //player.mesh->activate();
  //player.mesh->render();

}
