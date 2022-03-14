#include "compute_particles_common.h"


// --------------------------------------------------------------
// For the instanced solid balls rendered using the pbr tech
/*
V2F_GBuffer VS( 
  VtxPosNUvT input        // Sphere
, in uint InstanceID : SV_InstanceID
, StructuredBuffer<TParticle> instances_active : register(t0)
  ) {

  TParticle inst = instances_active[ InstanceID ];
  input.Pos.xyz *= inst.scale;
  input.Pos.xyz += inst.pos;
  return VS_GBuffer_Common( input, ObjWorld );
}
*/

// --------------------------------------------------------------
struct V2F_Billboard {
  float4 Pos : SV_POSITION;
  float4 Color : COLOR;
  float2 Uv : TEXCOORD0;
};

// input is the billboard. This is a placeholder, 
V2F_Billboard VS( VtxUv input ) {
  V2F_Billboard output = (V2F_Billboard)0;
  return output;
}

// input is the billboard
V2F_Billboard VS_Instanced(
  VtxUv input       // billboard
, in uint InstanceID : SV_InstanceID
, StructuredBuffer<TParticle> instances_active : register(t0)
) {
  V2F_Billboard output = (V2F_Billboard)0;
  output.Uv = input.Uv;

  TParticle inst = instances_active[InstanceID];

  output.Color = inst.color;

  // pos to render in world coords is the local position in the quad xy (input.Pos)
  // moved to the instance position (inst.pos)

  // input.Pos.x goes from -1 ..1, the same for y
  float3 localPos = -input.Pos.x * camera_right
                  + input.Pos.y * camera_up
                  ;

  float3 entitySpacePos = localPos * inst.scale + inst.pos;

  float4 worldPos = mul( float4(entitySpacePos,1), object_world );
  output.Pos = mul( worldPos, camera_view_projection);

  return output;
}

float4 PS(V2F_Billboard input) : SV_Target { 
  float4 albedo_color = txAlbedo.Sample(wrapLinear, input.Uv);
  //input.Color.xyz *= 10.0;
  //input.Color *= input.Color;
  //input.Color *= input.Color;
  float4 final_color = albedo_color * input.Color;
  return final_color;
}

