#pragma once
#include "struct_of_arrays.h"
struct SimpleMats;

namespace INPUTTER {
SimpleMats camera;
void init_camera(GW::GRAPHICS::GDirectX11Surface);
void walk_camera();
void look_camera();
}  // namespace INPUTTER

void INPUTTER::init_camera(GW::GRAPHICS::GDirectX11Surface d3d) {
  // Math
  GW::MATH::GMatrix m;
  m.Create();
  m.IdentityF(INPUTTER::camera.w);
  m.LookAtLHF(GW::MATH::GVECTORF{1, 0.5f, -2}, GW::MATH::GVECTORF{0, 0.25f, 0},
              GW::MATH::GVECTORF{0, 1, 0}, INPUTTER::camera.v);
  float ar = 1;
  d3d.GetAspectRatio(ar);
  m.ProjectionDirectXLHF(G_DEGREE_TO_RADIAN(70), ar, 0.1f, 1000,
                         INPUTTER::camera.p);
}
void INPUTTER::walk_camera() { return; }
void INPUTTER::look_camera() { return; }
