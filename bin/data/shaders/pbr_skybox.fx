//--------------------------------------------------------------------------------------
#include "common.h"

float4 PS(VtxFullScreen input) : SV_Target
{
  return txEnvironment.Sample(wrapLinear, input.WorldDir);
}
 