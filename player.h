#ifndef PLAYER_H
#define PLAYER_H
#include "vector.h"
#include <QVector>
#include "scene.h"
#include "camera.h"

#define EPSILON 1.0e-4

struct Triangle;

struct TriangleInstance
{
    Vector3 vertices[3];
    Vector3 normal;

    TriangleInstance(Vector3 a, Vector3 b, Vector3 c)
    {
        vertices[0] = a;
        vertices[1] = b;
        vertices[2] = c;
        normal = (b-a).cross(c-a);
        normal.normalize();
    }

    TriangleInstance scale(Vector3 factor)
    {
        return TriangleInstance(vertices[0]*factor, vertices[1]*factor, vertices[2]*factor);
    }

};

class Player
{
private:
    //ellip world

    //sphere world
    Vector3 sp_start;
    Vector3 sp_end;
    Vector3 sp_collision;
    Vector3 sp_colresp;

    Vector3 radius;
    Vector3 right, forward;
    Vector3 cur_normal;
    Vector3 eplback;


    float vup, height;//for jumping

    bool checkInsideTriangle(Vector3 p, const TriangleInstance &abc);
    bool checkOnEdge(Vector3 p, Vector3 &pc, Vector3 c, Vector3 d);
    void getEquationRoot(float a, float b, float c, float &t1, float &t2);

public:
    Player();
    void setStartPoint(Vector3 p) {start = p; sp_start = p/radius; }
    void setEndPoint(Vector3 p) {end = p; sp_end = p/radius; }
    void setRadius(Vector3 r) {radius = r;}
    void setHeightANDVelocity() {height = start.y; vup = 8;}//for jump
    Vector3 getCollisionPoint() {return collision; }

    bool calculateHitPoint(const OBJ &obj); // xz-plane collision test
    void iterativeCollisionDetect(OBJ &obj);


    Vector3 start;
    Vector3 end;
    Vector3 collision;
    Vector3 colresp;

    void render();
    void setDirection(Camera &c);

    void moveLeft(Scene &m_s);
    void moveRight(Scene &m_s);
    void moveForward(Scene &m_s);
    void moveBack(Scene &m_s);
    void moveUp(Scene &m_s, bool &injump, float t);


};

#endif // PLAYER_H
