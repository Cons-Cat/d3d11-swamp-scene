#include "input.h"

SimpleMats INPUTTER::camera;

void INPUTTER::init_camera(float ar) {
  GW::MATH::GMatrix m;
  m.Create();
  m.IdentityF(INPUTTER::camera.w);
  m.LookAtLHF(GW::MATH::GVECTORF{1, 0.5f, -2}, GW::MATH::GVECTORF{0, 0.25f, 0},
              GW::MATH::GVECTORF{0, 1, 0}, INPUTTER::camera.v);
  m.ProjectionDirectXLHF(G_DEGREE_TO_RADIAN(70), ar, 0.1f, 1000,
                         INPUTTER::camera.p);
  return;
}

void INPUTTER::walk_camera() { return; }
void INPUTTER::look_camera() { camera.v = GW::MATH::GIdentityMatrixF; }
