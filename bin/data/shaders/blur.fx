//--------------------------------------------------------------------------------------
#include "common.h"

void VS(
  in float4 iPos : POSITION
, out float4 oPos : SV_POSITION
, out float2 oTex0 : TEXCOORD0
, out float4 oTex1 : TEXCOORD1
, out float4 oTex2 : TEXCOORD2
, out float4 oTex3 : TEXCOORD3
)
{
  // Passthrough of coords and UV's
  // Convert input values from 0..1 to -1..1, no need for a camera
  oPos = float4(iPos.x * 2 - 1., 1 - iPos.y * 2, 0, 1);
  oTex0 = iPos.xy;

  // Save in the xy the positive offset 
  // Save in the zw the negative offset 
  oTex1.xy = iPos.xy + blur_step * blur_d.x;
  oTex1.zw = iPos.xy - blur_step * blur_d.x;

  oTex2.xy = iPos.xy + blur_step * blur_d.y;
  oTex2.zw = iPos.xy - blur_step * blur_d.y;

  oTex3.xy = iPos.xy + blur_step * blur_d.z;
  oTex3.zw = iPos.xy - blur_step * blur_d.z;
}


float4 PS(
  in float4 iPosition : SV_POSITION
, in float2 iTex0 : TEXCOORD0
, in float4 iTex1 : TEXCOORD1
, in float4 iTex2 : TEXCOORD1
, in float4 iTex3 : TEXCOORD1
) : SV_Target{

  float4 c0 = txAlbedo.Sample( clampLinear, iTex0 );
  float4 cp1 = txAlbedo.Sample( clampLinear, iTex1.xy );
  float4 cn1 = txAlbedo.Sample( clampLinear, iTex1.zw );
  float4 cp2 = txAlbedo.Sample( clampLinear, iTex2.xy );
  float4 cn2 = txAlbedo.Sample( clampLinear, iTex2.zw );
  float4 cp3 = txAlbedo.Sample( clampLinear, iTex3.xy );
  float4 cn3 = txAlbedo.Sample( clampLinear, iTex3.zw );

  float4 cfinal = 
    (c0       ) * blur_w.x
  + (cp1 + cn1) * blur_w.y
  + (cp2 + cn2) * blur_w.z
  + (cp3 + cn3) * blur_w.w
  ;

  return cfinal;
}

