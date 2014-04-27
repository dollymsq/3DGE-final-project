#ifndef TREE_H
#define TREE_H

//#include "math/vector.h"
#include <glm/common.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <QVector>
#include <QPair>

class Tree
{
public:
    Tree(glm::vec3 pos = glm::vec3(0,0,0));
    virtual ~Tree();
    void draw();
    void drawLines();
    void generate(QString L);
//    glm::vec3 getPosition();
private:
    void generateVBO();
    glm::vec3 m_pos;
    QVector<glm::vec3> m_points;
    QVector<float> m_thick;
//    QVector<glm::mat4> m_rotations;
    QVector<QVector<glm::mat4> > m_ctms;
//    QVector<QPair<glm::vec3,float> > m_rotations;
//    void rotate(QVector<glm::mat4> &rotations);
//    void addrotation(glm::mat4 &rotation);
    GLuint m_buffer,m_bufferSize;
};

#endif // TREE_H
