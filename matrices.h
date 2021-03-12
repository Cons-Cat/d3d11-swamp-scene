#pragma once

#define GATEWARE_ENABLE_MATH
#include "../Gateware/Gateware.h"

struct SimpleMats {
  GW::MATH::GMATRIXF w, v, p;
};
