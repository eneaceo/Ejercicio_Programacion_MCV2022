#include "common.h"
#include "constants_particles.h"

float4 sampleColor( float t ) {

  // Convert range 0..1 to 0..7
  float time_in_color_table = t * ( 8 - 1 );

  // if time_in_color_table = 1.2.      color_entry = 1., color_amount = 0.2
  float color_entry;
  float color_amount = modf( time_in_color_table, color_entry);
  float4 color_over_time = psystem_colors_over_time[ color_entry ] * ( 1 - color_amount )
                         + psystem_colors_over_time[ color_entry + 1 ] * ( color_amount );

  return color_over_time;
}

float sampleScale( float t ) {

  // Convert range 0..1 to 0..7
  float time_in_table = t * ( 8 - 1 );

  // if time_in_table = 1.2.      entry = 1., amount = 0.2
  float entry;
  float amount = modf( time_in_table, entry);
  float4 over_time = psystem_sizes_over_time[ entry ] * ( 1 - amount )
                   + psystem_sizes_over_time[ entry + 1 ] * ( amount );

  return over_time.x;
}

TParticle spawnParticle(uint unique_id);
void updateParticle(inout TParticle p);

// --------------------------------------------------------------
// IndirectDraw are 5 ints: NumIndices, NumInstances, FirstIndex, FirstVertex, FirstInstance
//                  Offset:          0             4           8           12             16
//              Normally  :  (fromMesh),   GPU      , (fromMesh),  0,           0
#define OFFSET_NUM_PARTICLES_TO_DRAW   4
#define NUM_PARTICLES_PER_THREAD_GROUP 32

// --------------------------------------------------------------
// using a single thread to spawn new particles
[numthreads(1, 1, 1)]
void cs_particles_spawn(
  uint thread_id : SV_DispatchThreadID,
  RWStructuredBuffer<TParticle> instances : register(u0),
  RWStructuredBuffer<TSystem>   system : register(u1),
  RWByteAddressBuffer indirect_draw : register(u2),
  RWByteAddressBuffer indirect_update : register(u3)
)
{
  // We start from the num particles left in the prev frame
  uint   nparticles_active = indirect_draw.Load(OFFSET_NUM_PARTICLES_TO_DRAW);

  // Clear num instances for indirect draw call. At offset 4, set zero
  indirect_draw.Store(OFFSET_NUM_PARTICLES_TO_DRAW, 0);

  // Get access to the max capacity of the buffer
  uint max_elements, bytes_per_instance;
  instances.GetDimensions(max_elements, bytes_per_instance);

  // Can we spawn particles?
  if (nparticles_active + emitter_num_particles_per_spawn <= max_elements)
  {

    system[0].time_to_next_spawn -= world_delta_time;
    if (system[0].time_to_next_spawn < 0)
    {
      system[0].time_to_next_spawn += emitter_time_between_spawns;

      // Spawn N
      uint unique_id = system[0].next_unique_id;
      for (uint i = 0; i < emitter_num_particles_per_spawn; ++i)
      {
        instances[nparticles_active] = spawnParticle(unique_id);
        ++nparticles_active;
        ++unique_id;
      }
      system[0].next_unique_id = unique_id;
    }
  }

  // Update DispatchIndirect 1st argument.
  uint nthread_groups = (nparticles_active + NUM_PARTICLES_PER_THREAD_GROUP - 1) / NUM_PARTICLES_PER_THREAD_GROUP;
  indirect_update.Store(0, nthread_groups);
  indirect_update.Store(4, 1);
  indirect_update.Store(8, 1);
  system[0].num_particles_to_update = nparticles_active;
}


// --------------------------------------------------------------
[numthreads(NUM_PARTICLES_PER_THREAD_GROUP, 1, 1)]
void cs_particles_update(
  uint thread_id : SV_DispatchThreadID,
  StructuredBuffer<TParticle> instances : register(t0),
  StructuredBuffer<TSystem>   system : register(t1),
  RWStructuredBuffer<TParticle> instances_active : register(u0),
  RWByteAddressBuffer indirect_draw : register(u2)
)
{
  // Do not simulate particles if the particle is not active.
  // If I have 40 particles, thread_group=32, 2 groups of 32 threads execute this
  // code. 
  if (thread_id >= system[0].num_particles_to_update)
    return;

  TParticle p = instances[thread_id];

  // Has died?
  p.time_normalized += world_delta_time * p.time_factor;
  if (p.time_normalized >= 1)
    return;

  // Call the specific method to update each particle
  updateParticle(p);

  // Update indirect draw call args
  uint index;
  indirect_draw.InterlockedAdd(OFFSET_NUM_PARTICLES_TO_DRAW, 1, index);

  // Save in the nexts buffer
  instances_active[index] = p;
}
