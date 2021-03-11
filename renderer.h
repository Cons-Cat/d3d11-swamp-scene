// minimalistic code to draw a single triangle, this is not part of the API.
// required for compiling shaders on the fly, consider pre-compiling instead
#include <d3dcompiler.h>

#include "DDSTextureLoader.h"
#include "asset/test_pyramid.h"
//#include "input.h"
#include "shader/pixl_simple.h"
#include "shader/vert_simple.h"
#include "struct_of_arrays.h"

#pragma comment(lib, "d3dcompiler.lib")

// Simple Vertex Shader
const char* vertexShaderSource = R"(
#pragma pack_matrix(row_major)
cbuffer SimpleMats {
   matrix w, v, p;
};

struct SimpleOutVert {
   float2 uv : UVW;
   float3 nrm : NORMAL;
   float4 pos : SV_POSITION;
};

SimpleOutVert main(  float3 inputVertex : POSITION,
                     float3 texCoord : UVW,
                     float3 normal : NORMAL) {
   SimpleOutVert output;
   output.uv = texCoord.xy;
   output.nrm = mul(normal, w);
	float4 in_vert = float4(inputVertex, 1);
   output.pos = mul(mul(mul(in_vert, w),v),p);
   return output;
}
)";

// Simple Pixel Shader
const char* pixelShaderSource = R"(
Texture2D inTex;
SamplerState sam;

float4 main(float2 uv : TEXTURE,
            float3 nrm : NORMAL) : SV_TARGET {
   float4 color = inTex.Sample(sam, uv);

   // Day 7 vid
   float3 lightDirWorld = {-1,-1, 1};
   float lightRatio = dot(-lightDirWorld, normalize(nrm));
	return color * lightRatio;
}
)";

// Creation, Rendering & Cleanup
class Renderer {
  // proxy handles
  GW::SYSTEM::GWindow win;
  GW::GRAPHICS::GDirectX11Surface d3d;
  // Render Objects
  ArraysToGpu gpu_buffs;
  Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
  Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
  Microsoft::WRL::ComPtr<ID3D11InputLayout> vertexFormat;
  // Texture
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderView;

 public:
  Renderer(GW::SYSTEM::GWindow _win, GW::GRAPHICS::GDirectX11Surface _d3d) {
    // rest of setup.
    win = _win;
    d3d = _d3d;
    d3d.GetDevice((void**)&creator);
    INPUTTER::init_camera(d3d);
    // Load texture
    HRESULT hr = CreateDDSTextureFromFile(creator, L"../asset/my_face.dds",
                                          nullptr, shaderView.GetAddressOf());

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
    // Push render objects.
    gpu_buffs.PushNewModel(test_pyramid_data, test_pyramid_vertexcount,
                           test_pyramid_indicies, test_pyramid_indexcount,
                           INPUTTER::camera);
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
    // Temp: Set texture
    ID3D11ShaderResourceView* const srvs[] = {shaderView.Get()};
    con->PSSetShaderResources(0, 1, srvs);
    // Send buffers to GPU.
    for (unsigned int i = 0; i < 1; i++) {
      // for (unsigned int i = 0; i < gpu_buffs.vertx_buffers.size(); i++) {
      con->VSSetConstantBuffers(0, 1,
                                gpu_buffs.const_buffers[i].GetAddressOf());
      ID3D11Buffer* const vbuff[] = {gpu_buffs.vertx_buffers[i].Get()};
      con->IASetVertexBuffers(0, ARRAYSIZE(vbuff), vbuff, strides, offsets);
      con->IASetIndexBuffer(gpu_buffs.index_buffers[i].Get(),
                            DXGI_FORMAT_R32_UINT, 0);
      // Move
      /*
      if (true) {
        cam_mat.RotationYF(INPUTTER::camera.w, 0.01f, INPUTTER::camera.w);
        con->UpdateSubresource(gpu_buffs.const_buffers[i].Get(), 0, nullptr,
                               static_cast<void*>(&INPUTTER::camera),
                               sizeof(SimpleMats), 0);
      }
      */
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
