#include "input.h"

#include <iostream>

SimpleMats INPUTTER::camera;
GW::MATH::GVECTORF up = GW::MATH::GVECTORF{0, 1, 0};
GW::MATH::GVECTORF cam_pos = GW::MATH::GVECTORF{1, 0.5f, -2};
GW::MATH::GVECTORF cam_look = GW::MATH::GVECTORF{0, 0.25f, 0};

GW::MATH::GMatrix m;
void INPUTTER::init_camera(float ar) {
  SimpleMats temp;
  GW::GReturn gr;
  gr = m.Create();
  gr = m.IdentityF(temp.w);
  gr = m.LookAtLHF(cam_pos, cam_look, up, temp.v);
  gr = m.ProjectionDirectXLHF(G_DEGREE_TO_RADIAN(70), ar, 0.1f, 1000, temp.p);
  INPUTTER::camera = temp;
  return;
}

void INPUTTER::walk_camera() { return; }
void INPUTTER::look_camera() {
  m.LookAtLHF(cam_pos, cam_look, up, INPUTTER::camera.v);
}
