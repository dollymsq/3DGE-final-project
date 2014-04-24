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

Tree::Tree(glm::vec3 pos)
{
    m_pos = pos;
}

Tree::~Tree()  {

}

//void Tree::setPosition()  {
//    m_pos = pos;
//}

void Tree::rotate(QVector<QPair<glm::vec3,float> > &rotations)  {
    for(int i = 0; i < rotations.length(); i++)  {
        glm::vec3 vec = rotations.at(i).first;
        float degrees = rotations.at(i).second;
        glRotatef(degrees,vec.x,vec.y,vec.z);
    }
}

void Tree::drawLines()  {
    glColor3f(0,0,1);
    glPushMatrix();
    glTranslatef(m_pos.x,m_pos.y,m_pos.z);
    glLineWidth(m_thick.at(0));
//    std::cout << "thickness: "<< m_thick.at(0) << std::endl;
//    glBegin(GL_LINES);
        for(int i = 0; i < m_points.length(); i+=2)  {
//            std::cout << m_thick.at(i) << std::endl;
            glLineWidth(m_thick.at(i));
            glBegin(GL_LINES);
            glm::vec3 vec = m_points.at(i);
            glm::vec3 vec2 = m_points.at(i+1);
            glVertex3f(vec.x,vec.y,vec.z);
            glVertex3f(vec2.x,vec2.y,vec2.z);
            glEnd();
//            if((i+1)%2 == 0)  {
//                glEnd();
//                glLineWidth(m_thick.at(i));
//                glBegin(GL_LINES);
//            }
        }
//    glEnd();
    glPopMatrix();
//    glColor3f(0,0,1);
}

void Tree::draw()  {
    glPushMatrix();
    glTranslatef(m_pos.x,m_pos.y,m_pos.z);

    glPopMatrix();
}

void Tree::addrotation(QVector<QPair<glm::vec3,float> > rotation)  {
    m_rotations.append(rotation);
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
    currCommand.first = CONTINUE;//LParser::parseString(L,currPointer);
//    std::cout << "first command: " << currCommand.first << std::endl;
    glm::mat3 rotation = glm::mat3(1,0,0,
                                   0,1,0,
                                   0,0,1);
    while(currCommand.first != STOP)  {
        currCommand = LParser::parseString(L,currPointer);
        bool toRotate = false;
//        std::cout << "next command: " << currCommand.first << std::endl;
//        std::cout << "currentDir: " << currentDir.x << "," << currentDir.y << "," << currentDir.z << std::endl;
//        std::cout << "currentLeft: " << currentLeft.x << "," << currentLeft.y << "," << currentLeft.z << std::endl;
//        std::cout << "currentUp: " << currentUp.x << "," << currentUp.y << "," << currentUp.z << std::endl;
        float param = currCommand.second;
        LEvent e,ePush;
//        glm::mat3 rotation = glm::mat3(1,0,0,
//                                       0,1,0,
//                                       0,0,1);
        switch(currCommand.first) {
        case F:
            m_points.append(currentPos);
            m_thick.append(currentThickness);
            currentPos += currentDir*param;
            m_points.append(currentPos);
            m_thick.append(currentThickness);
            break;
        case f:
            currentPos += currentDir*param;
            break;
        case YAW:
            rotation = glm::mat3(cos(param*M_PI/180.0f),sin(param*M_PI/180.0f),0,
                                 -1.0f*sin(param*M_PI/180.0f),cos(param*M_PI/180.0f),0,
                                 0,0,1);
            toRotate = true;
            break;
        case NEGYAW:
            rotation = glm::mat3(cos(-1.0f*param*M_PI/180.0f),sin(-1.0f*param*M_PI/180.0f),0,
                                 -1.0f*sin(-1.0f*param*M_PI/180.0f),cos(-1.0f*param*M_PI/180.0f),0,
                                 0,0,1);
            toRotate = true;
            break;
        case ROLL:
            rotation = glm::mat3(1,0,0,
                                 0,cos(param*M_PI/180.0f),-1.0f*sin(param*M_PI/180.0f),
                                 0,sin(param*M_PI/180.0f),cos(param*M_PI/180.0f));
            toRotate = true;
            break;
        case NEGROLL:
            rotation = glm::mat3(1,0,0,
                                 0,cos(-1.0f*param*M_PI/180.0f),-1.0f*sin(-1.0f*param*M_PI/180.0f),
                                 0,sin(-1.0f*param*M_PI/180.0f),cos(-1.0f*param*M_PI/180.0f));
            toRotate = true;
            break;
        case PITCH:
//            std::cout << "pitchin'" << std::endl;
            rotation = glm::mat3(cos(param*M_PI/180.0f),0,-1.0f*sin(param*M_PI/180.0f),
                                 0,1,0,
                                 sin(param*M_PI/180.0f),0,cos(param*M_PI/180.0f));
            toRotate = true;
            break;
        case NEGPITCH:
            rotation = glm::mat3(cos(-1.0f*param*M_PI/180.0f),0,-1.0f*sin(-1.0f*param*M_PI/180.0f),
                                 0,1,0,
                                 sin(-1.0f*param*M_PI/180.0f),0,cos(-1.0f*param*M_PI/180.0f));
            toRotate = true;
            break;
        case LINEWIDTH:
            currentThickness = param;
//            std::cout << thick << std::endl;
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
            break;
        case PUSH:
            ePush.currentDir = currentDir;
            ePush.currentLeft = currentLeft;
            ePush.currentPos = currentPos;
            ePush.currentUp = currentUp;
            ePush.currentThickness = currentThickness;
            eventStack.push_back(ePush);
            break;
        default:
            break;
        }
        if(toRotate) {
//            std::cout << "param: " << param << std::endl;
            glm::mat3 updateMat = glm::mat3(currentDir.x,currentLeft.x,currentUp.x,
                                            currentDir.y,currentLeft.y,currentUp.y,
                                            currentDir.z,currentLeft.z,currentUp.z);
//            std::cout << "updateMat: " << std::endl;
//            std::cout << "(0,0): " << updateMat[0][0] << " (0,1): " << updateMat[0][1] << " (0,2): " << updateMat[0][2] << std::endl;
//            std::cout << "(1,0): " << updateMat[1][0] << " (1,1): " << updateMat[1][1] << " (1,2): " << updateMat[1][2] << std::endl;
//            std::cout << "(2,0): " << updateMat[2][0] << " (2,1): " << updateMat[2][1] << " (2,2): " << updateMat[2][2] << std::endl;

//            std::cout << "rotation: " << std::endl;
//            std::cout << "(0,0): " << rotation[0][0] << " (0,1): " << rotation[0][1] << " (0,2): " << rotation[0][2] << std::endl;
//            std::cout << "(1,0): " << rotation[1][0] << " (1,1): " << rotation[1][1] << " (1,2): " << rotation[1][2] << std::endl;
//            std::cout << "(2,0): " << rotation[2][0] << " (2,1): " << rotation[2][1] << " (2,2): " << rotation[2][2] << std::endl;

            updateMat = rotation*updateMat;

//            std::cout << "updated Mat: " << std::endl;
//            std::cout << "(0,0): " << updateMat[0][0] << " (0,1): " << updateMat[0][1] << " (0,2): " << updateMat[0][2] << std::endl;
//            std::cout << "(1,0): " << updateMat[1][0] << " (1,1): " << updateMat[1][1] << " (1,2): " << updateMat[1][2] << std::endl;
//            std::cout << "(2,0): " << updateMat[2][0] << " (2,1): " << updateMat[2][1] << " (2,2): " << updateMat[2][2] << std::endl;

            currentDir = glm::vec3(updateMat[0][0],updateMat[1][0],updateMat[2][0]);
            currentLeft = glm::vec3(updateMat[0][1],updateMat[1][1],updateMat[2][1]);
            currentUp = glm::vec3(updateMat[0][2],updateMat[1][2],updateMat[2][2]);
        }
    }
}




