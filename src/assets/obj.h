#ifndef OBJ_H
#define OBJ_H


#include "graphics/opengl.h"
#include "assets/renderable.h"

#include <PxPhysicsAPI.h>
#include "PxSimulationEventCallback.h"

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/ext.hpp>

#include <QVector>
#include <QFile>
#include <QTextStream>

using namespace physx;

struct Vertex;

// A simple parser that reads and writes Wavefront .obj files
class Obj : public Renderable
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


    struct MeshBuffer {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texcoord;
    };

    GLuint m_vao;
    GLuint m_buffer;

    QVector<glm::vec3> vertices;
    QVector<glm::vec2> coords;
    QVector<glm::vec3> normals;
    QVector<Triangle> triangles;

    virtual void draw() const;
    virtual QVector<PxVec3> getVerts();
    virtual QVector<PxU32> getInds();
    bool read(const QString &path);
    bool write(const QString &path) const;
    QVector<float> transform(const glm::mat4 &transform);
    glm::vec3 transformPoint(const glm::vec4 &point, const glm::mat4 &transform);

    void vbo();
private:
    Index getIndex(const QString &str) const;
    void drawIndex(const Index &index) const;
};

#endif // OBJ_H
