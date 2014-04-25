#include <QVector>
#include <qgl.h>
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>
#include <iostream>
#include "tree.h"
#include "math/common.h"
#include "math/levent.h"
#include "math/lparser.h"
#include "assets/obj.h"
#include "graphics/opengl.h"

Tree::Tree(glm::vec3 pos)
{
    m_pos = pos;
}

Tree::~Tree()  {

}

//void Tree::setPosition()  {
//    m_pos = pos;
//}

//void Tree::rotate(QVector<QPair<glm::vec3,float> > &rotations)  {
//    for(int i = 0; i < rotations.length(); i++)  {
//        glm::vec3 vec = rotations.at(i).first;
//        float degrees = rotations.at(i).second;
//        glRotatef(degrees,vec.x,vec.y,vec.z);
//    }
//}

void Tree::drawLines()  {
    glColor3f(0,0,1);
    glPushMatrix();
    glTranslatef(m_pos.x,m_pos.y,m_pos.z);
    glLineWidth(m_thick.at(0));
        for(int i = 0; i < m_points.length(); i+=2)  {
            glLineWidth(m_thick.at(i));
            glBegin(GL_LINES);
            glm::vec3 vec = m_points.at(i);
            glm::vec3 vec2 = m_points.at(i+1);
            glVertex3f(vec.x,vec.y,vec.z);
            glVertex3f(vec2.x,vec2.y,vec2.z);
            glEnd();
        }
    glPopMatrix();
}

void Tree::draw()  {
    glPushMatrix();
    glTranslatef(m_pos.x,m_pos.y,m_pos.z);

    glPopMatrix();
}

void Tree::addrotation(glm::mat4 &rotation)  {
//    m_rotations.append(rotation);
}

void Tree::generate(QString L)  {
    QVector<LEvent> eventStack;
    glm::vec3 currentDir = glm::vec3(0,1,0);
    glm::vec3 currentUp = glm::vec3(1,0,0);
    glm::vec3 currentLeft = glm::vec3(0,0,1);
    glm::vec3 currentPos = glm::vec3(0,0,0);
    int currPointer = 0;
    float currentThickness = 1;
    QPair<Command,float> currCommand;
    QVector<glm::vec4> currentRotations;
//    glm::mat4 currentRot;
//    std::cout << currentRot[0][0] << std::endl;
    currCommand.first = CONTINUE;
    glm::mat3 rotation;
    QPair<glm::vec3,float> currentRot;
    currentRot.first = currentDir;
    currentRot.second = 0.0f;
    while(currCommand.first != STOP)  {
        currCommand = LParser::parseString(L,currPointer);
        bool toRotate = false;
        float param = currCommand.second;
        LEvent e,ePush;
        switch(currCommand.first) {
        case F:
            m_points.append(currentPos);
            m_thick.append(currentThickness);
            m_rotations.append(currentRot);
            currentPos += currentDir*param;
            m_points.append(currentPos);
            m_thick.append(currentThickness);
            m_rotations.append(currentRot);
            break;
        case f:
            currentPos += currentDir*param;
            break;
        case YAW:
            rotation = glm::mat3(cos(param*M_PI/180.0f),sin(param*M_PI/180.0f),0,
                                 -1.0f*sin(param*M_PI/180.0f),cos(param*M_PI/180.0f),0,
                                 0,0,1);
//            currentRot = glm::rotate(currentRot,(float)(param*M_PI/180.0f),currentUp);
            currentRot.first = currentUp;
            currentRot.second = param*M_PI/180.0f;
            toRotate = true;

            break;
        case NEGYAW:
            rotation = glm::mat3(cos(-1.0f*param*M_PI/180.0f),sin(-1.0f*param*M_PI/180.0f),0,
                                 -1.0f*sin(-1.0f*param*M_PI/180.0f),cos(-1.0f*param*M_PI/180.0f),0,
                                 0,0,1);
//            currentRot = glm::rotate(currentRot,-1.0f*(float)(param*M_PI/180.0f),currentUp);
            currentRot.first = currentUp;
            currentRot.second = -1.0f*param*M_PI/180.0f;
            toRotate = true;
            break;
        case ROLL:
            rotation = glm::mat3(1,0,0,
                                 0,cos(param*M_PI/180.0f),-1.0f*sin(param*M_PI/180.0f),
                                 0,sin(param*M_PI/180.0f),cos(param*M_PI/180.0f));
//            currentRot = glm::rotate(currentRot,(float)(param*M_PI/180.0f),currentDir);
            currentRot.first = currentDir;
            currentRot.second = param*M_PI/180.0f;
            toRotate = true;
            break;
        case NEGROLL:
            rotation = glm::mat3(1,0,0,
                                 0,cos(-1.0f*param*M_PI/180.0f),-1.0f*sin(-1.0f*param*M_PI/180.0f),
                                 0,sin(-1.0f*param*M_PI/180.0f),cos(-1.0f*param*M_PI/180.0f));
//            currentRot = glm::rotate(currentRot,-1.0f*(float)(param*M_PI/180.0f),currentDir);
            currentRot.first = currentDir;
            currentRot.second = -1.0f*param*M_PI/180.0f;
            toRotate = true;
            break;
        case PITCH:
            rotation = glm::mat3(cos(param*M_PI/180.0f),0,-1.0f*sin(param*M_PI/180.0f),
                                 0,1,0,
                                 sin(param*M_PI/180.0f),0,cos(param*M_PI/180.0f));
//            currentRot = glm::rotate(currentRot,(float)(param*M_PI/180.0f),currentLeft);
            currentRot.first = currentLeft;
            currentRot.second = param*M_PI/180.0f;
            toRotate = true;
            break;
        case NEGPITCH:
            rotation = glm::mat3(cos(-1.0f*param*M_PI/180.0f),0,-1.0f*sin(-1.0f*param*M_PI/180.0f),
                                 0,1,0,
                                 sin(-1.0f*param*M_PI/180.0f),0,cos(-1.0f*param*M_PI/180.0f));
//            currentRot = glm::rotate(currentRot,-1.0f*(float)(param*M_PI/180.0f),currentLeft);
            currentRot.first = currentLeft;
            currentRot.second = -1.0f*param*M_PI/180.0f;
            toRotate = true;
            break;
        case LINEWIDTH:
            currentThickness = param;
            break;
        case COLOR:
            break;
        case POP:
            e = eventStack.last();
            eventStack.pop_back();
            currentDir = e.currentDir;
            currentUp = e.currentUp;
            currentLeft = e.currentLeft;
            currentPos = e.currentPos;
            currentThickness = e.currentThickness;
            currentRot = e.currentRotation;
            break;
        case PUSH:
            ePush.currentDir = currentDir;
            ePush.currentLeft = currentLeft;
            ePush.currentPos = currentPos;
            ePush.currentUp = currentUp;
            ePush.currentThickness = currentThickness;
            ePush.currentRotation = currentRot;
            eventStack.push_back(ePush);
            break;
        default:
            break;
        }
        if(toRotate) {
            glm::mat3 updateMat = glm::mat3(currentDir.x,currentLeft.x,currentUp.x,
                                            currentDir.y,currentLeft.y,currentUp.y,
                                            currentDir.z,currentLeft.z,currentUp.z);

            updateMat = rotation*updateMat;

            currentDir = glm::vec3(updateMat[0][0],updateMat[1][0],updateMat[2][0]);
            currentLeft = glm::vec3(updateMat[0][1],updateMat[1][1],updateMat[2][1]);
            currentUp = glm::vec3(updateMat[0][2],updateMat[1][2],updateMat[2][2]);
        }
    }
}

int Tree::generateVBO()  {
    QVector<float> currentVBO;
    Obj cylinder(":/meshes/cylinder_mesh.obj");
    for(int i = 0; i < m_points.size(); i+=2)  {
        glm::mat4 CTM;
        CTM = glm::translate(CTM,m_pos);
        CTM = glm::translate(CTM,(m_points.at(i+1) + m_points.at(i))/2.0f);
        CTM = glm::rotate(CTM,m_rotations.at(i).second,m_rotations.at(i).first);
        glm::vec3 scaleVec = glm::vec3(m_thick.at(i),(m_points.at(i+1) - m_points.at(i)).length(),m_thick.at(i));
        CTM = glm::scale(CTM,scaleVec);
        currentVBO += cylinder.transform(CTM);
    }
    gl->glGenBuffers(1, &m_buffer);
    gl->glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
    gl->glBufferData(GL_ARRAY_BUFFER, currentVBO.size()*sizeof(float), currentVBO.data(), GL_STATIC_DRAW);
}




