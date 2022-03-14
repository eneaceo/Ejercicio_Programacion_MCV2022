#include "common.h"

#define NUM_PARTICLES_PER_GROUP 5

[numthreads(NUM_PARTICLES_PER_GROUP,1,1)]
void cs_demo( 
  uint thread_id : SV_DispatchThreadID
, RWStructuredBuffer<TWorldInstance> world_instances : register(u0)
) {

  world_instances[thread_id].world = float4x4( 
    1, 0, 0, 0, 
    0, 1, 0, 0,
    0, 0, 1, 0,
    thread_id * cs_demo_distance, 
    cos(world_time + thread_id * cs_demo_phase) * cs_demo_amplitude, 
    sin(world_time + thread_id * cs_demo_phase) * cs_demo_amplitude, 
    1 );
}
