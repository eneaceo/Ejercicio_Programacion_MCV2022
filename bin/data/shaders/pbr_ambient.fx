//--------------------------------------------------------------------------------------
#include "pbr_funcs.h"

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( in VtxFullScreen input ): SV_Target
{

  // Get access to the gbuffer
  GBuffer g;
  decodeGBuffer( input.Pos.xy, g );

  float ssao = txScreenSpaceAO.Sample(clampLinear, input.Uv).x;
  
  // Reflected color, the mipmap depends on the roughness for each pixel
  float mip_index = g.roughness * g.roughness * 7.0;
  float3 env = txEnvironment.SampleLevel(wrapLinear, g.reflected_dir, mip_index).xyz;
  // Convert value to linear space
  env = pow(abs(env), 2.2f);

  // Another approximation to know which color arrives to the point based on 
  // the normal
  float4 irradiance_n = txEnvironment.SampleLevel(wrapLinear, g.n, 7);
  float4 irradiance_ok = txIrradiance.Sample(wrapLinear, g.n);
  float4 irradiance = irradiance_n * irrandiance_texture_mipmap_factor + irradiance_ok * ( 1 - irrandiance_texture_mipmap_factor );
  
    // How much the environment we see
  float3 env_fresnel = Specular_F_Roughness(g.specular_color, 1. - g.roughness * g.roughness, g.n, g.view_dir);

  float4 final_color = float4(
      env_fresnel * env
    + g.albedo.xyz * irradiance.xyz
      , 1.0f
      );

  return final_color * ambient_factor * ssao;
}

