#pragma once

// Static texture which we load 
class CTexture : public IResource {

protected:

  // The texture object in d3d
  ID3D11Resource* texture = nullptr;

  // To be able to use the texture inside any shader, we need a shader resource view
  ID3D11ShaderResourceView*  shader_resource_view = nullptr;     // srv
  ID3D11UnorderedAccessView* uav = nullptr;

public:

  bool createFromFile(const std::string& filename);
  void activate(int nslot) const;
  static void deactivate(int slot);
  void destroy();
  void setDXParams(ID3D11Texture2D* new_texture, ID3D11ShaderResourceView* new_srv);
  ID3D11ShaderResourceView* getShaderResourceView() const { return shader_resource_view; }
  ID3D11UnorderedAccessView* getUAV() const { return uav; }

  // Create a new texture from params
  enum eCreateOptions {
    CREATE_STATIC
  , CREATE_DYNAMIC
  , CREATE_RENDER_TARGET
  , CREATE_WITH_COMPUTE_SUPPORT
  };
  bool create(int new_xres, int new_yres, DXGI_FORMAT new_color_format, eCreateOptions create_options = CREATE_STATIC);

  bool renderInMenu() const;

  bool updateFromIYUV(const uint8_t* new_data, size_t data_size);
};

