#if defined(_WIN32)

#define matrix                    MAT44
#define float4                    VEC4
#define float3                    VEC3
#define float2                    VEC2
#define uint                      uint32_t
#define DECL_SHADER_CTE(buf_name,slot_id)  struct buf_name

#else

#define DECL_SHADER_CTE(buf_name,slot_id)  cbuffer buf_name : register(b##slot_id)
#define TEXTURE_SLOT(slot_id)              : register(t##slot_id)

#endif