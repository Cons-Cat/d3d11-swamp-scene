// minimalistic code to draw a single triangle, this is not part of the API.
// required for compiling shaders on the fly, consider pre-compiling instead
#include <d3dcompiler.h>

#include "asset/test_pyramid.h"
#include "shader/pixl_simple.h"
#include "shader/vert_simple.h"
#include "struct_of_arrays.h"

#pragma comment(lib, "d3dcompiler.lib")

// Simple Vertex Shader
const char* vertexShaderSource = R"(
// an ultra simple hlsl vertex shader
#pragma pack_matrix(row_major)
cbuffer IntoGpu {
   matrix w, v, p;
};
float4 main(float3 inputVertex : POSITION) : SV_POSITION
{
	float4 output = float4(inputVertex, 1);
   return mul(mul(mul(output, w),v),p);
}
)";

// Simple Pixel Shader
const char* pixelShaderSource = R"(
// an ultra simple hlsl pixel shader
float4 main() : SV_TARGET 
{	
	return float4(0.25f,0.0f,1.0f,0);
}
)";

// Creation, Rendering & Cleanup
class Renderer {
  // proxy handles
  GW::SYSTEM::GWindow win;
  GW::GRAPHICS::GDirectX11Surface d3d;
  // Render Objects
  ArraysToGpu gpu_buffs;
  /*
  Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
  Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
  Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;
  */
  Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
  Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
  Microsoft::WRL::ComPtr<ID3D11InputLayout> vertexFormat;

  // Math
  GW::MATH::GMatrix cam_mat;
  SimpleMats shaderVars;

 public:
  Renderer(GW::SYSTEM::GWindow _win, GW::GRAPHICS::GDirectX11Surface _d3d) {
    // math
    cam_mat.Create();
    cam_mat.IdentityF(shaderVars.w);
    cam_mat.LookAtLHF(GW::MATH::GVECTORF{1, 0.5f, -2},
                      GW::MATH::GVECTORF{0, 0.25f, 0},
                      GW::MATH::GVECTORF{0, 1, 0}, shaderVars.v);
    float ar = 1;
    d3d.GetAspectRatio(ar);

    cam_mat.ProjectionDirectXLHF(G_DEGREE_TO_RADIAN(70), ar, 0.1f, 1000,
                                 shaderVars.p);
    // rest of setup.
    win = _win;
    d3d = _d3d;
    d3d.GetDevice((void**)&creator);

    UINT compilerFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if _DEBUG
    compilerFlags |= D3DCOMPILE_DEBUG;
#endif
    Microsoft::WRL::ComPtr<ID3DBlob> vsBlob, errors;
    /*
    // Create Vertex Shader
    if (SUCCEEDED(D3DCompileFromFile(
            L"../shader/vert_simple.hlsl", nullptr, nullptr, "main", "vs_4_0",
            compilerFlags, 0, vsBlob.GetAddressOf(), errors.GetAddressOf()))) {
      creator->CreateVertexShader(vsBlob->GetBufferPointer(),
                                  vsBlob->GetBufferSize(), nullptr,
                                  vertexShader.GetAddressOf());
    } else {
      std::cout << (char*)errors->GetBufferPointer() << std::endl;
    }
    // Create Pixel Shader
    Microsoft::WRL::ComPtr<ID3DBlob> psBlob;
    errors.Reset();
    if (SUCCEEDED(D3DCompileFromFile(
            L"../shader/pixl_simple.hlsl", nullptr, nullptr, "main", "ps_4_0",
            compilerFlags, 0, psBlob.GetAddressOf(), errors.GetAddressOf()))) {
      creator->CreatePixelShader(psBlob->GetBufferPointer(),
                                 psBlob->GetBufferSize(), nullptr,
                                 pixelShader.GetAddressOf());
    } else {
      std::cout << (char*)errors->GetBufferPointer() << std::endl;
    }
    */
    if (SUCCEEDED(D3DCompile(vertexShaderSource, strlen(vertexShaderSource),
                             nullptr, nullptr, nullptr, "main", "vs_4_0",
                             compilerFlags, 0, vsBlob.GetAddressOf(),
                             errors.GetAddressOf()))) {
      creator->CreateVertexShader(vsBlob->GetBufferPointer(),
                                  vsBlob->GetBufferSize(), nullptr,
                                  vertexShader.GetAddressOf());
    } else
      std::cout << (char*)errors->GetBufferPointer() << std::endl;
    // Create Pixel Shader
    Microsoft::WRL::ComPtr<ID3DBlob> psBlob;
    errors.Reset();
    if (SUCCEEDED(D3DCompile(pixelShaderSource, strlen(pixelShaderSource),
                             nullptr, nullptr, nullptr, "main", "ps_4_0",
                             compilerFlags, 0, psBlob.GetAddressOf(),
                             errors.GetAddressOf()))) {
      creator->CreatePixelShader(psBlob->GetBufferPointer(),
                                 psBlob->GetBufferSize(), nullptr,
                                 pixelShader.GetAddressOf());
    } else
      std::cout << (char*)errors->GetBufferPointer() << std::endl;

    // Create Input Layout
    D3D11_INPUT_ELEMENT_DESC format[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
         D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"UVW", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
         D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    creator->CreateInputLayout(
        format, ARRAYSIZE(format), vsBlob->GetBufferPointer(),
        vsBlob->GetBufferSize(), vertexFormat.GetAddressOf());
    // free temporary handle
    creator->Release();
  }
  void Render() {
    // grab the context & render target
    ID3D11DeviceContext* con;
    ID3D11RenderTargetView* view;
    d3d.GetImmediateContext((void**)&con);
    d3d.GetRenderTargetView((void**)&view);
    // setup the pipeline
    ID3D11RenderTargetView* const views[] = {view};
    con->OMSetRenderTargets(ARRAYSIZE(views), views, nullptr);
    const UINT strides[] = {sizeof(OBJ_VERT)};
    const UINT offsets[] = {0};
    // Set shaders.
    con->VSSetShader(vertexShader.Get(), nullptr, 0);
    con->PSSetShader(pixelShader.Get(), nullptr, 0);
    con->IASetInputLayout(vertexFormat.Get());
    // Send buffers to GPU.
    for (unsigned int i = 0; i < gpu_buffs.vertx_buffers.size(); i++) {
      // move pyramid
      // con->UpdateSubresource();
      ID3D11Buffer* const buffs[] = {gpu_buffs.vertx_buffers[i].Get()};
      con->IASetVertexBuffers(0, ARRAYSIZE(buffs), buffs, strides, offsets);
      con->IASetIndexBuffer(gpu_buffs.index_buffers[i].Get(),
                            DXGI_FORMAT_R32_UINT, 0);
      con->VSSetConstantBuffers(0, 1,
                                gpu_buffs.const_buffers[i].GetAddressOf());
    }
    // Draw.
    con->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    con->DrawIndexed(test_pyramid_vertexcount, 0, 0);
    // release temp handles
    view->Release();
    con->Release();
  }
  ~Renderer() {
    // ComPtr will auto release so nothing to do here
  }
};
