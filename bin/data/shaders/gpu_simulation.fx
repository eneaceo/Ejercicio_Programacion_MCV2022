//--------------------------------------------------------------------------------------
#include "common.h"

// 1 group has 64x64 threads
// --------------------------------------------------------------
[numthreads(32, 32, 1)]
void cs_simulation(
  uint2 id : SV_DispatchThreadID     // Unique id as uint
, Texture2D<half> inTexture : register(t0)			// read only -> srv
, RWTexture2D<half> outTexture : register(u0)	// read write -> uav
) 
{
	if (world_time < 5 )
	{
		if (id.x == 32 && id.y == 32)
		{
			outTexture[id] = 1.0f;
		}
		else
			outTexture[id] = 0.5f;
	}
	else
	{
		float up = inTexture[id + int2(0, -1)];
		float dw = inTexture[id + int2(0, 1)];
		float left = inTexture[id + int2(1, 0)];
		float right = inTexture[id + int2(-1, 0)];
		float center = outTexture[id];
		float new_value = (up + dw + left + right) / 2 - center;
		//new_value *= 0.995f;
		outTexture[id] = new_value;
	}
}