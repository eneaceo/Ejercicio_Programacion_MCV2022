#include "mcv_platform.h"
#include "camera.h"

void CCamera::updateViewProjection() {
  view_projection = view * projection;
}

void CCamera::lookAt(VEC3 new_eye, VEC3 new_target, VEC3 new_up_aux) {
  eye = new_eye;
  target = new_target;
  view = MAT44::CreateLookAt(new_eye, new_target, new_up_aux);
  updateViewProjection();

  front = (new_target - new_eye);
  front.Normalize();
  left = new_up_aux.Cross(front);
  left.Normalize();
  up = front.Cross(left);
}

void CCamera::setProjectionParams(float new_fov_vertical_radians, float new_aspect_ratio, float new_z_near, float new_z_far) {
  z_min = new_z_near;
  z_max = new_z_far;
  fov_vertical_radians = new_fov_vertical_radians;
  aspect_ratio = new_aspect_ratio;
  projection = MAT44::CreatePerspectiveFieldOfView(
    new_fov_vertical_radians, 
    new_aspect_ratio, 
    new_z_near, new_z_far
  );
  updateViewProjection();

  is_ortho = false;
}

void CCamera::setOrthoParams(bool is_centered, float offset_left, float width, float offset_top, float height, float new_znear, float new_zfar)
{
  z_min = new_znear;
  z_max = new_zfar;
  aspect_ratio = width/height;
  if (is_centered)
  {
      projection = MAT44::CreateOrthographic(width, height, z_min, z_max);
  }
  else
  {
      projection = MAT44::CreateOrthographicOffCenter(offset_left, width, height, offset_top, z_min, z_max);
  }
  updateViewProjection();

  is_ortho = true;
}

void CCamera::setAspectRatio(float new_aspect_ratio) {
  setProjectionParams(fov_vertical_radians, new_aspect_ratio, z_min, z_max);
}
