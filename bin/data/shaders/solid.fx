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
    float4 Color : COLOR0;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(VtxColor input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.Pos = mul( input.Pos, object_world );
    output.Pos = mul( output.Pos, camera_view_projection);
    output.Color = input.Color * object_color;
    return output;
}

VS_OUTPUT VS_NoColor(Vtx input)
{
  VS_OUTPUT output = (VS_OUTPUT)0;
  output.Pos = mul(input.Pos, object_world);
  output.Pos = mul(output.Pos, camera_view_projection);
  output.Color = object_color;
  return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( VS_OUTPUT input ) : SV_Target
{
    return input.Color;
}
