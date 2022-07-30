/* date = June 30th 2022 4:11 pm */

#ifndef CAMERA_H
#define CAMERA_H

#include "util.h"

#include <cglm/cglm.h>
#include <cglm/struct.h>

#define CameraType_LAST CameraType_ortho
enum CameraType
{
    CameraType_perspective, CameraType_ortho,
};


struct ViewProj
{
    mat4s view, proj;
};

struct OrthoCamera
{
    struct ViewProj view_proj;
    vec2s position, min, max;
};

struct PerspectiveCamera
{
    struct ViewProj view_proj;
    vec3s position, direction, up, right;
    f32 pitch, yaw, fov, aspect, znear, zfar;
};

void perspective_camera_init(struct PerspectiveCamera *self, f32 fov);
void perspective_camera_update(struct PerspectiveCamera *self);

void ortho_camera_init(struct OrthoCamera *self, vec2s min, vec2s max);
void ortho_camera_update(struct OrthoCamera *self);

#endif //CAMERA_H
