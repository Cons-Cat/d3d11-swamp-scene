#pragma once

#define GATEWARE_ENABLE_MATH
#include "../Gateware/Gateware.h"

#ifndef __OBJ_VERT__
typedef struct _OBJ_VERT_ {
  float pos[3];  // Left-handed +Z forward coordinate w not provided, assumed to
                 // be 1.
  float uvw[3];  // D3D/Vulkan style top left 0,0 coordinate.
  float nrm[3];  // Provided direct from obj file, may or may not be normalized.
} OBJ_VERT;
#define __OBJ_VERT__
#endif

struct SimpleMats {
  GW::MATH::GMATRIXF w, v, p;
};

struct ModelTemplate {
  const OBJ_VERT* vert;
  const unsigned int* ind;
  size_t v_count;
  size_t i_count;
};
