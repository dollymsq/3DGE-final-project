#ifndef OBJ_H
#define OBJ_H

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <QVector>

struct Vertex;

// A simple parser that reads and writes Wavefront .obj files
class Obj
{
public:
    Obj(const QString &path);

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

    QVector<glm::vec3> vertices;
    QVector<glm::vec2> coords;
    QVector<glm::vec3> normals;
    QVector<Triangle> triangles;

    void draw() const;
    bool read(const QString &path);
    bool write(const QString &path) const;

private:
    Index getIndex(const QString &str) const;
    void drawIndex(const Index &index) const;
};

#endif // OBJ_H
