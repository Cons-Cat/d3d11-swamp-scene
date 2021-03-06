// minimalistic code to draw a single triangle, this is not part of the API.
// required for compiling shaders on the fly, consider pre-compiling instead
#include <d3dcompiler.h>

#include "asset/test_pyramid.h"
#include "shader/pixl_simple.h"
#include "shader/vert_simple.h"

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
  // what we need at a minimum to draw a triangle
  Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
  Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
  Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;
  Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
  Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
  Microsoft::WRL::ComPtr<ID3D11InputLayout> vertexFormat;
  // Math
  GW::MATH::GMatrix m;
  struct IntoGpu {
    GW::MATH::GMATRIXF w, v, p;
  };
  IntoGpu shaderVars;

 public:
  Renderer(GW::SYSTEM::GWindow _win, GW::GRAPHICS::GDirectX11Surface _d3d) {
    // math
    m.Create();
    m.IdentityF(shaderVars.w);
    m.LookAtLHF(GW::MATH::GVECTORF{1, 0.5f, -2},
                GW::MATH::GVECTORF{0, 0.25f, 0}, GW::MATH::GVECTORF{0, 1, 0},
                shaderVars.v);
    float ar = 1;
    d3d.GetAspectRatio(ar);

    m.ProjectionDirectXLHF(G_DEGREE_TO_RADIAN(70), ar, 0.1f, 1000,
                           shaderVars.p);
    // rest of setup.
    win = _win;
    d3d = _d3d;
    ID3D11Device* creator;
    d3d.GetDevice((void**)&creator);

    // Create Vertex Buffer
    D3D11_SUBRESOURCE_DATA bData = {test_pyramid_data, 0, 0};
    CD3D11_BUFFER_DESC bDesc(sizeof(test_pyramid_data),
                             D3D11_BIND_VERTEX_BUFFER);
    creator->CreateBuffer(&bDesc, &bData, vertexBuffer.GetAddressOf());

    // Create Index Buffer
    D3D11_SUBRESOURCE_DATA iData = {test_pyramid_indicies, 0, 0};
    CD3D11_BUFFER_DESC iDesc(sizeof(test_pyramid_indicies),
                             D3D11_BIND_INDEX_BUFFER);
    creator->CreateBuffer(&iDesc, &iData, indexBuffer.GetAddressOf());

    // Create Constant Buffer
    D3D11_SUBRESOURCE_DATA cData = {&shaderVars, 0, 0};
    CD3D11_BUFFER_DESC cDesc(sizeof(shaderVars), D3D11_BIND_CONSTANT_BUFFER);
    creator->CreateBuffer(&cDesc, &cData, constantBuffer.GetAddressOf());

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
    ID3D11Buffer* const buffs[] = {vertexBuffer.Get()};
    con->IASetVertexBuffers(0, ARRAYSIZE(buffs), buffs, strides, offsets);
    con->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

    con->VSSetShader(vertexShader.Get(), nullptr, 0);
    con->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
    con->PSSetShader(pixelShader.Get(), nullptr, 0);
    con->IASetInputLayout(vertexFormat.Get());
    // move pyramid
    // con->UpdateSubresource();

    // now we can draw
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
