//--------------------------------------------------------------------------------------
#include "pbr_funcs.h"

//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(Vtx input)
{
    VS_OUTPUT output;
    float4 world_pos =  mul( float4(input.Pos.xyz, 1), object_world);
    output.Pos = mul( world_pos, camera_view_projection);
    return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS_Light_Common( VS_OUTPUT input, bool is_light_spot, bool enable_shadows)
{

  // Get access to the gbuffer
  GBuffer g;
  decodeGBuffer( input.Pos.xy, g );

  // Compute light
  float shadow_factor = 1.0;
  if( is_light_spot ) {

    if( enable_shadows ) {

      shadow_factor = getShadowFactor( g.pos );

    } else {
      // Convert to range -1..1
      float4 proj_coords = mul(float4(g.pos, 1), light_view_projection);
      proj_coords.xyz = abs( proj_coords.xyz ) / proj_coords.w;
      if( proj_coords.x > 1 || proj_coords.y > 1)
        return 0;
    }
  }

  float3 world_to_light = light_position - g.pos;
  float  distance_to_light = length( world_to_light );
  float3 light_dir = world_to_light / distance_to_light;

  float  NdL = saturate(dot(g.n, light_dir));
  float  NdV = saturate(dot(g.n, g.view_dir));
  float3 h   = normalize(light_dir + g.view_dir); // half vector

  float  NdH = saturate(dot(g.n, h));
  float  VdH = saturate(dot(g.view_dir, h));
  float  LdV = saturate(dot(light_dir, g.view_dir));

  // max is to avoid reach zero
  float  a   = max(0.001f, g.roughness * g.roughness);
  float3 cDiff = Diffuse(g.albedo);
  float3 cSpec = Specular(g.specular_color, h, g.view_dir, light_dir, a, NdL, NdV, NdH, VdH, LdV);

  // This is not correct, it should be 1/(r*r)
  float att = 1.0 - saturate( distance_to_light / light_max_radius );

  float3 final_color = light_color.xyz * NdL * (cDiff * (1.0f - cSpec) + cSpec) * att * light_intensity * shadow_factor;
  return float4(final_color,1);
}

// ---------------------------------------------------
float4 PS_Light_Point( VS_OUTPUT input ) : SV_Target 
{
  return PS_Light_Common( input, false, false );
}

float4 PS_Light_Spot( VS_OUTPUT input ) : SV_Target 
{
  return PS_Light_Common( input, true, false );
}

float4 PS_Light_Spot_Shadows( VS_OUTPUT input ) : SV_Target 
{
  return PS_Light_Common( input, true, true );
}