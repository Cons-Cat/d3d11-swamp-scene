// minimalistic code to draw a single triangle, this is not part of the API.
// required for compiling shaders on the fly, consider pre-compiling instead
#include <d3d11_1.h>
#include <d3dcompiler.h>

#include "DDSTextureLoader.h"
#include "asset/fountain.h"
#include "asset/inverse_box.h"
#include "asset/my_ground.h"
#include "asset/test_pyramid.h"
#include "asset/willow.h"

#pragma comment(lib, "d3dcompiler.lib")

// --------------------------------
// Vertex Shaders

// Simple Shader
#pragma region
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
#pragma endregion

// Ground Shader
#pragma region
const char* groundVertexShaderSource = R"(
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
   if (output.pos.x * output.pos.x + output.pos.z * output.pos.z > 80) {
      output.pos.y -= (sin(output.pos.x/2) * sin(output.pos.z/2)) / 1.5 + 0.5;
      if (output.pos.y > 0) {
         output.pos *= 10.5f;
      }
   }
   output.pos = mul(mul(mul(output.pos, w),v),p);
   output.uv = texCoord.xy;
   output.nrm = mul(normal, w);
   return output;
}
)";
#pragma endregion

// Skybox Shader
#pragma region
const char* skyVertexShaderSource = R"(
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
   output.nrm = output.pos;
   output.pos = mul(mul(mul(output.pos, w),v),p);
   output.uv = texCoord.xy;
   //output.nrm = mul(normal, w);
   return output;
}
)";
#pragma endregion

// Grass Shader
#pragma region
const char* grassVertexShaderSource = R"(
#pragma pack_matrix(row_major)
struct GRASS_VERT
{
   float4 posW_s : SV_POSITION;
};

GRASS_VERT main(float3 posW_s : POSITION)
{
   GRASS_VERT output = { posW_s, 1 }; // if this syntax doesn't work just set the members directly
   return output;
}
)";
#pragma endregion

// Instanced Shader
#pragma region
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
#pragma endregion

// --------------------------------
// Pixel Shaders

// Simple Shader
#pragma region
const char* pixelShaderSource = R"(
Texture2D in_tex;
SamplerState sam;

float4 main(float2 uv : TEXTURE,
            float3 nrm : NORMAL
) : SV_TARGET
{
	float4 color = in_tex.Sample(sam, uv);
	if (color.a == 0)
	{
		discard;
		return 0;
	}
	else
	{
		float temp_alpha = color.a;
		float3 lightDir = { -1, -1, 1 };
		lightDir = normalize(lightDir);
		float lightR = dot(-lightDir, normalize(nrm));
		color = color * (lightR + color.a) / 2;
		color.a = temp_alpha;
		return color;
	}
}
)";
#pragma endregion

// Normal Shader
#pragma region
const char* normalPixelShaderSource = R"(
Texture2D in_tex;
Texture2D in_norm;
SamplerState sam;

float4 main(float2 uv : TEXTURE,
            float3 nrm : NORMAL
) : SV_TARGET
{
	float4 color = in_tex.Sample(sam, uv);
	float4 pxnrm = in_norm.Sample(sam, uv);
	if (color.a == 0)
	{
		discard;
		return 0;
	}
	else
	{
		float temp_alpha = color.a;
		float3 lightDir = { -1, -1, 1 };
		lightDir = normalize(lightDir);
      
      float3 normalT = 2 * pxnrm - 1;
      float3 n = (nrm);
      float3 t = -normalize(tan(nrm) - dot(tan(nrm),n) * n);
      float3 b = cross(n, t);
      float3x3 tbn = float3x3(t,b,n);
      float3 nrm = mul(normalT, tbn);
		float lightR = dot(-lightDir, normalize(nrm));
		color = color * (lightR + color.a) / 2;
		color.a = temp_alpha;
		return color;
	}
}
)";
#pragma endregion

// Grass Shader
#pragma region
const char* grassPixelShaderSource = R"(
//SamplerState sam;

float4 main(
            float4 clr : COLOR,
            float4 pos : SV_POSITION
) : SV_TARGET
{
   return clr;
}
)";
#pragma endregion

// Skybox Shader
#pragma region

const char* skyPixelShaderSource = R"(
#pragma target 4.0

TextureCube in_tex;
SamplerState sam;

float4 main(float2 uv : TEXTURE,
            float3 nrm : NORMAL,
            float4 pos : SV_POSITION
) : SV_TARGET
{
   float4 color = in_tex.Sample(sam, nrm.xyz);
   return color;
}
)";
#pragma endregion

// Simple Shader
#pragma region
const char* tiledPixelShaderSource = R"(
Texture2D in_tex;
SamplerState sam;

float4 main(float2 uv : TEXTURE,
            float3 nrm : NORMAL
) : SV_TARGET
{
	float4 color = in_tex.Sample(sam, uv % 512);
	float temp_alpha = color.a;
	float3 lightDir = { -1, -1, 1 };
	lightDir = normalize(lightDir);
	float lightR = dot(-lightDir, normalize(nrm));
	color = color * (lightR + color.a) / 2;
	color.a = temp_alpha;
   return color;
}
)";
#pragma endregion

// --------------------------------
// Geometry Shaders

// Grass Shader
#pragma region
const char* grassGeoShaderSource = R"(
#pragma pack_matrix(row_major)
struct GSOutput
{
   float4 clr : COLOR;
   float4 posH : SV_POSITION;
};

cbuffer IntoGpu {
   matrix w, v, p;
};

struct GRASS_VERT
{
   float4 posW_s : SV_POSITION;
};

[maxvertexcount(3)]
void main (point GRASS_VERT input[1], inout TriangleStream<GSOutput> output)
{
   // Cull grass
	float dotv;
	float3 view_dir = float3(v._41,v._42,v._43);
	dotv = dot(input[0].posW_s.xyz, view_dir);
   float range = 10;

	//if (dotv > -range && dotv < range)
	{
      float blade_height = 1.5;
      float half_bh = blade_height * 0.22;
   
      GSOutput simple[3];

      simple[0].clr = float4(0.1f, 0.6f, 0.1f, 1);
      simple[1].clr = float4(0, 1, 0, 1);
      simple[2].clr = simple[0].clr;
   
      simple[0].posH = float4(input[0].posW_s.xyz, 1);
      simple[1].posH = simple[0].posH;
      simple[2].posH = simple[0].posH;

      simple[0].posH.x -= half_bh;
      simple[1].posH.y += blade_height;
      simple[2].posH.x += half_bh;

      for (uint i = 0; i < 3; ++i) {
         simple[i].posH = mul(simple[i].posH, v);
         simple[i].posH = mul(simple[i].posH, p);
         output.Append(simple[i]);
      }
   }
}
)";
#pragma endregion

// --------------------------------

// Creation, Rendering & Cleanup
class Renderer {
  // proxy handles
  GW::SYSTEM::GWindow win;
  GW::GRAPHICS::GDirectX11Surface d3d;
  // Rendering
  Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;
  Microsoft::WRL::ComPtr<ID3D11InputLayout> vertexFormat;
  Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
  Microsoft::WRL::ComPtr<ID3D11VertexShader> groundVertexShader;
  Microsoft::WRL::ComPtr<ID3D11InputLayout> instancedVertexFormat;
  Microsoft::WRL::ComPtr<ID3D11VertexShader> instancedVertexShader;
  Microsoft::WRL::ComPtr<ID3D11VertexShader> skyVertexShader;
  Microsoft::WRL::ComPtr<ID3D11VertexShader> grassVertexShader;
  Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
  Microsoft::WRL::ComPtr<ID3D11PixelShader> skyPixelShader;
  Microsoft::WRL::ComPtr<ID3D11PixelShader> grassPixelShader;
  Microsoft::WRL::ComPtr<ID3D11GeometryShader> grassGeoShader;
  Microsoft::WRL::ComPtr<ID3D11Buffer> grassPositions;
  Microsoft::WRL::ComPtr<ID3D11InputLayout> grassVertexFormat;
  Microsoft::WRL::ComPtr<ID3D11PixelShader> normalPixelShader;
  ID3D11BlendState* blendState;

  // Skybox
  Microsoft::WRL::ComPtr<ID3D11Texture2D> environmentTexture;
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> environmentView;
  float near_plane = 0.1f;
  float far_plane = 1000.0f;

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

  struct NORM_MESH {
    // Drawing data
    unsigned int index_count;
    GW::MATH::GMATRIXF world;
    // Loading data
    Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> diffuse;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normal;
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
  OBJ_MESH inverse_box;
  OBJ_MESH ground;
  NORM_MESH fountain;
  INSTANCED_MESH willows;
  std::vector<INSTANCES_POSITION_BUFFER> willows_instances;

  // Geometry shader
  unsigned int grass_count;
  float blade_height = 10.0f;

  struct GRASS_POS {
    float x;
    float y;
    float z;
  };

  std::vector<GRASS_POS> grass_positions;

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

  NORM_MESH LoadNormMesh(const OBJ_VERT* verts, unsigned num_verts,
                         const unsigned* indicies, unsigned num_index,
                         const wchar_t* diffuse_name,
                         const wchar_t* normal_name) {
    // Get device for loading
    ID3D11Device* creator;
    d3d.GetDevice((void**)&creator);
    // Initialize model
    NORM_MESH outModel;
    outModel.world = GW::MATH::GIdentityMatrixF;
    outModel.index_count = num_index;
    // Load in texture
    CreateDDSTextureFromFile(creator, diffuse_name, nullptr,
                             outModel.diffuse.GetAddressOf());
    CreateDDSTextureFromFile(creator, normal_name, nullptr,
                             outModel.normal.GetAddressOf());
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

  void DrawNormalObjMesh(const NORM_MESH& drawMe, size_t index_count,
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
    ID3D11ShaderResourceView* const srvs[] = {drawMe.diffuse.Get(),
                                              drawMe.normal.Get()};
    con->PSSetShaderResources(0, 2, srvs);
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
    m.ProjectionDirectXLHF(G_DEGREE_TO_RADIAN(70), ar, near_plane, far_plane,
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

    // Load the skybox
    inverse_box = LoadObjMesh(inverse_box_data, inverse_box_vertexcount,
                              inverse_box_indicies, inverse_box_indexcount,
                              L"../asset/skybox.dds");
    m.ScalingF(inverse_box.world,
               GW::MATH::GVECTORF{far_plane, far_plane, far_plane, 1},
               inverse_box.world);

    // Load the pyramid
    cat_pyramid = LoadObjMesh(test_pyramid_data, test_pyramid_vertexcount,
                              test_pyramid_indicies, test_pyramid_indexcount,
                              L"../asset/my_face.dds");

    // Load the ground
    ground = LoadObjMesh(my_ground_data, my_ground_vertexcount,
                         my_ground_indicies, my_ground_indexcount,
                         L"../asset/brownishDirt_seamless.dds");

    // Load the willow mesh
    willows.obj_mesh =
        LoadObjMesh(willow_data, willow_vertexcount, willow_indicies,
                    willow_indexcount, L"../asset/treeWillow_Trunk_D.dds");
    m.TranslatelocalF(willows.obj_mesh.world, GW::MATH::GVECTORF{0, 0, 10.0f},
                      willows.obj_mesh.world);
    m.ScalingF(willows.obj_mesh.world, GW::MATH::GVECTORF{0.1f, 0.1f, 0.1f},
               willows.obj_mesh.world);

    // Load the fountain
    fountain =
        LoadNormMesh(fountain_data, fountain_vertexcount, fountain_indicies,
                     fountain_indexcount, L"../asset/T_Fountain3_D.dds",
                     L"../asset/T_Fountain3_N.dds");

    // Instance some willows
    D3D11_BUFFER_DESC instanceBufferDesc;
    D3D11_SUBRESOURCE_DATA instanceData;

    instanceBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    instanceBufferDesc.CPUAccessFlags = 0;
    instanceBufferDesc.MiscFlags = 0;
    // Make willows data
    float world_size = 200;
    float min_dist = 80;
    for (size_t i = 0; i < 430; i++) {
      // Distribute trees far away from {0,0,0}
      float rng_m =
          (float(rand()) / float((RAND_MAX)) + (min_dist / world_size)) /
          (min_dist + world_size) * world_size;
      float rng_t = float(rand()) / float((RAND_MAX)) * 3.15 * 2;

      float weight_m =
          (sin(rng_m * (3.14f / 4) * 2) + (min_dist / world_size)) /
          ((min_dist + world_size) / world_size);

      float m = (rng_m * weight_m * world_size / 2);
      float x = cos(rng_t) * m;
      float z = sin(rng_t) * m;

      if (x > -20 && x < 20 && z > -20 && z < 20) {
        if (rand() % 20 != 0 || m < min_dist * 2) {
          // Make a path.
          continue;
        }
      }

      willows_instances.push_back({x, 0, z});
    }
    willows.instance_count = willows_instances.size();
    instanceBufferDesc.ByteWidth =
        sizeof(INSTANCES_POSITION_BUFFER) * willows.instance_count;
    instanceData.pSysMem = &willows_instances[0];
    // Make instance buffer
    creator->CreateBuffer(&instanceBufferDesc, &instanceData,
                          &willows.instanceBuffer);

    // Create grass
    D3D11_SUBRESOURCE_DATA grassInstanceDataPos;

    for (float i = -200; i < 200; i += 0.5f) {
      for (float j = -200; j < 200; j += 0.5f) {
        if (i * i + j * j > 15 * 15) {
          if (rand() % 3 != 0) {
            float k = float(rand()) / float(RAND_MAX) - 1;
            grass_positions.push_back({i, k, j});
          }
        }
      }
    }

    grass_count = grass_positions.size();
    instanceBufferDesc.ByteWidth = sizeof(GRASS_POS) * grass_count;
    grassInstanceDataPos.pSysMem = &grass_positions[0];
    creator->CreateBuffer(&instanceBufferDesc, &grassInstanceDataPos,
                          grassPositions.GetAddressOf());

    // Create Constant Buffer
    D3D11_SUBRESOURCE_DATA cData = {&shaderVars, 0, 0};
    CD3D11_BUFFER_DESC cDesc(sizeof(shaderVars), D3D11_BIND_CONSTANT_BUFFER);
    creator->CreateBuffer(&cDesc, &cData, constantBuffer.GetAddressOf());

    UINT compilerFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if _DEBUG
    compilerFlags |= D3DCOMPILE_DEBUG;
#endif

    Microsoft::WRL::ComPtr<ID3DBlob> errors;

    // Create Vertex Shaders
#define LoadVertShader(shdname)                                              \
  Microsoft::WRL::ComPtr<ID3DBlob>##shdname##Blob;                           \
  D3DCompile(##shdname##Source, strlen(##shdname##Source), nullptr, nullptr, \
             nullptr, "main", "vs_4_0", compilerFlags, 0,                    \
             ##shdname##Blob.GetAddressOf(), errors.GetAddressOf());         \
  creator->CreateVertexShader(##shdname##Blob->GetBufferPointer(),           \
                              ##shdname##Blob->GetBufferSize(), nullptr,     \
                              ##shdname##.GetAddressOf());                   \
  errors.Reset();

    LoadVertShader(vertexShader);
    LoadVertShader(skyVertexShader);
    LoadVertShader(instancedVertexShader);
    LoadVertShader(grassVertexShader);
    LoadVertShader(groundVertexShader);

    // Create Pixel Shaders
#define LoadPixShader(shdname)                                               \
  Microsoft::WRL::ComPtr<ID3DBlob>##shdname##Blob;                           \
  D3DCompile(##shdname##Source, strlen(##shdname##Source), nullptr, nullptr, \
             nullptr, "main", "ps_4_0", compilerFlags, 0,                    \
             ##shdname##Blob.GetAddressOf(), errors.GetAddressOf());         \
  creator->CreatePixelShader(##shdname##Blob->GetBufferPointer(),            \
                             ##shdname##Blob->GetBufferSize(), nullptr,      \
                             ##shdname##.GetAddressOf());                    \
  errors.Reset();

    LoadPixShader(pixelShader);
    LoadPixShader(skyPixelShader);
    LoadPixShader(normalPixelShader);
    LoadPixShader(grassPixelShader);

    // Create Geometry Shaders
#define LoadGeoShader(shdname)                                               \
  Microsoft::WRL::ComPtr<ID3DBlob>##shdname##Blob;                           \
  D3DCompile(##shdname##Source, strlen(##shdname##Source), nullptr, nullptr, \
             nullptr, "main", "gs_4_0", compilerFlags, 0,                    \
             ##shdname##Blob.GetAddressOf(), errors.GetAddressOf());         \
  creator->CreateGeometryShader(##shdname##Blob->GetBufferPointer(),         \
                                ##shdname##Blob->GetBufferSize(), nullptr,   \
                                ##shdname##.GetAddressOf());                 \
  errors.Reset();

    LoadGeoShader(grassGeoShader);

    // Create Simple Input Layout
    D3D11_INPUT_ELEMENT_DESC format[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
         D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"UVW", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
         D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    creator->CreateInputLayout(
        format, ARRAYSIZE(format), vertexShaderBlob->GetBufferPointer(),
        vertexShaderBlob->GetBufferSize(), vertexFormat.GetAddressOf());

    // Create Instanced Input Layout
    D3D11_INPUT_ELEMENT_DESC instanced_format[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
         D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"UVW", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
         D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},

        {"INSTANCEPOS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0,
         D3D11_INPUT_PER_INSTANCE_DATA, 1},
    };

    creator->CreateInputLayout(instanced_format, ARRAYSIZE(instanced_format),
                               instancedVertexShaderBlob->GetBufferPointer(),
                               instancedVertexShaderBlob->GetBufferSize(),
                               instancedVertexFormat.GetAddressOf());

    // Create Grass Input Layout
    D3D11_INPUT_ELEMENT_DESC grass_format[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
         D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    creator->CreateInputLayout(grass_format, ARRAYSIZE(grass_format),
                               grassVertexShaderBlob->GetBufferPointer(),
                               grassVertexShaderBlob->GetBufferSize(),
                               grassVertexFormat.GetAddressOf());

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
    con->GSSetShader(nullptr, 0, 0);

    con->IASetInputLayout(vertexFormat.Get());

    // Ground
    con->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
    con->VSSetShader(groundVertexShader.Get(), nullptr, 0);
    con->PSSetShader(pixelShader.Get(), nullptr, 0);
    DrawObjMesh(ground, my_ground_indexcount, 0);

    // Skybox
    con->VSSetShader(skyVertexShader.Get(), nullptr, 0);
    con->PSSetShader(skyPixelShader.Get(), nullptr, 0);
    ID3D11ShaderResourceView* const sky_tex[] = {environmentView.Get()};
    con->PSSetShaderResources(0, 1, sky_tex);
    DrawObjMesh(inverse_box, inverse_box_indexcount, 0);

    // Fountain
    con->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
    con->VSSetShader(vertexShader.Get(), nullptr, 0);
    con->PSSetShader(normalPixelShader.Get(), nullptr, 0);

    DrawNormalObjMesh(fountain, fountain_indexcount, 0);

    // TODO: Abstract instanced mesh function.
    // Set texture
    ID3D11ShaderResourceView* const srvs[] = {willows.obj_mesh.diffuse.Get()};
    con->PSSetShaderResources(0, 1, srvs);

    // Instanced draw
    con->VSSetShader(instancedVertexShader.Get(), nullptr, 0);
    con->PSSetShader(pixelShader.Get(), nullptr, 0);
    con->IASetInputLayout(instancedVertexFormat.Get());
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

    // Grass geometry
    const unsigned int grass_offsets[] = {0};
    const unsigned int grass_strides[] = {sizeof(GRASS_POS)};
    ID3D11Buffer* const grass_buffs[] = {grassPositions.Get()};

    con->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
    con->IASetInputLayout(grassVertexFormat.Get());
    con->IASetVertexBuffers(0, ARRAYSIZE(grass_buffs), grass_buffs,
                            grass_strides, grass_offsets);

    con->GSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

    con->VSSetShader(grassVertexShader.Get(), 0, 0);
    con->GSSetShader(grassGeoShader.Get(), 0, 0);
    con->PSSetShader(grassPixelShader.Get(), 0, 0);

    // TODO: Move into constructor.
    ID3D11Device* creator;
    d3d.GetDevice((void**)&creator);
    const D3D11_RASTERIZER_DESC grass_desc =
        D3D11_RASTERIZER_DESC{D3D11_FILL_SOLID, D3D11_CULL_NONE};
    ID3D11RasterizerState* grass_rs;
    creator->CreateRasterizerState(&grass_desc, &grass_rs);
    con->RSSetState(grass_rs);

    con->Draw(grass_count, 0);

    // release temp handles
    view->Release();
    depth->Release();
    con->Release();
  }

  ~Renderer() {
    // ComPtr will auto release so nothing to do here
  }
};
