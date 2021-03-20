// minimalistic code to draw a single triangle, this is not part of the API.
// required for compiling shaders on the fly, consider pre-compiling instead
#include <d3d11_1.h>
#include <d3dcompiler.h>

#include "DDSTextureLoader.h"
#include "asset/test_pyramid.h"
#include "asset/willow.h"
#include "skybox.h"

#pragma comment(lib, "d3dcompiler.lib")

// Simple Vertex Shader
const char* vertexShaderSource = R"(
#pragma pack_matrix(row_major)
cbuffer IntoGpu {
   matrix w, v, p;
};

struct OUT_VERT
{
   float2 uv : TEXTURE;
   float3 nrm : NORMAL;
   float4 pos : SV_POSITION;
};

OUT_VERT main(float3 inputVertex : POSITION,
              float3 texCoord : UVW,
              float3 normal : NORMAL)
{
   OUT_VERT output;
	output.pos = float4(inputVertex, 1);
   output.pos = mul(mul(mul(output.pos, w),v),p);
   output.uv = texCoord.xy;
   output.nrm = mul(normal, w);
   return output;
}
)";

// Simple Instanced Vertex Shader
const char* instancedVertexShaderSource = R"(
#pragma pack_matrix(row_major)
cbuffer IntoGpu {
   matrix w, v, p;
};

struct OUT_VERT
{
   float2 uv : TEXTURE;
   float3 nrm : NORMAL;
   float4 pos : SV_POSITION;
};

OUT_VERT main(float3 inputVertex : POSITION,
              float3 texCoord : UVW,
              float3 normal : NORMAL,
              float3 inst_pos : INSTANCEPOS
)
{
   OUT_VERT output;
   float4 inst_pos4 = float4(inst_pos, 1);
	output.pos = float4(inputVertex, 1);
   // Moving spaces
   output.pos = mul(output.pos, w);
   output.pos.x += inst_pos.x;
   output.pos.y += inst_pos.y;
   output.pos.z += inst_pos.z;
   output.pos = mul(mul(output.pos,v),p);
   output.uv = texCoord.xy;
   output.nrm = mul(normal, w);
   return output;
}
)";

// Simple Pixel Shader
const char* pixelShaderSource = R"(
Texture2D in_tex;
SamplerState sam;

float4 main(float2 uv : TEXTURE,
            float3 nrm : NORMAL) : SV_TARGET
{
   float4 color = in_tex.Sample(sam, uv);
   if (color.a == 0) {
      discard;
      return 0;
   } else {
      float temp_alpha = color.a;
      float3 lightDir = { -1, -1, 1 };
      lightDir = normalize(lightDir);
      float lightR = dot(-lightDir, normalize(nrm));
      color = color * lightR;
      color.a = temp_alpha;
      return color;
   }
}
)";

// Creation, Rendering & Cleanup
class Renderer {
  // proxy handles
  GW::SYSTEM::GWindow win;
  GW::GRAPHICS::GDirectX11Surface d3d;
  // Rendering
  Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;
  Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
  Microsoft::WRL::ComPtr<ID3D11VertexShader> instancedVertexShader;
  Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
  Microsoft::WRL::ComPtr<ID3D11InputLayout> vertexFormat;
  ID3D11BlendState* blendState;

  // Loading models
  struct OBJ_MESH {
    // Drawing data
    unsigned int index_count;
    GW::MATH::GMATRIXF world;
    // Loading data
    Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> diffuse;
  };

  struct INSTANCED_MESH {
    Microsoft::WRL::ComPtr<ID3D11Buffer> instanceBuffer;
    unsigned int instance_count;
    OBJ_MESH obj_mesh;
  };

  struct INSTANCES_POSITION_BUFFER {
    float position[3];
  };

  // Math
  GW::MATH::GMatrix m;
  struct IntoGpu {
    GW::MATH::GMATRIXF w, v, p;
  };
  IntoGpu shaderVars;

  // Make objects for models here.
  OBJ_MESH cat_pyramid;
  INSTANCED_MESH willows;
  std::vector<INSTANCES_POSITION_BUFFER> willows_instances;

 public:
  OBJ_MESH LoadObjMesh(const OBJ_VERT* verts, unsigned num_verts,
                       const unsigned* indicies, unsigned num_index,
                       const wchar_t* diffuse_name) {
    // Get device for loading
    ID3D11Device* creator;
    d3d.GetDevice((void**)&creator);
    // Initialize model
    OBJ_MESH outModel;
    outModel.world = GW::MATH::GIdentityMatrixF;
    outModel.index_count = num_index;
    // Load in texture
    HRESULT hr_tex = CreateDDSTextureFromFile(creator, diffuse_name, nullptr,
                                              outModel.diffuse.GetAddressOf());
    // Create Vertex Buffer
    D3D11_SUBRESOURCE_DATA bData = {verts, 0, 0};
    CD3D11_BUFFER_DESC bDesc(sizeof(OBJ_VERT) * num_verts,
                             D3D11_BIND_VERTEX_BUFFER);
    creator->CreateBuffer(&bDesc, &bData, outModel.vertexBuffer.GetAddressOf());
    // Create Index Buffer
    D3D11_SUBRESOURCE_DATA iData = {indicies, 0, 0};
    CD3D11_BUFFER_DESC iDesc(sizeof(unsigned) * num_index,
                             D3D11_BIND_INDEX_BUFFER);
    creator->CreateBuffer(&iDesc, &iData, outModel.indexBuffer.GetAddressOf());
    // Free
    creator->Release();
    return outModel;
  }

  void DrawObjMesh(const OBJ_MESH& drawMe, size_t index_count,
                   size_t index_offset) {
    ID3D11DeviceContext* con;
    d3d.GetImmediateContext((void**)&con);
    // Update world matrix
    shaderVars.w = drawMe.world;
    con->UpdateSubresource(constantBuffer.Get(), 0, nullptr,
                           static_cast<void*>(&shaderVars), sizeof(IntoGpu), 0);
    // Setup pipeline for this mesh
    const UINT strides[] = {sizeof(OBJ_VERT)};
    const UINT offsets[] = {0};
    ID3D11Buffer* const buffs[] = {drawMe.vertexBuffer.Get()};
    con->IASetVertexBuffers(0, ARRAYSIZE(buffs), buffs, strides, offsets);
    con->IASetIndexBuffer(drawMe.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    // Set texture
    ID3D11ShaderResourceView* const srvs[] = {drawMe.diffuse.Get()};
    con->PSSetShaderResources(0, 1, srvs);
    // now we can draw
    con->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    con->DrawIndexed(index_count, index_offset, 0);
    // Release reference count
    con->Release();
  }

  void DrawObjSubmesh(const OBJ_MESH& subMesh) {
    //
  }

  void Update() {
    // Update a mesh
    // m.RotationYF(cat_pyramid.world, 0.01f, cat_pyramid.world);

    // Look camera
    m.InverseF(shaderVars.v, shaderVars.v);
    auto camera_pos = shaderVars.v.row4;
    m.InverseF(shaderVars.v, shaderVars.v);
    shaderVars.v.row4 = GW::MATH::GZeroVectorF;
    shaderVars.v.row4.w = 1;
    m.InverseF(shaderVars.v, INPUTTER::camera);
    INPUTTER::look_camera();
    shaderVars.v = INPUTTER::camera;
    m.InverseF(shaderVars.v, shaderVars.v);
    shaderVars.v.row4 = camera_pos;
    m.InverseF(shaderVars.v, shaderVars.v);

    // Walk camera
#define EVAL_INPUT(DIR)                               \
  float o_##DIR## = 0;                                \
  INPUTTER::input.GetState(G_KEY_##DIR##, o_##DIR##); \
  float is_##DIR## = (o_##DIR## == 0 ? 0 : 1.0f);

    EVAL_INPUT(UP);
    EVAL_INPUT(DOWN);
    EVAL_INPUT(LEFT);
    EVAL_INPUT(RIGHT);
    float z = INPUTTER::walk_speed * (is_UP - is_DOWN);
    float x = INPUTTER::walk_speed * (is_RIGHT - is_LEFT);
    m.InverseF(shaderVars.v, INPUTTER::camera);
    m.TranslatelocalF(INPUTTER::camera, GW::MATH::GVECTORF{x, 0, z},
                      INPUTTER::camera);
    m.InverseF(INPUTTER::camera, shaderVars.v);
  }

  Renderer(GW::SYSTEM::GWindow _win, GW::GRAPHICS::GDirectX11Surface _d3d) {
    // math
    m.Create();
    m.IdentityF(shaderVars.w);

    // rest of setup.
    win = _win;
    d3d = _d3d;
    ID3D11Device* creator;
    d3d.GetDevice((void**)&creator);

    // Camera
    float ar;
    d3d.GetAspectRatio(ar);
    m.ProjectionDirectXLHF(G_DEGREE_TO_RADIAN(70), ar, 0.1f, 1000,
                           shaderVars.p);
    INPUTTER::init_camera(ar, win);

    m.LookAtLHF(GW::MATH::GVECTORF{1, 1, -2}, GW::MATH::GVECTORF{0, 1, 1},
                GW::MATH::GVECTORF{0, 1, 0}, shaderVars.v);
    m.RotationYawPitchRollF(0, 0, 3.14f, shaderVars.v);

    D3D11_BLEND_DESC blendStateDesc;
    ZeroMemory(&blendStateDesc, sizeof(D3D11_BLEND_DESC));
    blendStateDesc.AlphaToCoverageEnable = TRUE;
    blendStateDesc.IndependentBlendEnable = TRUE;
    blendStateDesc.RenderTarget[0].BlendEnable = TRUE;
    blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
    blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendStateDesc.RenderTarget[0].RenderTargetWriteMask =
        D3D11_COLOR_WRITE_ENABLE_ALL;

    if (FAILED(creator->CreateBlendState(&blendStateDesc, &blendState))) {
      printf("Failed To Create Blend State\n");
    }

    // Load the pyramid
    cat_pyramid = LoadObjMesh(test_pyramid_data, test_pyramid_vertexcount,
                              test_pyramid_indicies, test_pyramid_indexcount,
                              L"../asset/my_face.dds");

    // Load the willow mesh
    willows.obj_mesh =
        LoadObjMesh(willow_data, willow_vertexcount, willow_indicies,
                    willow_indexcount, L"../asset/treeWillow_Trunk_D.dds");
    m.TranslatelocalF(willows.obj_mesh.world, GW::MATH::GVECTORF{0, 0, 10.0f},
                      willows.obj_mesh.world);
    m.ScalingF(willows.obj_mesh.world, GW::MATH::GVECTORF{0.1f, 0.1f, 0.1f},
               willows.obj_mesh.world);

    // Instance some willows
    D3D11_BUFFER_DESC instanceBufferDesc;
    D3D11_SUBRESOURCE_DATA instanceData;

    instanceBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    instanceBufferDesc.CPUAccessFlags = 0;
    instanceBufferDesc.MiscFlags = 0;
    // Make willows data
    willows_instances.push_back({0, 0, 40});
    willows_instances.push_back({-5, 0, 10});
    willows.instance_count = willows_instances.size();
    instanceBufferDesc.ByteWidth =
        sizeof(INSTANCES_POSITION_BUFFER) * willows.instance_count;
    instanceData.pSysMem = &willows_instances[0];
    // Make instance buffer
    creator->CreateBuffer(&instanceBufferDesc, &instanceData,
                          &willows.instanceBuffer);

    // Create Constant Buffer
    D3D11_SUBRESOURCE_DATA cData = {&shaderVars, 0, 0};
    CD3D11_BUFFER_DESC cDesc(sizeof(shaderVars), D3D11_BIND_CONSTANT_BUFFER);
    creator->CreateBuffer(&cDesc, &cData, constantBuffer.GetAddressOf());

    // Create Vertex Shaders
    UINT compilerFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if _DEBUG
    compilerFlags |= D3DCOMPILE_DEBUG;
#endif
    Microsoft::WRL::ComPtr<ID3DBlob> vsBlob, errors;
    if (SUCCEEDED(D3DCompile(vertexShaderSource, strlen(vertexShaderSource),
                             nullptr, nullptr, nullptr, "main", "vs_4_0",
                             compilerFlags, 0, vsBlob.GetAddressOf(),
                             errors.GetAddressOf()))) {
      creator->CreateVertexShader(vsBlob->GetBufferPointer(),
                                  vsBlob->GetBufferSize(), nullptr,
                                  vertexShader.GetAddressOf());
    } else
      std::cout << (char*)errors->GetBufferPointer() << std::endl;

    if (SUCCEEDED(D3DCompile(
            instancedVertexShaderSource, strlen(instancedVertexShaderSource),
            nullptr, nullptr, nullptr, "main", "vs_4_0", compilerFlags, 0,
            vsBlob.GetAddressOf(), errors.GetAddressOf()))) {
      creator->CreateVertexShader(vsBlob->GetBufferPointer(),
                                  vsBlob->GetBufferSize(), nullptr,
                                  instancedVertexShader.GetAddressOf());
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
        // Vertex buffer
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
         D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"UVW", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
         D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},

        // Instance buffer
        {"INSTANCEPOS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0,
         D3D11_INPUT_PER_INSTANCE_DATA, 1},
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
    ID3D11DepthStencilView* depth;
    d3d.GetImmediateContext((void**)&con);
    d3d.GetRenderTargetView((void**)&view);
    d3d.GetDepthStencilView((void**)&depth);

    // setup the pipeline
    ID3D11RenderTargetView* const views[] = {view};
    con->OMSetRenderTargets(ARRAYSIZE(views), views, depth);
    con->OMSetBlendState(blendState, 0, 0xFFFFFF);

    con->VSSetShader(vertexShader.Get(), nullptr, 0);
    con->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
    con->PSSetShader(pixelShader.Get(), nullptr, 0);
    con->IASetInputLayout(vertexFormat.Get());

    // move pyramid
    DrawObjMesh(cat_pyramid, test_pyramid_indexcount, 0);

    // Set texture
    ID3D11ShaderResourceView* const srvs[] = {willows.obj_mesh.diffuse.Get()};
    con->PSSetShaderResources(0, 1, srvs);

    // Instanced draw
    con->VSSetShader(instancedVertexShader.Get(), nullptr, 0);
    con->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    const UINT strides[2] = {sizeof(OBJ_VERT), sizeof(float) * 3};
    const UINT offsets[2] = {0, 0};

    ID3D11Buffer* willow_instance_buffers[2] = {
        willows.obj_mesh.vertexBuffer.Get(), willows.instanceBuffer.Get()};
    con->IASetIndexBuffer(willows.obj_mesh.indexBuffer.Get(),
                          DXGI_FORMAT_R32_UINT, 0);
    con->IASetVertexBuffers(0, 2, willow_instance_buffers, strides, offsets);
    con->DrawIndexedInstanced(willow_indexcount, willows.instance_count, 0, 0,
                              0);

    // release temp handles
    view->Release();
    depth->Release();
    con->Release();
  }
  ~Renderer() {
    // ComPtr will auto release so nothing to do here
  }
};
