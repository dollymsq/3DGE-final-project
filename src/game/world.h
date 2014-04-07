#ifndef WORLD_H
#define WORLD_H

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/noise.hpp>

#include <QHash>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLFunctions>

#include <iostream>

class World : protected QOpenGLFunctions
{
public:
    World();
    void init();
    void drawWorld(QOpenGLShaderProgram *program);

private:
    int m_numCubes;

    void initWorld();

    GLuint m_vboIds[2];

    enum MaterialType {
        GRASS, DIRT, WATER
    };

    struct VertexData {
        glm::vec3 pos;
        glm::vec2 tex;
    };

    struct Chunk {
        glm::vec3 loc;
        bool active;
        MaterialType material;
        qint8 sides; //1,2,3,4,5,6
    };

    QHash<quint32, Chunk> world;

    void testCube(QVector<VertexData> &vertices, QVector<GLushort> &indices);
//    void addCube(glm::vec3 &location, QVector<VertexData> &vertices, QVector<GLuint> &indices);
    void addCube(int x, int y, int z, QVector<VertexData> &vertices, QVector<GLushort> &indices);
};

#endif // WORLD_H
