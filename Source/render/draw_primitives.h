#pragma once

// Create vertex buffer
struct VtxPosColor {
  VEC3 p; 
  VEC4 color;
  VtxPosColor() = default;
  VtxPosColor(float px, float py, float pz, VEC4 new_color)
    : p(px, py, pz), color(new_color) {}
  VtxPosColor(VEC3 new_p, VEC4 new_color) : p(new_p), color(new_color) {}
};

struct VtxPosNUvT {
  VEC3 pos;
  VEC3 normal;
  VEC2 uv;
  VEC3 tangent;
  float tangent_w;
};

struct VtxPosUv {
  VEC3 pos;
  VEC2 uv;
  VtxPosUv() = default;
  VtxPosUv(float px, float py, float pz, VEC2 new_uv)
    : pos(px, py, pz), uv(new_uv) {
  }
};


bool createAndRegisterPrimitives();

class CCamera;
void activateCamera(const CCamera& camera);
void activateObject(const MAT44& world, VEC4 color = Colors::White);

void drawLine(VEC3 src, VEC3 dst, VEC4 color);
void drawAxis(const MAT44& transform);
void drawWiredAABB(AABB aabb, const MAT44 world, VEC4 color = Colors::White);
void drawWiredSphere(const MAT44 world, float radius, VEC4 color = Colors::White);
void drawCircleXZ(const MAT44 user_world, float radius, VEC4 color);
void drawText3D(const VEC3& world_coord, VEC4 color, const char* text);
void drawPrimitive(const CMesh* mesh, MAT44 world, VEC4 color = Colors::White);
void drawFullScreenQuad(const std::string& pipeline_name, const CTexture* extra_texture);

const CCamera& getActiveCamera();
