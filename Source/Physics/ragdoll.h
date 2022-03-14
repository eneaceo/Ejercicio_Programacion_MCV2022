#ifndef INC_RAGDOLL_H_
#define INC_RAGDOLL_H_

#include "PxPhysicsAPI.h"
struct TRagdollCore;
struct TRagdollBoneCore;

struct TRagdoll {
  struct TRagdollBone {
    TRagdollBoneCore* core = nullptr;

    physx::PxRigidDynamic* actor = nullptr;
    physx::PxJoint* parent_joint = nullptr;

    static const int MAX_RAGDOLL_BONE_CHILDREN = 128;
    int children_idxs[MAX_RAGDOLL_BONE_CHILDREN];

    int num_children = 0;
    int idx = 0;
    int parent_idx = 0;
  };

  static const int MAX_RAGDOLL_BONES = 128;
  int num_bones = 0;
  TRagdollBone bones[MAX_RAGDOLL_BONES];
  TRagdollCore* core = nullptr;
  bool created = false;
};


#endif
