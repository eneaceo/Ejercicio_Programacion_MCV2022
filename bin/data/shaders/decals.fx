//--------------------------------------------------------------------------------------
#include "common.h"

struct V2F
{
  float4 Pos : SV_POSITION;
  float4 Color : COLOR0;

  float4 InvRow0 : TEXCOORD0;
  float4 InvRow1 : TEXCOORD1;
  float4 InvRow2 : TEXCOORD2;
  float4 InvRow3 : TEXCOORD3;
  float3 ProjectionDir : TEXCOORD4;
};


V2F VS_Common(Vtx input, float4x4 world )
{
  V2F output = (V2F)0;
  output.Pos = mul(input.Pos, object_world);
  output.Pos = mul(output.Pos, camera_view_projection);
  output.Color = object_color;

  // Compute the inverse of the given world
  float3 axisX = float3( world._m00, world._m01, world._m02 );
  float3 axisY = float3( world._m10, world._m11, world._m12 ); 
  float3 axisZ = float3( world._m20, world._m21, world._m22 );
  float3 trans = float3( world._m30, world._m31, world._m32 );

  float tx = -dot( axisX, trans );
  float ty = -dot( axisY, trans );
  float tz = -dot( axisZ, trans );

  float sx = 1 / dot( axisX, axisX );
  float sy = 1 / dot( axisY, axisY );
  float sz = 1 / dot( axisZ, axisZ );

  output.InvRow0 = float4( axisX.x*sx, axisY.x*sy, axisZ.x*sz, 0 );
  output.InvRow1 = float4( axisX.y*sx, axisY.y*sy, axisZ.y*sz, 0 );
  output.InvRow2 = float4( axisX.z*sx, axisY.z*sy, axisZ.z*sz, 0 );
  output.InvRow3 = float4(      tx*sx,      ty*sy,      tz*sz, 1 );

  output.ProjectionDir = normalize(axisY);
  return output;
}

V2F VS(Vtx input)
{
  return VS_Common(input, object_world);
}


float3 getWorldCoords(float2 iPosition)
{
  // Normalize to 0..1  
  float2 xy = float2(iPosition.x * inv_render_width, iPosition.y * inv_render_height);

  float3 viewDir = float3(xy, 1);
  viewDir.y = viewDir.y * 2.0 - 1.0;
  viewDir.x = viewDir.x * 2.0 - 1.0;
  viewDir.y *= camera_tan_half_fov;
  viewDir.x *= camera_tan_half_fov * camera_aspect_ratio;
  float3 worldDir = camera_forward * viewDir.z
    + camera_right * viewDir.x
    - camera_up * viewDir.y;

  float zlinear = txDeferredLinearDepths.Sample(wrapLinear, xy).x;
  return camera_position + worldDir * zlinear;
}


// --------------------------------------
float4 PS(V2F input) : SV_Target{
  float3 wPos = getWorldCoords(input.Pos.xy);

  float2 xy = float2(input.Pos.x * inv_render_width, input.Pos.y * inv_render_height);
  float4 normal_01 = txDeferredNormals.Sample(wrapLinear, xy);
  float3 normal = decodeNormal( normal_01.xyz );  

  // Do disable the decal if the surface normal is perp to the decal direction
  float perp_factor = saturate(abs(dot( input.ProjectionDir, normal )));

  float4x4 InvWorld = float4x4(
    input.InvRow0,
    input.InvRow1,
    input.InvRow2,
    input.InvRow3
    );

  // The 'inside' of the box is the range -0.5..0.5
  float3 localPos = mul( float4(wPos,1), InvWorld ).xyz ;
  
  // Skip if too going outside of the volume by up or down
  if( abs(localPos.y) > 0.5 )
    return float4(0,0,1,1);

  // Convert to the range 0..1
  localPos.xz += 0.5;

  float4 color = txAlbedo.Sample( borderLinear, localPos.xz );

  // To atenuate the effect of the normal x projection dir
  //perp_factor = pow(perp_factor,0.2);

  color.a *= perp_factor;

  return color;

  return float4(localPos.xz, 0, 1 );


  return worldUnits(localPos.xyz);

  return input.Color;
}

// Can we optimize a bit this??
// Can we render all decals of the same type at once? -> Instancing
// Can the decals fade out with the time?
// Can we avoid the player to be affected by the decals?
//   Use the stencil to mark which pixel are the character
//   Enforce the decals pipeline to use the test stencil != characters stencil value