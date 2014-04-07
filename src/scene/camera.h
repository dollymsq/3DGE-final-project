#ifndef CAMERA_H
#define CAMERA_H

#include "math/vector.h"

/*
 * This struct models a simple camera
 */

#undef near
#undef far
struct Camera
{
    Vector3 eye, center, up;
    float angle, near, far, yaw = 0.0f, pitch = 0.0f;
};

#endif // CAMERA_H
