#pragma once

#include "matrices.h"

namespace INPUTTER {
extern SimpleMats camera;
void init_camera(float);
void walk_camera();
void look_camera();
}  // namespace INPUTTER
