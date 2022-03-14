//--------------------------------------------------------------------------------------
#include "common.h"

//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR0;        // Not used
    float2 Uv : TEXCOORD0;
    float3 WorldPos : TEXCOORD1;
    float3 WorldNormal : NORMAL;
    float4 WorldTan : TEXCOORD2;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS_Base(VtxNormalUvTan input, matrix World )
{
  VS_OUTPUT output;
  float4 world_pos =  mul( float4(input.Pos.xyz, 1), World);
  output.WorldPos = world_pos.xyz;
  output.WorldNormal = mul( input.Normal, (float3x3)World);
  output.Pos = mul( world_pos, camera_view_projection);
  output.Color = float4((input.Normal + 1)*0.5,1);
  //output.Color = float4(input.Normal,1);
  output.Uv = input.Uv;
  output.WorldTan.xyz = mul( input.Tan.xyz, (float3x3)World);
  output.WorldTan.w = input.Tan.w;
  return output;
} 

VS_OUTPUT VS(VtxNormalUvTan input)
{
  return VS_Base(input, object_world);
}

VS_OUTPUT VS_Skin(VtxNormalUvTan input, VtxSkinInfo skin)
{
  matrix skinMatrix = getSkinMtx(skin);
  return VS_Base(input, skinMatrix);
}

// -----------------------------------------------------
VS_OUTPUT VS_Instanced(VtxNormalUvTan input, uint InstanceID : SV_InstanceID ) {
  return VS_Base(input, getInstanceWorld(InstanceID));
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
void PS_Common( 
  in VS_OUTPUT input 
, float4 albedo_color
, out float4 o_albedo : SV_Target0
, out float4 o_normal : SV_Target1
, out float  o_depth  : SV_Target2    // Linear depth
  )
{

  // Compute Final Normal -----------------------------------
  float4 normal_color = txNormal.Sample(wrapLinear, input.Uv);

  // Generate TBN space and compute normal map
  float3 normal_tangent_space = decodeNormal( normal_color.xyz );
  // normal_tangent_space.z *= 0.7;   // Increase the effect of the normal map
  // normal_tangent_space.z *= 7.0;   // Decrease the effect of the normal map
  float3 wN = input.WorldNormal;
  float3 wT = input.WorldTan.xyz;
  float3 wB = cross(wN,wT) * input.WorldTan.w;
  // This matrix transforms from tangent space to world space
  float3x3 TBN = float3x3( wT, -wB, wN );
  float3 N = normalize(mul( normal_tangent_space, TBN ));

  // ---------------------------
  float3 cam2world_pos = input.WorldPos - camera_position;
  float  forward_units = dot( cam2world_pos, camera_forward );

  // Compute Final Normal -----------------------------------
  float  metallic_color = txMetallic.Sample(wrapLinear, input.Uv).x;
  float  roughness_color = txRoughness.Sample(wrapLinear, input.Uv).x;

  // Encode gbuffer outputs -----------------------------------

  // Store albedo.rgb & metallic in the texture
  o_albedo = float4( albedo_color.xyz, metallic_color ) * object_color;

  // Save roughness in the alpha channel of the N render target
  o_normal = float4( encodeNormal(N), roughness_color );
  o_depth = forward_units;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
void PS(
  in VS_OUTPUT input
  , out float4 o_albedo : SV_Target0
  , out float4 o_normal : SV_Target1
  , out float  o_depth : SV_Target2    // Linear depth
)
{
  float4 albedo_color = txAlbedo.Sample(wrapLinear, input.Uv);
  PS_Common( input, albedo_color, o_albedo, o_normal, o_depth );
}

//--------------------------------------------------------------------------------------
#include "video.h"
void PS_Video(
  in VS_OUTPUT input
  , out float4 o_albedo : SV_Target0
  , out float4 o_normal : SV_Target1
  , out float  o_depth : SV_Target2    // Linear depth
)
{
  float4 albedo_color = sampleVideoTexture( txAlbedo, input.Uv );
  PS_Common(input, albedo_color, o_albedo, o_normal, o_depth);
}
