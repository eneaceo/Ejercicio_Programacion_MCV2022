//--------------------------------------------------------------------------------------
#include "common.h"

Texture2D txBlur0         TEXTURE_SLOT(0);
Texture2D txBlur1         TEXTURE_SLOT(1);
Texture2D txBlur2         TEXTURE_SLOT(2);
Texture2D txBlur3         TEXTURE_SLOT(3);

void VS(
  in float4 iPos : POSITION
, out float4 oPos : SV_POSITION
, out float2 oTex0 : TEXCOORD0
) {
  // Passthrough of coords and UV's
  // Convert input values from 0..1 to -1..1, no need for a camera
  oPos = float4(iPos.x * 2 - 1., 1 - iPos.y * 2, 0, 1);
  oTex0 = iPos.xy;
}


float4 PS_Filter(
  in float4 iPosition : SV_POSITION
  , in float2 iTex0 : TEXCOORD0
) : SV_Target{

  float4 c0 = txAlbedo.Sample(clampLinear, iTex0);
  float lum = c0.r * 0.2126 + c0.g * 0.7152 + c0.b * 0.0722;
  if (lum > bloom_threshold_min)
    return float4(1,1,1,1);
  return 0;
}

// --------------------------------------
float4 PS_Add(
  in float4 iPosition : SV_POSITION
, in float2 iTex0 : TEXCOORD0
) : SV_Target{

  float4 c0 = txBlur0.Sample(clampLinear, iTex0);
  float4 c1 = txBlur1.Sample(clampLinear, iTex0);
  float4 c2 = txBlur2.Sample(clampLinear, iTex0);
  float4 c3 = txBlur3.Sample(clampLinear, iTex0);

  float4 c = (
         c0 * bloom_weights.x 
       + c1 * bloom_weights.y
       + c2 * bloom_weights.z
       + c3 * bloom_weights.w
       ) * bloom_multiplier;
       ;
  return c;
}
