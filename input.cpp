#include "input.h"

#include <iostream>

SimpleMats INPUTTER::camera;

void INPUTTER::init_camera(float ar) {
  GW::MATH::GMatrix m;
  SimpleMats temp;
  GW::GReturn gr;
  gr = m.Create();
  std::cout << (gr == GW::GReturn::SUCCESS ? "Success" : "Fail");
  gr = m.IdentityF(temp.w);
  std::cout << (gr == GW::GReturn::SUCCESS ? "Success" : "Fail");
  gr = m.LookAtLHF(GW::MATH::GVECTORF{1, 0.5f, -2},
                   GW::MATH::GVECTORF{0, 0.25f, 0}, GW::MATH::GVECTORF{0, 1, 0},
                   temp.v);
  std::cout << (gr == GW::GReturn::SUCCESS ? "Success" : "Fail");
  gr = m.ProjectionDirectXLHF(G_DEGREE_TO_RADIAN(70), ar, 0.1f, 1000, temp.p);
  std::cout << (gr == GW::GReturn::SUCCESS ? "Success" : "Fail");
  INPUTTER::camera = temp;
  return;
}

/*
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
*/

void INPUTTER::walk_camera() { return; }
void INPUTTER::look_camera() { camera.v = GW::MATH::GIdentityMatrixF; }
