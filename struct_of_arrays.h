#pragma once
#include <vector>

#ifndef __OBJ_VERT__
typedef struct _OBJ_VERT_ {
  float pos[3];  // Left-handed +Z forward coordinate w not provided, assumed to
                 // be 1.
  float uvw[3];  // D3D/Vulkan style top left 0,0 coordinate.
  float nrm[3];  // Provided direct from obj file, may or may not be normalized.
} OBJ_VERT;
#define __OBJ_VERT__
#endif

// TODO: Find a better place.
ID3D11Device* creator;

struct SimpleMats {
  GW::MATH::GMATRIXF w, v, p;
};

struct ArraysToGpu {
  std::vector<SimpleMats> mats;
  std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> const_buffers;
  std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> vertx_buffers;
  std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> index_buffers;

 public:
  void PushNewModel(OBJ_VERT*, unsigned int*);
};

void ArraysToGpu::PushNewModel(OBJ_VERT* vtx, unsigned int* ind) {
  size_t i = mats.size();
  // Create Matrix
  mats.push_back(SimpleMats{GW::MATH::GIdentityMatrixF});
  // Create Constant Buffer
  D3D11_SUBRESOURCE_DATA cData = {&mats[i], 0, 0};
  CD3D11_BUFFER_DESC cDesc(sizeof(mats[i]), D3D11_BIND_CONSTANT_BUFFER);
  creator->CreateBuffer(&cDesc, &cData, const_buffers[i].GetAddressOf());
  // Create Vertex Buffer
  D3D11_SUBRESOURCE_DATA bData = {vtx, 0, 0};
  CD3D11_BUFFER_DESC bDesc(sizeof(*vtx), D3D11_BIND_VERTEX_BUFFER);
  creator->CreateBuffer(&bDesc, &bData, vertx_buffers[i].GetAddressOf());
  // Create Index Buffer
  D3D11_SUBRESOURCE_DATA iData = {ind, 0, 0};
  CD3D11_BUFFER_DESC iDesc(sizeof(*ind), D3D11_BIND_INDEX_BUFFER);
  creator->CreateBuffer(&iDesc, &iData, index_buffers[i].GetAddressOf());
  return;
};
