#include "struct_of_arrays.h"

ID3D11Device* creator;

void ArraysToGpu::PushNewModel(const OBJ_VERT* vtx, const unsigned v_count,
                               const unsigned int* ind, const unsigned i_count,
                               SimpleMats cam) {
  size_t i = mats.size();
  mats.push_back(cam);
  // Create Matrix
  D3D11_SUBRESOURCE_DATA cData = {&mats[i], 0, 0};
  CD3D11_BUFFER_DESC cDesc(sizeof(mats[i]), D3D11_BIND_CONSTANT_BUFFER);
  // Create Constant Buffer
  const_buffers.push_back(Microsoft::WRL::ComPtr<ID3D11Buffer>());
  creator->CreateBuffer(&cDesc, &cData, const_buffers[i].GetAddressOf());
  // Create Vertex Buffer
  vertx_buffers.push_back(Microsoft::WRL::ComPtr<ID3D11Buffer>());
  D3D11_SUBRESOURCE_DATA vData = {vtx, 0, 0};
  CD3D11_BUFFER_DESC vDesc(sizeof(*vtx) * v_count, D3D11_BIND_VERTEX_BUFFER);
  creator->CreateBuffer(&vDesc, &vData, vertx_buffers[i].GetAddressOf());
  // Create Index Buffer
  index_buffers.push_back(Microsoft::WRL::ComPtr<ID3D11Buffer>());
  D3D11_SUBRESOURCE_DATA iData = {ind, 0, 0};
  CD3D11_BUFFER_DESC iDesc(sizeof(*ind) * i_count, D3D11_BIND_INDEX_BUFFER);
  creator->CreateBuffer(&iDesc, &iData, index_buffers[i].GetAddressOf());
  return;
};

void ArraysToGpu::UpdateView(size_t ind) { return; }
