#pragma once

#include "matrices.h"

namespace INPUTTER {
extern SimpleMats camera;
void init_camera(float, GW::SYSTEM::GWindow&);
void walk_camera();
void look_camera();
void update_camera();
}  // namespace INPUTTER
