#include "shader_platform.h"

#define MAX_SUPPORTED_BONES     120
#define PI 3.14159265359f

#define CB_SLOT_CAMERA      0
#define CB_SLOT_OBJECT      2
#define CB_SLOT_SKIN        3
#define CB_SLOT_LIGHT       4
#define CB_SLOT_WORLD       5
#define CB_SLOT_UI			    6
#define CB_SLOT_BLUR			  7
#define CB_SLOT_FOCUS			  8
#define CB_SLOT_BLOOM			  9
#define CB_SLOT_INSTANCING	10
#define CB_SLOT_CUSTOM			11
#define CB_SLOT_PARTICLES		12

#define TS_ALBEDO           0
#define TS_NORMAL           1
#define TS_ROUGHNESS        2
#define TS_METALLIC         3
#define TS_EMISSIVE         4
#define TS_NUM_SLOTS_PER_MATERIAL (TS_EMISSIVE+1)

#define TS_LIGHT_PATTERN    5
#define TS_LIGHT_SHADOW_MAP 6
#define TS_ENVIRONMENT      7
#define TS_IRRADIANCE       8
#define TS_NOISE            9

#define TS_DEFERRED_ALBEDOS      10
#define TS_DEFERRED_NORMALS      11
#define TS_DEFERRED_LINEAR_DEPTH 12
#define TS_DEFERRED_OUTPUT       13
#define TS_DEFERRED_AO           14

#define TS_LUT_COLOR_GRADING     15


//--------------------------------------------------------------------------------------
#define OUTPUT_COMPLETE			0
#define OUTPUT_ALBEDOS			1
#define OUTPUT_NORMALS			2
#define OUTPUT_BEFORE_TONE_MAPPING	3
#define OUTPUT_BEFORE_GAMMA_CORRECTION 4
#define OUTPUT_SCREEN_SPACE_AO     5

//--------------------------------------------------------------------------------------
DECL_SHADER_CTE(CtesCamera, CB_SLOT_CAMERA)
{
	matrix camera_projection;
	matrix camera_view;
	matrix camera_view_projection;
	matrix camera_inverse_view_projection;
	
	float3 camera_forward;
	float  camera_zfar;
	
	float3 camera_position;
	float  camera_tan_half_fov;
	
	float3 camera_right;
	float  camera_aspect_ratio;

	float3 camera_up;
	float  camera_dummy1;
};

DECL_SHADER_CTE(CtesObject, CB_SLOT_OBJECT) {
	matrix object_world;
	float4 object_color;
};

DECL_SHADER_CTE(CtesLight, CB_SLOT_LIGHT) {
	matrix light_view_projection;
	matrix light_view_projection_offset;			// Add the -1..1 to 0..1 conversion
	float3 light_position;
	float  light_intensity;
	float3 light_forward;
	float  light_max_radius;
	float4 light_color;

	float  light_shadows_inv_resolution;
	float  light_shadows_step;
	float  light_shadows_step_over_resolution;
	float  light_shadows_bias;
};

DECL_SHADER_CTE(CtesWorld, CB_SLOT_WORLD) {
	float  render_width;
	float  render_height;
	float  inv_render_width;
	float  inv_render_height;

	float  world_time;
	float  ambient_factor;
	float  exposure_factor;
	float  irrandiance_texture_mipmap_factor;

	int    output_channel;
	float  amount_color_grading;
	float  world_delta_time;
	int    world_dummy3;
};


DECL_SHADER_CTE(CtesSkin, CB_SLOT_SKIN)
{
	matrix bones[MAX_SUPPORTED_BONES];
};

DECL_SHADER_CTE(CtesUI, CB_SLOT_UI)
{
	float2 ui_min_uv;
	float2 ui_max_uv;
};

DECL_SHADER_CTE(CtesParticle, CB_SLOT_PARTICLES)
{
	float2 particle_min_uv;
	float2 particle_max_uv;
};

DECL_SHADER_CTE(CtesBlur, CB_SLOT_BLUR)
{
	float4 blur_w;        // weights
	float4 blur_d;        // distances for the 1st, 2nd and 3rd tap
	float2 blur_step;     // Extra modifier
	float2 blur_center;   // To keep aligned x4
};

DECL_SHADER_CTE(CtesFocus, CB_SLOT_FOCUS)
{
	float focus_z_center_in_focus;
	float focus_z_margin_in_focus;
	float focus_transition_distance;
	float focus_modifier;
};

DECL_SHADER_CTE(CtesBloom, CB_SLOT_BLOOM)
{
	float4 bloom_weights;

	float  bloom_threshold_min;
	float  bloom_multiplier;
	float  bloom_dummy1;
	float  bloom_dummy2;
};

DECL_SHADER_CTE(CtesCSDemo, CB_SLOT_CUSTOM)
{
	float  cs_demo_amplitude;
	float  cs_demo_distance;
	float  cs_demo_phase;
	float  cs_demo_dummy;
};
