#pragma once
#include <d3d11.h>
#include <wrl\client.h>

#include <vector>

#include "matrices.h"

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
extern ID3D11Device* creator;

// typedef struct ArraysToGpu ArraysToGpu;
struct ArraysToGpu {
  std::vector<SimpleMats> mats;
  std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> const_buffers;
  std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> vertx_buffers;
  std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> index_buffers;

 public:
  void PushNewModel(const OBJ_VERT*, const unsigned, const unsigned int*,
                    const unsigned, SimpleMats);
  void UpdateView(size_t);
};
