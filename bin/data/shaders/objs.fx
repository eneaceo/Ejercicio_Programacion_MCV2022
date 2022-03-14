//--------------------------------------------------------------------------------------
// File: Tutorial04.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
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

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_Target
{
  // Take a sample of the texture txalbedo (in slot 0), using the 
  // configuration defined by samLinear, at the texture coords input.uV
  float4 texture_color = txAlbedo.Sample(wrapLinear, input.Uv);
  float4 normal_color = txNormal.Sample(wrapLinear, input.Uv);

  // Generate TBN space and compute normal map
  float3 normal_tangent_space = decodeNormal( normal_color );
  // normal_tangent_space.z *= 0.7;   // Increase the effect of the normal map
  // normal_tangent_space.z *= 7.0;   // Decrease the effect of the normal map
  float3 wN = input.WorldNormal;
  float3 wT = input.WorldTan.xyz;
  float3 wB = cross(wN,wT) * input.WorldTan.w;
  // This matrix transforms from tangent space to world space
  float3x3 TBN = float3x3( wT, -wB, wN );
  float3 N = normalize(mul( normal_tangent_space, TBN ));

  // 
  float4 light_pattern_color = sampleLightPattern( input.WorldPos );

  float shadow_factor = getShadowFactor( input.WorldPos );

  float3 L = -light_forward;
  float NdotL = saturate(dot(L, N));
  return NdotL * texture_color * light_color * light_intensity * shadow_factor;

  //return float4(input.Uv.x, input.Uv.y, 0, 1);
  return float4(input.Color);
}

