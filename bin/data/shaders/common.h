#include "constants.h"

// Samplers in the enum order
SamplerState wrapLinear : register(s0);
SamplerState borderLinear : register(s1);
SamplerComparisonState borderPCF : register(s2);
SamplerState clampLinear : register(s3);

// Textures by semantic
Texture2D txAlbedo         TEXTURE_SLOT(TS_ALBEDO);
Texture2D txNormal         TEXTURE_SLOT(TS_NORMAL);
Texture2D txMetallic       TEXTURE_SLOT(TS_METALLIC);
Texture2D txRoughness      TEXTURE_SLOT(TS_ROUGHNESS);
Texture2D txEmissive       TEXTURE_SLOT(TS_EMISSIVE);

Texture2D txLightPattern   TEXTURE_SLOT(TS_LIGHT_PATTERN);
Texture2D txLightShadowMap TEXTURE_SLOT(TS_LIGHT_SHADOW_MAP);

Texture2D txDeferredAlbedos      TEXTURE_SLOT(TS_DEFERRED_ALBEDOS);
Texture2D txDeferredNormals      TEXTURE_SLOT(TS_DEFERRED_NORMALS);
Texture2D txDeferredLinearDepths TEXTURE_SLOT(TS_DEFERRED_LINEAR_DEPTH);
Texture2D txDeferredOutput       TEXTURE_SLOT(TS_DEFERRED_OUTPUT);
Texture2D txScreenSpaceAO        TEXTURE_SLOT(TS_DEFERRED_AO);

TextureCube txEnvironment    TEXTURE_SLOT(TS_ENVIRONMENT);
TextureCube txIrradiance     TEXTURE_SLOT(TS_IRRADIANCE);
Texture2D   txNoise          TEXTURE_SLOT(TS_NOISE);
Texture3D   txColorGrading   TEXTURE_SLOT(TS_LUT_COLOR_GRADING);

// Vertex types
struct VtxColor {
  float4 Pos : POSITION;
  float4 Color : COLOR;
};

struct Vtx {
  float4 Pos : POSITION;
};

struct VtxUv {
  float4 Pos : POSITION;
  float2 Uv : TEXCOORD0;
};

struct VtxNormalUvTan
{
  float4 Pos : POSITION;
  float3 Normal : NORMAL;
  float2 Uv  : TEXCOORD0;
  float4 Tan : TEXCOORD1;
};

struct VtxSkinInfo
{
  uint4  BoneIDs     : BONEIDS;
  float4 BoneWeights : WEIGHTS;
};

struct VtxFullScreen {
  float4 Pos : SV_POSITION;
  float2 Uv : TEXCOORD0;
  float3 WorldDir : TEXCOORD1;
  float3 ViewDir : TEXCOORD2;
};

// Combine the bones by the vertex weights
matrix getSkinMtx(VtxSkinInfo skin)
{
  return 
      bones[skin.BoneIDs[0]] * skin.BoneWeights[0]
    + bones[skin.BoneIDs[1]] * skin.BoneWeights[1]
    + bones[skin.BoneIDs[2]] * skin.BoneWeights[2]
    + bones[skin.BoneIDs[3]] * skin.BoneWeights[3]
    ;
}

float2 hash2(float n) { return frac(sin(float2(n, n + 1.0))*float2(43758.5453123, 22578.1459123)); }

float rand( float x ) { return frac( sin(x) * 43758.5453 ); }

float4 sampleLightPattern( float3 WorldPos ) {
  float4 proj_coords = mul( float4(WorldPos,1), light_view_projection_offset);
  float3 homo_coords_offset = proj_coords.xyz / proj_coords.w;
  float4 light_pattern_color = txLightPattern.Sample(borderLinear, homo_coords_offset.xy);
  if( proj_coords.w < 0 ) light_pattern_color = 0;
  // proj_coords.w is the distance in view space of the light
  float distance_att_factor = 1 - saturate( proj_coords.w / light_max_radius );
  
  // Change the linear effect of the distance
  // distance_att_factor = pow( saturate( distance_att_factor ), 0.5 );

  return light_pattern_color * distance_att_factor;
}

float tapShadow(float2 xy, float z) {
  return txLightShadowMap.SampleCmp(borderPCF, xy, z).x;
}

float2 vogelDiskSample(int sampleIndex, int samplesCount, float phi) {
  float GoldenAngle = 2.4f;

  float r = sqrt((sampleIndex + 0.5f) / (float)samplesCount);
  float theta = sampleIndex * GoldenAngle + phi;

  float sine = sin(theta);
  float cosine = cos(theta);

  return float2(r * cosine, r * sine);
}

float getShadowFactor(float3 WorldPos) {

  // homo coords => in the range 0..1
  float4 proj_coords = mul(float4(WorldPos, 1), light_view_projection_offset);
  float3 homo_coords_offset = proj_coords.xyz / proj_coords.w;
  homo_coords_offset.z -= light_shadows_bias;

  float random_angle = rand(WorldPos.x + 3.14 * WorldPos.z) * 3.1415;

  float scale_factor = light_shadows_step_over_resolution;
  float shadow_factor = 0;

  const int nsamples = 10;
  [unroll]
  for (int i = 0; i < nsamples; ++i) {
    float2 delta_rotated = vogelDiskSample(i, nsamples, random_angle) * scale_factor;
    shadow_factor += tapShadow(homo_coords_offset.xy + delta_rotated, homo_coords_offset.z);
  }

  return shadow_factor / nsamples;
}

//----------------------------------------------------------
// Returns normal in the -1..1 range
float3 decodeNormal( float3 n_between_0_1 ) {
  return n_between_0_1 * 2. - 1.;
}

float3 encodeNormal( float3 n ) {
  return ( n + 1. ) * 0.5;
}




float worldUnits( float3 worldPos ) {
  int3 iWorld = (int3)worldPos.xyz;
  float dx = abs( worldPos.x - (int)iWorld.x );
  float dy = abs( worldPos.y - (int)iWorld.y );
  float dz = abs( worldPos.z - (int)iWorld.z );
  return dx * dz;
}


//--------------------------------------------------------------------------------------
// Vertex Shader for drawFullScreenQuad method
//--------------------------------------------------------------------------------------
VtxFullScreen VS_FullScreen(VtxUv input) {
  VtxFullScreen output;
  // There is no need for camera
  // We output the coordinates in full screen 
  // knowing the input is a fullscreen quad in the 0..1 range
  output.Pos = float4(
    input.Pos.x * 2 - 1
    , 1 - input.Pos.y * 2
    , 1
    , 1);
  output.Uv = input.Uv;

  float3 viewDir = float3(input.Uv, 1);
  viewDir.y = viewDir.y * 2.0 - 1.0;
  viewDir.x = viewDir.x * 2.0 - 1.0;
  viewDir.y *= camera_tan_half_fov;
  viewDir.x *= camera_tan_half_fov * camera_aspect_ratio;
  output.ViewDir = viewDir;

  output.WorldDir = camera_forward * viewDir.z
    + camera_right * viewDir.x
    - camera_up * viewDir.y;

  return output;
}

//--------------------------------------------------------------------------------------
struct TWorldInstance
{
  float4x4 world;
};

// --------------------------------------------------------------
DECL_SHADER_CTE(TCtes, CB_SLOT_INSTANCING)
{
  uint  total_num_objs;
  uint  instance_base;
  uint2 instancing_padding;
};

StructuredBuffer<TWorldInstance> world_instances : register(t0);   // Buffer with the instances  // Uav
float4x4 getInstanceWorld(uint InstanceID)
{
  return world_instances[instance_base + InstanceID].world;
}

