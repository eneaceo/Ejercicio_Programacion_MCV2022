#include "compute_particles_common.h"

// --------------------------------------------------------------
TParticle spawnParticle( uint unique_id ) {
  TParticle p;
  p.unique_id = unique_id;
  float2 rand_val = hash2(p.unique_id);
  float yaw = rand_val.x * 2 * PI;
  p.pos = float3( 0, 0, 0 );
  p.initial_scale = emitter_initial_scale_avg + emitter_initial_scale_range * ( rand_val.x * 2 - 1 );
  p.speed = emitter_initial_speed * normalize( float3( cos(yaw), 3 + 5 *rand_val.y, sin(yaw) ));
  p.time_factor = 0.15f;     // 1 / life_time
  p.particle_dummy2 = 0.f;
  p.color = float4(1,0,0,1);
  p.scale = p.initial_scale;
  p.time_normalized = 0.0f;
  return p;
}

void updateParticle( inout TParticle p ) {
  p.speed.y -= 9.8 * world_delta_time;
  p.pos += p.speed * world_delta_time;
  if (p.pos.y < 0)
  {
    p.speed.y = -p.speed.y * 0.7;
    p.pos.y = 0;
  }
  p.color = sampleColor( p.time_normalized );
  p.scale = p.initial_scale * sampleScale( p.time_normalized );
}
