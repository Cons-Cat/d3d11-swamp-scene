#pragma once
#include <d3d11.h>
#include <wrl\client.h>

#include <vector>

#include "matrices.h"

// TODO: Find a better place.
extern ID3D11Device* creator;

struct ArraysToGpu {
  // TODO: All v fields should be contiguous.
  std::vector<SimpleMats> mats;
  std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> const_buffers;
  std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> vertx_buffers;
  std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> index_buffers;
  std::vector<ModelTemplate> models;

 public:
  void PushNewModel(ModelTemplate, SimpleMats);
  void UpdateView();
  void TranslateModel(const unsigned, float, float, float);
};
