#ifndef CAMERA_H
#define CAMERA_H

#include "vector.h"
#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif
#include "qgl.h"
#include <iostream>

using namespace std;

class Camera
{
public:
    Camera();
    ~Camera();

    void lookAt();
    void cameraOrient(int x, int y);
    void setEyePosition(Vector3 pos) { eye = pos - forward*5 + Vector3(0,3,0); }

    Vector3 right, forward;

private:
    Vector3 eye, look, up;
    float cur_yaw, cur_pitch; // degrees
};

#endif // CAMERA_H
