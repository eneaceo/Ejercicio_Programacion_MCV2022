//--------------------------------------------------------------------------------------
#include "common.h"

struct GBuffer
{
  float3 pos;
  float3 n;
  float3 albedo;
  float3 specular_color;
  float  metallic;
  float  roughness;
  float3 view_dir;
  float3 reflected_dir;
  // ...
};

void decodeGBuffer( in float2 iPosition, out GBuffer g ) 
{
  // Normalize to 0..1
  float2 xy = float2( iPosition.x * inv_render_width, iPosition.y * inv_render_height );

  float3 viewDir = float3( xy, 1 );
  viewDir.y = viewDir.y * 2.0 - 1.0;
  viewDir.x = viewDir.x * 2.0 - 1.0;
  viewDir.y *= camera_tan_half_fov;
  viewDir.x *= camera_tan_half_fov * camera_aspect_ratio;
  float3 worldDir = camera_forward * viewDir.z
                  + camera_right * viewDir.x
                  - camera_up * viewDir.y;

  float zlinear = txDeferredLinearDepths.Sample(wrapLinear, xy).x;
  g.pos = camera_position + worldDir * zlinear;

  float4 normal_01 = txDeferredNormals.Sample(wrapLinear, xy);

  // Read pure albedo color from the texture
  float4 albedo = txDeferredAlbedos.Sample(wrapLinear, xy);

  // Metallic was stored in the alpha channel
  float metallic = albedo.a;

  // Fill the input structure
  g.n = decodeNormal( normal_01.xyz );  

  // Eye to object
  float3 incident_dir = normalize(g.pos - camera_position);
  g.reflected_dir = reflect( incident_dir, g.n);
  // View dir goes to the Eye
  g.view_dir = -incident_dir;
  
  // Apply gamma correction to albedo to bring it back to linear.
  albedo.rgb = pow(abs(albedo.rgb), 2.2f);

  // Lerp with metallic value to find the good diffuse and specular.
  // If metallic = 0, albedo is the albedo, if metallic = 1, the
  // used albedo is almost black
  g.albedo = albedo.xyz * ( 1. - metallic );

  // 0.03 default specular value for dielectric.
  g.specular_color = lerp(0.03f, albedo.rgb, metallic);

  // 
  g.metallic = albedo.a;
  g.roughness = normal_01.a;

}

// -------------------------------------------------
// Gloss = 1 - rough*rough
float3 Specular_F_Roughness(float3 specularColor, float gloss, float3 h, float3 v) {
  // Sclick using roughness to attenuate fresnel.
  return (specularColor + (max(gloss, specularColor) - specularColor) * pow((1 - saturate(dot(v, h))), 5));
}

float3 Diffuse(float3 pAlbedo) {
    return pAlbedo/PI;
}

float NormalDistribution_GGX(float a, float NdH)
{
    // Isotropic ggx.
    float a2 = a*a;
    float NdH2 = NdH * NdH;

    float denominator = NdH2 * (a2 - 1.0f) + 1.0f;
    denominator *= denominator;
    denominator *= PI;

    return a2 / denominator;
}

float Geometric_Smith_Schlick_GGX(float a, float NdV, float NdL)
{
    // Smith schlick-GGX.
    float k = a * 0.5f;
    float GV = NdV / (NdV * (1 - k) + k);
    float GL = NdL / (NdL * (1 - k) + k);
    return GV * GL;
}

float3 Fresnel_Schlick(float3 specularColor, float3 h, float3 v)
{
    return (specularColor + (1.0f - specularColor) * pow((1.0f - saturate(dot(v, h))), 5));
}

float Specular_D(float a, float NdH)
{
    return NormalDistribution_GGX(a, NdH);
}

float Specular_G(float a, float NdV, float NdL, float NdH, float VdH, float LdV) 
{
    return Geometric_Smith_Schlick_GGX( a, NdV, NdL );
}

float3 Specular_F(float3 specularColor, float3 h, float3 v)
{
    return Fresnel_Schlick(specularColor, h, v);
}

float3 Specular(float3 specularColor, float3 h, float3 v, float3 l, float a, float NdL, float NdV, float NdH, float VdH, float LdV)
{
    return ((Specular_D(a, NdH) * Specular_G(a, NdV, NdL, NdH, VdH, LdV)) * Specular_F(specularColor, v, h) ) / (4.0f * NdL * NdV + 0.0001f);
}
