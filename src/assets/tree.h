#ifndef TREE_H
#define TREE_H

//#include "math/vector.h"
#include "glm/common.hpp"
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
    glm::vec3 m_pos;
    QVector<glm::vec3> m_points;
    QVector<float> m_thick;
    QVector<QVector<QPair<glm::vec3,float> > > m_rotations;
    void rotate(QVector<QPair<glm::vec3,float> > &rotations);
    void addrotation(QVector<QPair<glm::vec3,float> > rotation);
};

#endif // TREE_H
