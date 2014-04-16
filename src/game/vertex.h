#ifndef VERTEX_H
#define VERTEX_H

#include "assets/obj.h"

struct Vertex  {
    Obj::Triangle *tri;
    Vertex* prev=0,*after;
    Vector3 v0,v1;
};

#endif // VERTEX_H
