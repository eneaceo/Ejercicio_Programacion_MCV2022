//--------------------------------------------------------------------------------------
#include "common.h"

//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR0;        // Not used
    float2 Uv : TEXCOORD0;
    float3 WorldPos : TEXCOORD1;
    float3 WorldNormal : NORMAL;
    float4 WorldTan : TEXCOORD2;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(VtxNormalUvTan input)
{
  VS_OUTPUT output;
  float3 local_pos = input.Pos.xyz + input.Normal * 0.1 * cos( world_time ); 
  float4 world_pos = mul(float4(local_pos, 1), object_world);
  output.WorldPos = world_pos.xyz;
  output.WorldNormal = mul(input.Normal, (float3x3)object_world);
  output.Pos = mul(world_pos, camera_view_projection);
  output.Color = float4((input.Normal + 1) * 0.5, 1);
  output.Uv = input.Uv;
  output.WorldTan.xyz = mul(input.Tan.xyz, (float3x3)object_world);
  output.WorldTan.w = input.Tan.w;
  return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(
  in VS_OUTPUT input
) : SV_Target
{

  // 4 channels between -1..1 continuous in object space 
  float4 raw_noise = txNoise.Sample( wrapLinear, input.Uv * 8 + world_time * 0.1);
  float4 raw_noise2 = txNoise.Sample( wrapLinear, input.Uv * 1 + world_time * 0.1);
  float4 noise_color = (raw_noise + raw_noise2 ) * 0.5 * 2 - 1;
  float3 distorsion = 0.1 * noise_color.xyz;

  // Beware!, this is not normalized, the distorsion should be in the tangent space
  float3 n = input.WorldNormal + 0.5 * distorsion;

  // Eye to object
  float3 incident_dir = normalize(input.WorldPos - camera_position);
  float3 reflected_dir = reflect( incident_dir, n );
  float4 reflected_color = txEnvironment.Sample(wrapLinear, reflected_dir);

  // Project into the camera screen coords.
  float4 world_pos = float4( input.WorldPos, 1 );
  world_pos.xyz += distorsion;
  float4 proj_coords = mul( world_pos, camera_view_projection );
  float2 homo_coords = proj_coords.xy / proj_coords.w;
  homo_coords.x = ( homo_coords.x + 1 ) * 0.5;
  homo_coords.y = ( 1 - homo_coords.y ) * 0.5;
    
  // Depth of the solid underwater
  float waterbed_linear_depth = txDeferredLinearDepths.Sample(clampLinear, homo_coords).x;
  float3 cam2world_pos = input.WorldPos - camera_position;
  float  forward_units = dot( cam2world_pos, camera_forward );

  // Distance vs our distance to the camera is how much distance we are travelling through
  // the water. The 0.5 is modulates when we don't see the under color anymore
  float  underwater_distance_factor = saturate( ( waterbed_linear_depth - forward_units ) * 1.1 );

  // ---------------------------
  float4 water_color = float4(0.2,0.6,0.3,1);    // An arbitrary water color

  // This is the pure color of the background
  float4 under_color = txDeferredOutput.Sample(clampLinear, homo_coords);

  // Mix the water color with the background based on the distance travelled
  under_color = under_color * ( 1 - underwater_distance_factor ) + (water_color * underwater_distance_factor);

  // ---------------------------
  // This will be used to modulate the reflections
  float fresnel = dot( n, -incident_dir );
  float4 final_color = under_color * fresnel + reflected_color * ( 1 - fresnel );

  // ---------------------------
  // This uses the last light activated!
  float shadow_factor = getShadowFactor( world_pos );

  // Specular approx
  float3 world_to_light = light_position - world_pos;
  float  distance_to_light = length(world_to_light); 
  float3 L = world_to_light / distance_to_light;
  float3 H = normalize( L - incident_dir );
  float cos_beta = saturate( dot( H, n ) );
  float specular_amount = light_intensity * pow( cos_beta, 80 );

  // This is not correct, it should be 1/(r*r)
  float att = 1.0 - saturate( distance_to_light / light_max_radius );
  shadow_factor *= att;

  return (final_color) * saturate( ambient_factor + shadow_factor ) + specular_amount * shadow_factor;
  return float4(input.Uv,0,1);
}

