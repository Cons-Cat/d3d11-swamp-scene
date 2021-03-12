#include "input.h"

SimpleMats INPUTTER::camera;
GW::MATH::GMatrix INPUTTER::m;

void INPUTTER::init_camera(GW::GRAPHICS::GDirectX11Surface d3d) {
  // Math
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
void INPUTTER::look_camera() { camera.v = GW::MATH::GIdentityMatrixF; }
