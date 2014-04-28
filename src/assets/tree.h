#ifndef TREE_H
#define TREE_H

//#include "math/vector.h"
#include <glm/common.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <QVector>
#include <QPair>
#include <PxPhysicsAPI.h>
#include "PxSimulationEventCallback.h"

class Tree
{
public:
    Tree(glm::vec3 pos = glm::vec3(0,0,0));
    virtual ~Tree();
    void draw();
    void drawLines();
    void generate(QString L);
//    QVector<PxVec3> getVerts(){return m_vertices;}
private:
    void generateVBO();
    void generateVBOOld();
    glm::vec3 m_pos;

    QVector<glm::vec3> m_points,m_prevDirs;
    QVector<float> m_thick;
    QVector<QPair<glm::vec3,glm::vec3> > m_vecs;
    GLuint m_buffer,m_bufferSize;
//    QVector<PxVec3> m_vertices;
};

#endif // TREE_H
