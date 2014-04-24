#ifndef OBJ_H
#define OBJ_H

#include <glm/common.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/ext.hpp>

#include "math/vector.h"
#include <QVector>

struct Vertex;

// A simple parser that reads and writes Wavefront .obj files
class OBJ
{
public:
    struct Index
    {
        int vertex;
        int coord;
        int normal;

        Index() : vertex(-1), coord(-1), normal(-1) {}
        Index(int vertex, int coord, int normal) : vertex(vertex), coord(coord), normal(normal) {}
    };

    struct Triangle
    {
        Vertex *v;
        bool visited;

        Index a, b, c;

        Triangle() {}
        Triangle(const Index &a, const Index &b, const Index &c) : a(a), b(b), c(c) {}
    };

    QVector<Vector3> vertices;
    QVector<Vector2> coords;
    QVector<Vector3> normals;
    QVector<Triangle> triangles;

    void draw() const;
    bool read(const QString &path);
    bool write(const QString &path) const;
    void transform(const glm::mat4 &transform);

private:
    Index getIndex(const QString &str) const;
    void drawIndex(const Index &index) const;
};

#endif // OBJ_H
