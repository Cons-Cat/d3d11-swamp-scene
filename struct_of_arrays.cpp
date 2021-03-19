#include "struct_of_arrays.h"

#include "input.h"

ID3D11Device* creator;

void ArraysToGpu::PushNewModel(ModelTemplate model, SimpleMats cam) {
  // Pushing camera.
  size_t i = mats.size();
  mats.push_back(cam);
  // Get model information.
  const OBJ_VERT* vtx = model.vert;
  const unsigned int* ind = model.ind;
  size_t v_count = model.v_count;
  size_t i_count = model.i_count;
  // Create Matrix.
  D3D11_SUBRESOURCE_DATA cData = {&mats[i], 0, 0};
  CD3D11_BUFFER_DESC cDesc(sizeof(mats[i]), D3D11_BIND_CONSTANT_BUFFER);
  // Create Constant Buffer.
  const_buffers.push_back(Microsoft::WRL::ComPtr<ID3D11Buffer>());
  creator->CreateBuffer(&cDesc, &cData, const_buffers[i].GetAddressOf());
  // Create Vertex Buffer.
  vertx_buffers.push_back(Microsoft::WRL::ComPtr<ID3D11Buffer>());
  D3D11_SUBRESOURCE_DATA vData = {vtx, 0, 0};
  CD3D11_BUFFER_DESC vDesc(sizeof(*vtx) * v_count, D3D11_BIND_VERTEX_BUFFER);
  creator->CreateBuffer(&vDesc, &vData, vertx_buffers[i].GetAddressOf());
  // Create Index Buffer.
  index_buffers.push_back(Microsoft::WRL::ComPtr<ID3D11Buffer>());
  D3D11_SUBRESOURCE_DATA iData = {ind, 0, 0};
  CD3D11_BUFFER_DESC iDesc(sizeof(*ind) * i_count, D3D11_BIND_INDEX_BUFFER);
  creator->CreateBuffer(&iDesc, &iData, index_buffers[i].GetAddressOf());
  return;
};

void ArraysToGpu::UpdateView() {
  for (size_t i = 0; i < mats.size(); i++) {
    // mats[i].v = INPUTTER::camera.v;
  }
}

void ArraysToGpu::TranslateModel(const unsigned ind, float x, float y,
                                 float z) {
  // Wipe the buffer
  // Copy in verts from header
  // Translate
  // Copy into replacement buffer
}