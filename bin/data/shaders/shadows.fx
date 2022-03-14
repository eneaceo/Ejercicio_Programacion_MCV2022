//--------------------------------------------------------------------------------------
// File: Tutorial04.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include "common.h"

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
float4 VS(float4 Pos : POSITION) : SV_POSITION 
{
  float4 output = mul( Pos, object_world );
  output = mul( output, camera_view_projection);
  return output;
}

float4 VS_Skin(float4 Pos : POSITION, VtxSkinInfo skin) : SV_POSITION
{
  matrix skinMatrix = getSkinMtx(skin);
  float4 output = mul(Pos, skinMatrix);
  output = mul(output, camera_view_projection);
  return output;
}

float4 VS_Instanced(
  float4 Pos : POSITION
, uint InstanceID : SV_InstanceID
) : SV_POSITION
{
  float4 output = mul(Pos, getInstanceWorld(InstanceID));
  output = mul(output, camera_view_projection);
  return output;
}