//--------------------------------------------------------------------------------------
#include "common.h"

//--------------------------------------------------------------------------------------
float3 toneMappingReinhard(float3 hdr, float k = 1.0) {
    return hdr / (hdr + k);
}

float3 gammaCorrect( float3 linear_color ) {
  return pow( abs(linear_color), 1. / 2.2 ); 
}

float3 applyColorGrading(float3 input) {
  float3 color_graded = txColorGrading.Sample(clampLinear, input).xyz;
  return color_graded * amount_color_grading + input * (1 - amount_color_grading);
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( in VtxFullScreen input ): SV_Target
{
  float3 hdr_color = txAlbedo.Sample(clampLinear, input.Uv).xyz;

  hdr_color *= exposure_factor;

  float3 tone_mapped_color = toneMappingReinhard( hdr_color );

  float3 gamma_corrected_color = gammaCorrect( tone_mapped_color );

#if !defined(RELEASE_VERSION)
  if (output_channel == OUTPUT_ALBEDOS)
    return txDeferredAlbedos.Sample(clampLinear, input.Uv);
  if (output_channel == OUTPUT_NORMALS)
    return txDeferredNormals.Sample(clampLinear, input.Uv);
  if (output_channel == OUTPUT_BEFORE_TONE_MAPPING)
    return float4(hdr_color, 1);
  if (output_channel == OUTPUT_BEFORE_GAMMA_CORRECTION)
    return float4(tone_mapped_color, 1);
  if (output_channel == OUTPUT_SCREEN_SPACE_AO) {
    float ao_amount = txScreenSpaceAO.Sample(clampLinear, input.Uv).x;
    return ao_amount;
  }
#endif

  float3 color_graded = applyColorGrading( gamma_corrected_color );
  return float4(color_graded, 1);
}


