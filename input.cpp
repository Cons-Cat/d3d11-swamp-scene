#define GATEWARE_ENABLE_INPUT

#include "input.h"

#include <iostream>

// SimpleMats INPUTTER::camera;
GW::MATH::GMATRIXF INPUTTER::camera;
GW::INPUT::GInput INPUTTER::input;
float INPUTTER::walk_speed = 0.2f;
/*
GW::MATH::GVECTORF up = GW::MATH::GVECTORF{0, 1, 0};
GW::MATH::GVECTORF cam_pos = GW::MATH::GVECTORF{1, 0.5f, -2.0f};
GW::MATH::GVECTORF cam_look = GW::MATH::GVECTORF{0, 0.25f, 0};
*/
float mouse_x;
float mouse_y;
float mouse_speed = 200.0f;
float look_angle_x = 0;
float look_angle_y = 0;

GW::MATH::GMatrix m;
void INPUTTER::init_camera(float ar, GW::SYSTEM::GWindow &win) {
  input.Create(win);
  input.GetMousePosition(mouse_x, mouse_y);
  /*SimpleMats temp;
  GW::GReturn gr;
  gr = m.Create();
  gr = m.IdentityF(temp.w);
  gr = m.LookAtLHF(cam_pos, cam_look, up, temp.v);
  gr = m.ProjectionDirectXLHF(G_DEGREE_TO_RADIAN(70), ar, 0.1f, 1000, temp.p);
  INPUTTER::camera = temp;
  return;*/
}

/*
#define EVAL_INPUT(DIR)                     \
  float o_##DIR## = 0;                      \
  input.GetState(G_KEY_##DIR##, o_##DIR##); \
  float is_##DIR## = (o_##DIR## == 0 ? 0 : 1.0f);

void INPUTTER::walk_camera() {
  EVAL_INPUT(UP);
  EVAL_INPUT(DOWN);
  EVAL_INPUT(LEFT);
  EVAL_INPUT(RIGHT);
  float z = walk_speed * (is_UP - is_DOWN);
  float x = walk_speed * (is_RIGHT - is_LEFT);
  GW::MATH::GMATRIXF inverse;
  m.InverseF(camera.v, inverse);
  m.TranslatelocalF(inverse, GW::MATH::GVECTORF{x, 0, z}, inverse);
  m.InverseF(inverse, camera.v);
}
*/

void INPUTTER::look_camera() {
  float temp_x;
  float temp_y;
  input.GetMousePosition(temp_x, temp_y);
  look_angle_x = -(mouse_x - temp_x) / mouse_speed;
  look_angle_y = (mouse_y - temp_y) / mouse_speed;

  // temp_mat = GW::MATH::GIdentityMatrixF;
  GW::MATH::GMATRIXF temp_mat;
  // Local x rotation
  m.RotationYawPitchRollF(look_angle_x, 0, 0, temp_mat);
  m.MultiplyMatrixF(camera, temp_mat, camera);
  // Global Y rotation
  m.InverseF(camera, camera);
  m.RotationYawPitchRollF(0, look_angle_y, 0, temp_mat);
  m.MultiplyMatrixF(camera, temp_mat, camera);

  mouse_x = temp_x;
  mouse_y = temp_y;
}
