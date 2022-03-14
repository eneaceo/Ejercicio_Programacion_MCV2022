//--------------------------------------------------------------------------------------
#include "common.h"

//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR0;        // Not used
    float2 Uv : TEXCOORD0;
    float3 WorldPos : TEXCOORD1;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS_Base(VtxNormalUvTan input, matrix World )
{
  VS_OUTPUT output;
  float4 world_pos =  mul( float4(input.Pos.xyz, 1), World);
  output.WorldPos = world_pos.xyz;
  output.Pos = mul( world_pos, camera_view_projection);
  output.Color = object_color;
  output.Uv = input.Uv;
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

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( in VS_OUTPUT input ) : SV_Target
{

  // Compute Final Normal -----------------------------------
  float4 emissive_color = txEmissive.Sample(wrapLinear, input.Uv);
  //emissive_color = 1.5 * pow( emissive_color, 2 );
  return emissive_color;
}
