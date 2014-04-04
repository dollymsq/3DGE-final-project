#include "camera.h"

Camera::Camera()
{
    eye = Vector3(-1, 6, 0);
    up = Vector3(0, 1, 0);
    look = Vector3(1, 0, 0);
    right = Vector3(0, 0, 1);
    forward = look;

    cur_yaw = 0;
    cur_pitch = 0;
}

Camera::~Camera()
{
}

void Camera::lookAt()
{
    gluLookAt(eye.x, eye.y, eye.z,
              eye.x + look.x, eye.y + look.y, eye.z + look.z,
              up.x, up.y, up.z);
}

void Camera::cameraOrient(int x, int y)
{
    cur_pitch += (float)y * 0.1;

    cur_pitch = min(89.99f, cur_pitch);
    cur_pitch = max(-89.99f, cur_pitch);

    cur_yaw += (float)x * 0.1;
    if(cur_yaw > 360.0f) cur_yaw -= 360.0f;
    else if(cur_yaw < -360.0f) cur_yaw += 360.0f;

    look = look.fromAngles(cur_yaw / 180.0f * M_PI, cur_pitch / 180.0f * M_PI);
    right = look.cross(up).unit();
    forward = up.cross(right);
}

