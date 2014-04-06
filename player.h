#ifndef PLAYER_H
#define PLAYER_H

#include "vector.h"

struct Player  {
    Vector3 pos,vel,radius = Vector3(.5,.5,.5);
    Vector4 color;
    const float horVelConst = 5,vertVelConst = 5,jumpVelConst = 5.5;
    float lives;
    bool m_jump=false;
};

#endif // PLAYER_H
