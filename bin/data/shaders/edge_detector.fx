//--------------------------------------------------------------------------------------
#include "common.h"

void VS(
  in float4 iPos : POSITION
, out float4 oPos : SV_POSITION
, out float2 oTex0 : TEXCOORD0
)
{
  // Passthrough of coords and UV's
  // Convert input values from 0..1 to -1..1, no need for a camera
  oPos = float4(iPos.x * 2 - 1., 1 - iPos.y * 2, 0, 1);
  oTex0 = iPos.xy;
}


float4 PS(
  in float4 iPosition : SV_POSITION
  , in float2 iTex0 : TEXCOORD0
) : SV_Target{

  float dx = inv_render_width;
  float dy = inv_render_height;

/*
  // https://es.wikipedia.org/wiki/Operador_Sobel
  float4 c_00 = txDeferredLinearDepths.Sample(clampLinear, iTex0.xy + float2(-dx, -dy));
  float4 c_01 = txDeferredLinearDepths.Sample(clampLinear, iTex0.xy + float2(  0, -dy));
  float4 c_02 = txDeferredLinearDepths.Sample(clampLinear, iTex0.xy + float2( dx, -dy));
  float4 c_10 = txDeferredLinearDepths.Sample(clampLinear, iTex0.xy + float2(-dx, 0));
  //float4 c_11 = txDeferredLinearDepths.Sample(clampLinear, iTex0.xy + float2( 0, 0));
  float4 c_12 = txDeferredLinearDepths.Sample(clampLinear, iTex0.xy + float2( dx,  0));
  float4 c_20 = txDeferredLinearDepths.Sample(clampLinear, iTex0.xy + float2(-dx,  dy));
  float4 c_21 = txDeferredLinearDepths.Sample(clampLinear, iTex0.xy + float2( 0,   dy));
  float4 c_22 = txDeferredLinearDepths.Sample(clampLinear, iTex0.xy + float2( dx,  dy));

  float gx = (-1) * c_00 + (-2) * c_10 + (-1) * c_20
           +  (1) * c_02 + (2) * c_12 + (1) * c_22
    ;

  float gy = (-1) * c_00 + (-2) * c_01 + (-1) * c_02
           +  (1) * c_20 + (2) * c_21 + (1) * c_22
    ;

  float g = sqrt( gx*gx + gy * gy );
    */

    // Cheaper 3 samples
  float4 c_00 = txDeferredLinearDepths.Sample(clampLinear, iTex0.xy + float2(0,0));
  float4 c_01 = txDeferredLinearDepths.Sample(clampLinear, iTex0.xy + float2(dx,0));
  float4 c_10 = txDeferredLinearDepths.Sample(clampLinear, iTex0.xy + float2(0,dy));
  float zx = abs(c_10 - c_00);
  float zy = abs( c_01 - c_00);
  float gz = zx + zy;

  if( gz < 1 ) 
    gz = 0;

    // Cheaper 3 samples
  float3 n_00 = decodeNormal(txDeferredNormals.Sample(clampLinear, iTex0.xy + float2(0,0)).xyz);
  float3 n_01 = decodeNormal(txDeferredNormals.Sample(clampLinear, iTex0.xy + float2(dx*2,0)).xyz);
  float3 n_10 = decodeNormal(txDeferredNormals.Sample(clampLinear, iTex0.xy + float2(0,dy*2)).xyz);
  float nx = 1 - saturate(dot( n_00, n_01 ));
  float ny = 1 - saturate(dot( n_00, n_10 ));
  float gn = nx * nx + ny * ny;
  if( gn < 0.5 ) 
    gn = 0;

  float g = gz + gn;

  float4 color = txAlbedo.Sample(clampLinear, iTex0.xy);
  return color * ( 1 - g ) + float4(1,1,0,1) * g;;

}

