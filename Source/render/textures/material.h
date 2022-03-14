#pragma once

#include "render/render_channel.h"

class CPipeline;
class CTexture;
struct TCompBuffers;

// Static texture which we load 
class CMaterial : public IResource {

  const CPipelineState* pipeline = nullptr;
  
  const CTexture* albedo = nullptr;
  const CTexture* normal = nullptr;
  const CTexture* roughness = nullptr;
  const CTexture* metallic = nullptr;
  const CTexture* emissive = nullptr;     // RGB

  bool  uses_skin = false;
  bool  uses_instanced = false;
  eRenderChannel render_channel = eRenderChannel::RC_SOLID;
  const CMaterial* material_shadows = nullptr;
  const CMaterial* material_skin = nullptr;
  const CMaterial* material_instanced = nullptr;
  int   priority = 0;

  // Optimization for dx11
  ID3D11ShaderResourceView* shader_resource_views[TS_NUM_SLOTS_PER_MATERIAL];

  void cacheSRVS();
  void activateTextures(int slot_base) const;

public:

  bool createFromJson(const json& j);
  void activate() const;
  void destroy();

  bool renderInMenu() const;

  bool usesSkin() const { return uses_skin; }
  bool usesInstanced() const { return uses_instanced; }
  eRenderChannel getRenderChannel() const { return render_channel; }
  const CMaterial* getShadowsMaterial() const { return material_shadows; }
  const CMaterial* getSkinMaterial() const { return material_skin; }
  const CMaterial* getInstancedMaterial() const { return material_instanced; }
  int getPriority() const { return priority; }
  
  void activateBuffers( TCompBuffers* buffers ) const;
  void setAlbedo(const CTexture* new_albedo);
};

