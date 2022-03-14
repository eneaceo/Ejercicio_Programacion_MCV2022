#include "mcv_platform.h"
#include "draw_primitives.h"

extern CShaderCte<CtesCamera> cte_camera;
extern CShaderCte<CtesObject> cte_object;

static CMesh* line = nullptr;
static CMesh* axis = nullptr;
static CMesh* unit_wired_cube = nullptr;
static CMesh* unit_wired_circle_xz = nullptr;

CCamera the_active_camera;

static void createAxis(CMesh& mesh) {
  VtxPosColor vertices[6] = {
    { VEC3(0,0,0), Colors::Red },
    { VEC3(1,0,0), Colors::Red },
    { VEC3(0,0,0), Colors::Green },
    { VEC3(0,2,0), Colors::Green },
    { VEC3(0,0,0), Colors::Blue },
    { VEC3(0,0,3), Colors::Blue },
  };
  mesh.create(
    vertices, sizeof(vertices), sizeof(VtxPosColor),
    nullptr, 0, 0,
    D3D11_PRIMITIVE_TOPOLOGY_LINELIST
  );
}

static void createGrid(CMesh& mesh, int npoints) {
  std::vector<VtxPosColor> vertices;

  VEC4 clrA(0.5f, 0.5f, 0.5f, 1.0f);
  VEC4 clrB(0.25f, 0.25f, 0.25f, 1.0f);
  float maxi = (float)npoints;
  for (int i = -npoints; i <= npoints; ++i) {
    VEC4 clr = (i % 5) == 0 ? clrA : clrB;
    vertices.emplace_back(VEC3(-maxi, 0, (float)i), clr);
    vertices.emplace_back(VEC3(maxi, 0, (float)i), clr);
    vertices.emplace_back(VEC3((float)i, 0, -maxi), clr);
    vertices.emplace_back(VEC3((float)i, 0, maxi), clr);
  }

  mesh.create(
    vertices.data(), vertices.size() * sizeof(VtxPosColor), sizeof(VtxPosColor),
    nullptr, 0, 0,
    D3D11_PRIMITIVE_TOPOLOGY_LINELIST
  );
}

static void createLine(CMesh& mesh) {
  VtxPosColor vertices[2];
  vertices[0] = VtxPosColor(VEC3(0, 0, 0), Colors::White);
  vertices[1] = VtxPosColor(VEC3(0, 0, 1), Colors::White);
  mesh.create(
    vertices, 2 * sizeof(VtxPosColor), sizeof(VtxPosColor),
    nullptr, 0, 0,
    D3D11_PRIMITIVE_TOPOLOGY_LINELIST
  );
}

static void createUnitWiredCube(CMesh& mesh) {
  std::vector<VtxPosColor> vtxs =
  {
    VtxPosColor(-0.5f,-0.5f, -0.5f,  Colors::White),    // 
    VtxPosColor( 0.5f,-0.5f, -0.5f,  Colors::White),
    VtxPosColor(-0.5f, 0.5f, -0.5f,  Colors::White),
    VtxPosColor( 0.5f, 0.5f, -0.5f,  Colors::White),    // 
    VtxPosColor(-0.5f,-0.5f, 0.5f,   Colors::White),    // 
    VtxPosColor( 0.5f,-0.5f, 0.5f,   Colors::White),
    VtxPosColor(-0.5f, 0.5f, 0.5f,   Colors::White),
    VtxPosColor( 0.5f, 0.5f, 0.5f,   Colors::White),    // 
  };
  const std::vector<uint16_t> idxs = {
      0, 1, 2, 3, 4, 5, 6, 7
    , 0, 2, 1, 3, 4, 6, 5, 7
    , 0, 4, 1, 5, 2, 6, 3, 7
  };
  const int nindices = 8 * 3;
  mesh.create(
    vtxs.data(), (uint32_t)vtxs.size() * sizeof(VtxPosColor), sizeof(VtxPosColor),
    idxs.data(), nindices * sizeof(uint16_t), sizeof(uint16_t),
    D3D11_PRIMITIVE_TOPOLOGY_LINELIST
  );
}

static void createUnitWiredCircleXZ(CMesh& mesh)
{
  const int samples = 32;
  std::vector<uint16_t> idxs;
  std::vector< VtxPosColor > vtxs;
  vtxs.reserve(samples);
  for (int i = 0; i <= samples; ++i)
  {
    float angle = 2 * (float)M_PI * (float)i / (float)samples;
    VtxPosColor v1;
    v1.p = yawToVector(angle);
    v1.color = Colors::White;
    vtxs.push_back(v1);
    idxs.push_back(i);
  }
  mesh.create(
    vtxs.data(), (uint32_t)vtxs.size() * sizeof(VtxPosColor), sizeof(VtxPosColor),
    idxs.data(), idxs.size() * sizeof(uint16_t), sizeof(uint16_t),
    D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP
  );
}

static void createViewVolumeWired(CMesh& mesh) {
  std::vector<VtxPosColor> vtxs =
  {
    VtxPosColor(-1.0f,-1.0f, 0.0f,  Colors::White),    // 
    VtxPosColor(1.0f,-1.0f, 0.0f,  Colors::White),
    VtxPosColor(-1.0f, 1.0f, 0.0f,  Colors::White),
    VtxPosColor(1.0f, 1.0f, 0.0f,  Colors::White),    // 
    VtxPosColor(-1.0f,-1.0f, 1.0f,  Colors::White),    // 
    VtxPosColor(1.0f,-1.0f, 1.0f,  Colors::White),
    VtxPosColor(-1.0f, 1.0f, 1.0f,  Colors::White),
    VtxPosColor(1.0f, 1.0f, 1.0f,  Colors::White),    // 
  };
  const std::vector<uint16_t> idxs = {
      0, 1, 2, 3, 4, 5, 6, 7
    , 0, 2, 1, 3, 4, 6, 5, 7
    , 0, 4, 1, 5, 2, 6, 3, 7
  };
  const int nindices = 8 * 3;
  mesh.create(
    vtxs.data(), (uint32_t)vtxs.size() * sizeof(VtxPosColor), sizeof(VtxPosColor),
    idxs.data(), nindices * sizeof(uint16_t), sizeof(uint16_t),
    D3D11_PRIMITIVE_TOPOLOGY_LINELIST
  );

}

// ------------------------------------------------
static void createUnitQuadXY(CMesh& mesh) {
  VtxPosUv vtxs[4];
  vtxs[0] = VtxPosUv(0, 0, 0, VEC2(0, 0));
  vtxs[1] = VtxPosUv(1, 0, 0, VEC2(1, 0));
  vtxs[2] = VtxPosUv(1, 1, 0, VEC2(1, 1));
  vtxs[3] = VtxPosUv(0, 1, 0, VEC2(0, 1));

  // Create some idxs 0,1,2,3...
  const int nindices = 6;
  uint16_t idxs[nindices] = {
    0, 1, 2,
    2, 3, 0,
  };

  mesh.create(
    vtxs, 4 * sizeof(VtxPosUv), sizeof(VtxPosUv),
    idxs, nindices * sizeof(uint16_t), sizeof(uint16_t),
    D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
    getVertexDeclarationByName("PosUv")
  );
}



bool createAndRegisterPrimitives() {
  PROFILE_FUNCTION("createAndRegisterPrimitives");
  {
    CMesh* mesh = new CMesh;
    createAxis(*mesh);
    Resources.registerResource(mesh, "axis.mesh", getClassResourceType<CMesh>());
    axis = mesh;
  }
  {
    CMesh* mesh = new CMesh;
    createGrid(*mesh, 10);
    Resources.registerResource(mesh, "grid.mesh", getClassResourceType<CMesh>());
  }
  {
    CMesh* mesh = new CMesh;
    createLine(*mesh);
    Resources.registerResource(mesh, "line.mesh", getClassResourceType<CMesh>());
    line = mesh;
  }
  {
    CMesh* mesh = new CMesh;
    createUnitWiredCube(*mesh);
    Resources.registerResource(mesh, "unit_wired_cube.mesh", getClassResourceType<CMesh>());
    unit_wired_cube = mesh;
  }
  {
    CMesh* mesh = new CMesh;
    createUnitWiredCircleXZ(*mesh);
    Resources.registerResource(mesh, "unit_wired_circle_xz.mesh", getClassResourceType<CMesh>());
    unit_wired_circle_xz = mesh;
  }
  {
    CMesh* mesh = new CMesh;
    createViewVolumeWired(*mesh);
    Resources.registerResource(mesh, "view_volume_wired.mesh", getClassResourceType<CMesh>());
  }
  {
    CMesh* mesh = new CMesh;
    createUnitQuadXY(*mesh);
    Resources.registerResource(mesh, "unit_quad_xy.mesh", getClassResourceType<CMesh>());
  }
  return true;
}

// Gets all the information from the camera obj and uploads it to the GPU cte buffer 
// associated to the camera
void activateCamera(const CCamera& camera) {
  the_active_camera = camera;
  cte_camera.camera_view = camera.getView();
  cte_camera.camera_projection = camera.getProjection();
  cte_camera.camera_view_projection = camera.getViewProjection();
  cte_camera.camera_inverse_view_projection = cte_camera.camera_view_projection.Invert();
  cte_camera.camera_forward = camera.getForward();
  cte_camera.camera_zfar = camera.getFar();
  cte_camera.camera_position = camera.getEye();
  cte_camera.camera_tan_half_fov = tanf( camera.getFov() * 0.5f );
  cte_camera.camera_aspect_ratio = camera.getAspectRatio();
  cte_camera.camera_dummy1 = 0.0f;
  cte_camera.camera_right = camera.getRight();
  cte_camera.camera_up = camera.getUp();
  cte_camera.updateFromCPU();
}

const CCamera& getActiveCamera() {
  return the_active_camera;
}

void activateObject(const MAT44& world, VEC4 color) {
  cte_object.object_world = world;
  cte_object.object_color = color;
  cte_object.updateFromCPU();
}

void drawLine(VEC3 src, VEC3 dst, VEC4 color) {
  VEC3 delta = dst - src;
  float d = delta.Length();
  if (d < 1e-3f)
    return;
  if (fabsf(delta.x) < 1e-5f && fabsf(delta.z) < 1e-5f) 
    dst.x += 1e-3f;
  MAT44 world = MAT44::CreateScale(1, 1, -d) * MAT44::CreateLookAt(src, dst, VEC3(0, 1, 0)).Invert();
  activateObject(world, color);
  line->activate();
  line->render();
}

void drawAxis(const MAT44& transform) {
  activateObject(transform, Colors::White);
  axis->activate();
  axis->render();
}

void drawWiredAABB(AABB aabb, const MAT44 world, VEC4 color) {
  MAT44 unit_cube_to_world = 
    MAT44::CreateScale( VEC3(aabb.Extents) * 2.0f ) 
    * MAT44::CreateTranslation( aabb.Center )
    * world;
  drawPrimitive(unit_wired_cube, unit_cube_to_world, color);
}

void drawCircleXZ(const MAT44 user_world, float radius, VEC4 color) {
  MAT44 world = MAT44::CreateScale(radius) * user_world;
  const CMesh* mesh = unit_wired_circle_xz;
  // Draw 3 circleXZ 
  drawPrimitive(mesh, world, color);
}

void drawWiredSphere(const MAT44 user_world, float radius, VEC4 color) {
  MAT44 world = MAT44::CreateScale(radius) * user_world;
  const CMesh* mesh = unit_wired_circle_xz;
  // Draw 3 circleXZ 
  drawPrimitive(mesh, world, color);
  drawPrimitive(mesh, MAT44::CreateRotationX((float)M_PI_2) * world, color);
  drawPrimitive(mesh, MAT44::CreateRotationZ((float)M_PI_2) * world, color);
}

void drawPrimitive(const CMesh* mesh, MAT44 world, VEC4 color) {
  assert(mesh);
  auto vdecl = mesh->getVertexDecl();
  if (vdecl->name == "Pos") {
    const CPipelineState* pipe = Resources.get("debug_white.pipeline")->as<CPipelineState>();
    pipe->activate();
  }
  else if(vdecl->name != "PosClr")
  {
    const CPipelineState* pipe = Resources.get("debug_white.pipeline")->as<CPipelineState>();
    pipe->activate();
  }
  activateObject(world, color);
  mesh->activate();
  mesh->render();
}

void drawText3D(const VEC3& world_coord, VEC4 color, const char* text ) {
  const CCamera& camera = getActiveCamera();
  const MAT44& view_proj = camera.getViewProjection();

  // Because it divides by w
  // It's in the range -1..1
  VEC3 homo_pos = VEC3::Transform(world_coord, view_proj);

  if (homo_pos.z <= 0.0f || homo_pos.z >= 1.0f )
    return;

  float render_width = (float)Render.getWidth();
  float render_height = (float)Render.getHeight();

  // Convert -1..1 to 0..1024
  float sx = (homo_pos.x + 1.f) * 0.5f * render_width;
  float sy = (1.0f - homo_pos.y) * 0.5f * render_height;

  ImVec2 sz = ImGui::CalcTextSize(text);
  int margin = 4;

  auto* dl = ImGui::GetBackgroundDrawList();
  // center
  sx -= sz.x / 2 - margin;
  sy -= sz.y / 2 - margin;
  dl->AddRectFilled(ImVec2(sx - margin, sy - margin), ImVec2(sx + sz.x + margin, sy + sz.y + margin), 0xa0000000, 2.f);
  dl->AddText(ImVec2(sx, sy), ImGui::ColorConvertFloat4ToU32(*(ImVec4*)&color.x), text);
}


void drawFullScreenQuad(const std::string& pipeline_name, const CTexture* extra_texture) {
  PROFILE_FUNCTION("FullQUad");
  CGpuScope gpu_scope(pipeline_name.c_str());
  // from name to CPipelineState
  auto* pipeline = Resources.get(pipeline_name)->as<CPipelineState>();
  assert(pipeline);
  pipeline->activate();
  if (extra_texture)
    extra_texture->activate(TS_ALBEDO);
  auto* mesh = Resources.get("unit_quad_xy.mesh")->as<CMesh>();
  mesh->activate();
  mesh->render();
}
