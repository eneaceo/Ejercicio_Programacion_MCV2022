
DECL_SHADER_CTE(CtesParticleSystem, CB_SLOT_PARTICLES)
{
  float emitter_time_between_spawns;
  uint  emitter_num_particles_per_spawn;
  float emitter_initial_speed;
  float emitter_dummy;

  float emitter_initial_scale_avg;
  float emitter_initial_scale_range;
  float emitter_dummy2;
  float emitter_dummy3;

  // More data needed by the update
  // 4 floats
  float4 psystem_colors_over_time[8];

  // Stored as float4 because hlsl will access it as array
  // Only the .x is currently being used in the shader particles.inc
  float4 psystem_sizes_over_time[8];
};

// Data associated to a single particle
// Size must be kept in sync with compute_particles.json/buffers/instances/bytes_per_elem & instances_active
struct TParticle {
  float3 pos;
  float  scale;

  float3 speed;
  float  time_normalized;   // 0...1  birth ... death

  float4 color;

  float  time_factor;       // Speed to increase time_normalized
  uint   unique_id;
  float  initial_scale;
  float  particle_dummy2;     
};

// Structure to keep the state of the full system
struct TSystem
{
  float  time_to_next_spawn;
  uint   num_particles_to_update;
  uint   next_unique_id;
  uint   system_dummy;
};
