//--------------------------------------------------------------------------------------
#include "common.h"

// --------------------------------------------------------------
// objs input data: AABB + WORLD. Matches struct CGPUCullingModule::TObj
struct TObj {
  float3   aabb_center;
  uint     prefab_idx;
  float3   aabb_half;
  uint     dummy2;
  float4x4 world;
};

struct TVisibleObj {
  float4x4 world;
};

struct TPrefab {
  uint id;
  int  lod_prefab;
  float lod_threshold;
  uint num_objs;
  uint num_render_type_ids;
  uint total_num_objs;
  uint render_type_ids[6];
};

// --------------------------------------------------------------
// Holds the 6 planes to perform the culling
cbuffer TCullingPlanes : register(b11) {
  float4 planes[6];
  float3 culling_camera_pos;
  float  culling_dummy;
};

// --------------------------------------------------------------
// Moved to common.h
//cbuffer TCtes : register(b10) {
//  uint  total_num_objs;
//  uint  instance_base;
//  uint2 instancing_padding;
//};

// --------------------------------------------------------------
bool isVisible( TObj obj ) {
  [unroll]
  for( int i=0; i<6; ++i ) {
    const float4 plane = planes[i];
    const float r = dot( abs( plane.xyz ), obj.aabb_half );
    const float c = dot( plane.xyz, obj.aabb_center ) + plane.w;
    if( c < -r )
      return false;
  }
  return true;
}

// --------------------------------------------------------------
[numthreads(64, 1, 1)]
void cs_cull_instances(
  uint id : SV_DispatchThreadID     // Unique id as uint
,   StructuredBuffer<TObj>            objs         : register(t0)
,   StructuredBuffer<TPrefab>         prefabs      : register(t1)
, RWStructuredBuffer<TVisibleObj>     visible_objs : register(u0)
, RWByteAddressBuffer                 draw_datas   : register(u1)
) 
{

  // Process only the num objects set by the CPU
  if( id >= total_num_objs )
    return;

  TObj obj = objs[ id ];
  
  if( !isVisible( obj ) )
    return;

  // Each prefab defines which draw call types must be added
  TPrefab prefab = prefabs[ obj.prefab_idx ];

  // LOD -> Change prefab type based on distance
  float  distance_to_camera = length( culling_camera_pos - obj.aabb_center );
  if( distance_to_camera > prefab.lod_threshold ) {
    // The model is no longer visible...
    if( prefab.lod_prefab < 0 )
      return;
    // Replace the prefab for the lod prefab
    prefab = prefabs[prefab.lod_prefab];

    // Check if the lod can be discarted doing a second check
    if (distance_to_camera > prefab.lod_threshold) {
      // The model is no longer visible...
      if (prefab.lod_prefab < 0)
        return;
    }
  }

  const uint bytes_per_draw_data_id = 32;

  for( uint i=0; i<prefab.num_render_type_ids; ++i ) {
    uint render_type_id = prefab.render_type_ids[i];
    uint offset_render_type = render_type_id * bytes_per_draw_data_id;
    uint base = draw_datas.Load( offset_render_type + 20 );  // Read TDrawData[render_type_id].base

    // Atomically read+add(+1)+write. Result is recv in myIndex
    uint myIndex;
    draw_datas.InterlockedAdd( offset_render_type + 4, 1, myIndex );   // +1 to .instanceCount

    // Store the obj in the requested position
    visible_objs[ base + myIndex ].world = obj.world;
  }

}

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
//V2F_GBuffer VS(
//  in VtxPosNUvT input
//, in uint InstanceID : SV_InstanceID
//, StructuredBuffer<TVisibleObj> visible_objs : register(t0)
//)
//{
//  TVisibleObj obj = visible_objs[ instance_base + InstanceID ];
//  float4x4 newWorld = obj.world;
//  return VS_GBuffer_Common( input, newWorld );
//}
