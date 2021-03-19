#pragma once

#include <d3d11_1.h>

#include <vector>

namespace Sky {
GW::MATH::GMATRIXF sphereWorld;
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> skytex;

void CreateSphere(GW::GRAPHICS::GDirectX11Surface&);

void CreateSphere(GW::GRAPHICS::GDirectX11Surface& d3d) {
  // Get device for loading
  ID3D11Device* creator;
  d3d.GetDevice((void**)&creator);
  CreateDDSTextureFromFile(creator, L"../asset/skybox.dds", nullptr,
                           skytex.GetAddressOf());
}

}  // namespace Sky
