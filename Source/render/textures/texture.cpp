#include "mcv_platform.h"
#include "texture.h"
#include "DirectXTK/DDSTextureLoader.h"

class CTextureResourceType : public CResourceType {
public:
  const char* getExtension(int idx) const { return ".dds"; }
  const char* getName() const { return "Texture"; }
  IResource* create(const std::string& name) const {
    CTexture* texture = new CTexture();
    bool is_ok = texture->createFromFile(name);
    assert(is_ok);
    return texture;
  }
};

// -----------------------------------------
template<>
CResourceType* getClassResourceType<CTexture>() {
  static CTextureResourceType factory;
  return &factory;
}

void CTexture::setDXParams(ID3D11Texture2D* new_texture, ID3D11ShaderResourceView* new_srv) {
  //xres = new_xres;
  //yres = new_yres;
  texture = new_texture;
  shader_resource_view = new_srv;
  new_texture->AddRef();
  new_srv->AddRef();
}

// -----------------------------------------
void CTexture::destroy() {
  SAFE_RELEASE(shader_resource_view);
  SAFE_RELEASE(texture);
}

// ------------------------------------------------
bool CTexture::create(
  int nxres
  , int nyres
  , DXGI_FORMAT nformat
  , eCreateOptions options
) {

  D3D11_TEXTURE2D_DESC desc = {};
  desc.Width = nxres;
  desc.Height = nyres;
  desc.MipLevels = 1;
  desc.ArraySize = 1;
  desc.Format = nformat;
  desc.SampleDesc.Count = 1;
  desc.SampleDesc.Quality = 0;
  desc.Usage = D3D11_USAGE_DEFAULT;
  desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
  desc.CPUAccessFlags = 0;
  desc.MiscFlags = 0;

  if (options == CREATE_DYNAMIC) {
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

  }
  else if (options == CREATE_RENDER_TARGET) {
    desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
  }
  else if (options == CREATE_WITH_COMPUTE_SUPPORT) {
    desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;

  }
  else {
    assert(options == CREATE_STATIC);
  }

  ID3D11Texture2D* tex2d = nullptr;
  HRESULT hr = Render.device->CreateTexture2D(&desc, nullptr, &tex2d);
  if (FAILED(hr))
    return false;
  texture = tex2d;
  setDXName(texture, getName().c_str());

  // -----------------------------------------
  // Create a resource view so we can use the data in a shader
  D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
  ZeroMemory(&srv_desc, sizeof(srv_desc));
  srv_desc.Format = nformat;
  srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
  srv_desc.Texture2D.MipLevels = desc.MipLevels;

  hr = Render.device->CreateShaderResourceView(texture, &srv_desc, &shader_resource_view);
  if (FAILED(hr))
    return false;
  setDXName(shader_resource_view, getName().c_str());

  // -----------------------------------------
  // If an UAV object has been requested, create it
  if (desc.BindFlags & D3D11_BIND_UNORDERED_ACCESS) {
    D3D11_UNORDERED_ACCESS_VIEW_DESC uav_desc = {};
    uav_desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
    uav_desc.Texture2D.MipSlice = 0;
    uav_desc.Format = DXGI_FORMAT_UNKNOWN;
    hr = Render.device->CreateUnorderedAccessView(texture, &uav_desc, &uav);
    if (FAILED(hr))
      return false;
    setDXName(uav, getName().c_str());
  }

  return true;
}



void CTexture::activate(int nslot) const {
  Render.ctx->PSSetShaderResources(nslot, 1, &shader_resource_view);
}

void CTexture::deactivate(int nslot) {
  ID3D11ShaderResourceView* null_srv = nullptr;
  Render.ctx->PSSetShaderResources(nslot, 1, &null_srv);
}

bool CTexture::createFromFile(const std::string& filename) {

  wchar_t wFilename[MAX_PATH];
  mbstowcs(wFilename, filename.c_str(), MAX_PATH);

  HRESULT hr = DirectX::CreateDDSTextureFromFile(
    Render.device,
    wFilename,
    &texture,
    &shader_resource_view
  );

  if (FAILED(hr)) {
    fatal("Failed to load texture %s. HResult=%08x\n", filename.c_str(), hr);
    return false;
  }

  return true;
}

bool CTexture::renderInMenu() const {

  D3D11_RESOURCE_DIMENSION ResourceDimension;
  texture->GetType(&ResourceDimension);

  if (ResourceDimension == D3D11_RESOURCE_DIMENSION_TEXTURE2D) {
    ID3D11Texture2D* tex2D = (ID3D11Texture2D*)texture;
    D3D11_TEXTURE2D_DESC desc;
    tex2D->GetDesc(&desc);
    ImGui::Text("%dx%d %s", desc.Width, desc.Height, getName().c_str());
  }

  ImGui::Image(shader_resource_view, ImVec2(256, 256));

  return false;
}

bool CTexture::updateFromIYUV(const uint8_t* data, size_t data_size) {
  assert(data);
  D3D11_MAPPED_SUBRESOURCE ms;
  HRESULT hr = Render.ctx->Map(texture, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
  if (FAILED(hr))
    return false;

  // Read resolution and format from the texture
  ID3D11Texture2D* tex2d = static_cast<ID3D11Texture2D*>(texture);
  D3D11_TEXTURE2D_DESC desc;
  tex2d->GetDesc(&desc);
  int xres = desc.Width;
  int yres = desc.Height;
  DXGI_FORMAT format = desc.Format;

  uint32_t bytes_per_texel = 1;
  assert(format == DXGI_FORMAT_R8_UNORM);
  assert(data_size == xres * yres * 3 / 4);

  const uint8_t* src = data;
  uint8_t* dst = (uint8_t*)ms.pData;

  // Copy the Y lines
  uint32_t nlines = yres / 2;
  uint32_t bytes_per_row = xres * bytes_per_texel;
  for (uint32_t y = 0; y < nlines; ++y) {
    memcpy(dst, src, bytes_per_row);
    src += bytes_per_row;
    dst += ms.RowPitch;
  }

  // Now the U and V lines, need to add Width/2 pixels of padding between each line
  uint32_t uv_bytes_per_row = bytes_per_row / 2;
  for (uint32_t y = 0; y < nlines; ++y) {
    memcpy(dst, src, uv_bytes_per_row);
    src += uv_bytes_per_row;
    dst += ms.RowPitch;
  }

  Render.ctx->Unmap(texture, 0);
  return true;
}
