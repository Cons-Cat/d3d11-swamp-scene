#pragma once

#include "matrices.h"

namespace INPUTTER {
extern SimpleMats camera;
extern GW::MATH::GMatrix m;
void init_camera(GW::GRAPHICS::GDirectX11Surface);
void walk_camera();
void look_camera();
}  // namespace INPUTTER
