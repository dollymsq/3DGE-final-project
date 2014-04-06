#ifndef VERTEX_H
#define VERTEX_H

#include "obj.h"

struct Vertex  {
    OBJ::Triangle *tri;
    Vertex* prev=0,*after;
    Vector3 v0,v1;
};

#endif // VERTEX_H
