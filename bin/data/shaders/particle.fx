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
    float2 Uv : TEXCOORD0;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(VtxUv input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.Pos = input.Pos - float4(0.5f, 0.5f, 0.f, 0.f);
    output.Pos = mul( output.Pos, object_world );
    output.Pos = mul( output.Pos, camera_view_projection);
    output.Uv = lerp( particle_min_uv, particle_max_uv, input.Uv);
    return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( VS_OUTPUT input ) : SV_Target
{
    float4 texture_color = txAlbedo.Sample(wrapLinear, input.Uv);
    return texture_color * object_color;
}
