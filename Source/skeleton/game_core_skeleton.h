#pragma once

#include "resources/resources.h"
#include "bone_correction.h"
#include "Cal3d/cal3d.h"
#include "physics/ragdoll_core.h"

// ---------------------------------------------------------------------------------------
// Cal2DX conversions, VEC3 are the same, QUAT must change the sign of w
class CGameCoreSkeleton : public IResource, public CalCoreModel {
  std::string root_path;

  void convertCalMeshToEngineMesh(const std::string& mesh_name, int mesh_id);

public:

  float              bone_ids_debug_scale = 1.f;
  std::vector< int > bone_ids_to_debug;
  std::vector< int > bone_ids_for_aabb;
  VEC3               aabb_extra_factor = VEC3::One;
  std::vector< TBoneCorrection > lookat_corrections;

  CGameCoreSkeleton(const std::string& name) : CalCoreModel( name ) {}

  bool create(const std::string& res_name);
  bool renderInMenu() const override;

  TRagdollCore ragdoll_core;
};


