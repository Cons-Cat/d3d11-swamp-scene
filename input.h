#pragma once

#include "matrices.h"

namespace INPUTTER {
extern GW::MATH::GMATRIXF camera;
extern float walk_speed;
extern GW::INPUT::GInput input;
void init_camera(float, GW::SYSTEM::GWindow&);
// void walk_camera();
void look_camera();
}  // namespace INPUTTER
