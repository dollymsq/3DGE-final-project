#include <QVector>
#include <qgl.h>
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
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

void Tree::draw() const {
    gl->glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
    gl->glEnableClientState(GL_VERTEX_ARRAY);
    gl->glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    gl->glEnableClientState(GL_NORMAL_ARRAY);

    GLuint stride = sizeof(float)*(3+3+2);
    gl->glVertexPointer(3,GL_FLOAT,stride, (void*) 0);
    gl->glNormalPointer(GL_FLOAT,stride,(void*) (3*sizeof(float)));
    gl->glTexCoordPointer(2,GL_FLOAT,stride,(void*) (6*sizeof(float)));

    gl->glDrawArrays(GL_TRIANGLES,0,m_bufferSize);
    gl->glBindBuffer(GL_ARRAY_BUFFER,0);
}

void Tree::generate(QString L)  {
    QVector<LEvent> eventStack;
    glm::vec3 currentDir = glm::vec3(0,1,0);
    glm::vec3 currentUp = glm::vec3(1,0,0);
    glm::vec3 currentLeft = glm::vec3(0,0,1);
    glm::vec3 currentPos = glm::vec3(0,0,0);
    glm::vec3 prevDir = currentDir;
    QPair<glm::vec3,glm::vec3> currentVecs;
    currentVecs.first = currentDir;
    currentVecs.second = currentUp;
    int currPointer = 0;
    float currentThickness = 22;
    float prevThickness = currentThickness;
    QPair<Command,float> currCommand;
    currCommand.first = CONTINUE;
    glm::mat3 rotation;

    QVector<glm::mat4> currentCTMStack;

    while(currCommand.first != STOP)  {
        currCommand = LParser::parseString(L,currPointer);
        bool toRotate = false;
        float param = currCommand.second;
        LEvent e,ePush;
        switch(currCommand.first) {
        case F:
            m_points.append(currentPos);
            m_thick.append(prevThickness);
            m_vecs.append(currentVecs);
            m_prevDirs.append(prevDir);
            currentPos += currentDir*param;
            m_points.append(currentPos);
            m_thick.append(currentThickness);
            m_vecs.append(currentVecs);
            m_prevDirs.append(prevDir);
            prevDir = currentDir;
            break;
        case f:
            currentPos += currentDir*param;
            break;
        case YAW:
            rotation = glm::mat3(cos(glm::radians(param)),sin(glm::radians(param)),0,
                                 -sin(glm::radians(param)),cos(glm::radians(param)),0,
                                 0,0,1);
            toRotate = true;

            break;
        case NEGYAW:
            rotation = glm::mat3(cos(-glm::radians(param)),sin(-glm::radians(param)),0,
                                 -sin(-glm::radians(param)),cos(-glm::radians(param)),0,
                                 0,0,1);
            currentCTMStack += glm::rotate(glm::mat4(1.f),(float)(-(glm::radians(param))),currentUp);
            toRotate = true;
            break;
        case ROLL:
            rotation = glm::mat3(1,0,0,
                                 0,cos(glm::radians(param)),-sin(glm::radians(param)),
                                 0,sin(glm::radians(param)),cos(glm::radians(param)));
            toRotate = true;
            break;
        case NEGROLL:
            rotation = glm::mat3(1,0,0,
                                 0,cos(-glm::radians(param)),-sin(-glm::radians(param)),
                                 0,sin(-glm::radians(param)),cos(-glm::radians(param)));
            toRotate = true;
            break;
        case PITCH:
            rotation = glm::mat3(cos(glm::radians(param)),0,-sin(glm::radians(param)),
                                 0,1,0,
                                 sin(glm::radians(param)),0,cos(glm::radians(param)));
            toRotate = true;
            break;
        case NEGPITCH:
            rotation = glm::mat3(cos(-glm::radians(param)),0,-sin(-glm::radians(param)),
                                 0,1,0,
                                 sin(-glm::radians(param)),0,cos(-glm::radians(param)));
            toRotate = true;
            break;
        case LINEWIDTH:
            prevThickness = currentThickness;
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
            prevThickness = e.prevThickness;
            currentVecs = e.currentVecs;
            prevDir = e.prevDir;
            break;
        case PUSH:
            ePush.currentDir = currentDir;
            ePush.currentLeft = currentLeft;
            ePush.currentPos = currentPos;
            ePush.currentUp = currentUp;
            ePush.currentThickness = currentThickness;
            ePush.prevThickness = prevThickness;
            ePush.currentVecs = currentVecs;
            ePush.prevDir = prevDir;
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
        currentVecs.first = currentDir;
        currentVecs.second = currentUp;
    }
    generateVBO();
}

void Tree::generateVBOOld()  {
    QVector<float> currentVBO;
    Obj cylinder("cylinder_meshBetter.obj");
    glm::mat4 CTM;
    for(int i = 0; i < m_points.size(); i+=2)  {

        CTM = glm::translate(CTM,m_pos);
        CTM = glm::translate(CTM,(m_points.at(i+1) + m_points.at(i))/2.0f);

        CTM *= glm::orientation(glm::normalize(m_points.at(i+1) - m_points.at(i)), glm::vec3(0,1,0));

        glm::vec3 scaleVec = glm::vec3(m_thick.at(i)/10.0f,glm::length((m_points.at(i+1) - m_points.at(i))),m_thick.at(i)/10.0f);
        CTM = glm::scale(CTM,scaleVec);

        currentVBO += cylinder.transform(CTM);
    }
    m_bufferSize = currentVBO.size()/8.0f;
    gl->glGenBuffers(1, &m_buffer);
    gl->glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
    gl->glBufferData(GL_ARRAY_BUFFER, currentVBO.size()*sizeof(float), currentVBO.data(), GL_STATIC_DRAW);
}

void Tree::generateVBO()  {
    int SMOOTH_ITER = 24;
    QVector<float> currentVBO;
    float epsilon = .01;
    for(int i = 0; i < m_points.size(); i+=2)  {
        QVector<float> VBOAdd;
        QVector<glm::vec3> forwardPoints,backPoints,normals;
        QPair<glm::vec3,glm::vec3> currentVecs = m_vecs.at(i);
        glm::vec3 forwardDir,backDir,tempDir;
        tempDir = m_prevDirs.at(i);
        float angleBetweenDirs = glm::angle(currentVecs.first,tempDir);
        glm::vec3 left;
        if(fabs(angleBetweenDirs) < 1e-6) left = glm::cross(currentVecs.second,currentVecs.first);
        else left = glm::cross(currentVecs.first,tempDir);
        tempDir = glm::rotate(currentVecs.second,-1.0f*angleBetweenDirs,left);
        float forwardThickness,backThickness;
        forwardThickness = m_thick.at(i+1)/10.0f;
        backThickness = m_thick.at(i)/10.0f;
        float toAdd = 2.0f*M_PI/SMOOTH_ITER;
        for(int j = 0; j < SMOOTH_ITER; j++)  {
            float angle = toAdd*j;
            forwardDir = glm::normalize(glm::rotate(currentVecs.second,angle,currentVecs.first));
            backDir = glm::normalize(glm::rotate(tempDir,angle,m_prevDirs.at(i)));
            forwardPoints.append(m_points.at(i+1) + forwardDir*forwardThickness);
            backPoints.append(m_points.at(i) + backDir*backThickness - currentVecs.first*epsilon);
            normals.append(forwardDir);
        }
        for(int j = 0; j < SMOOTH_ITER; j++)  {
            int secondInd = (j+1) % SMOOTH_ITER;
            QVector<float> aVBO,bVBO,cVBO,dVBO,eVBO;
            glm::vec3 a,b,c,d,e,firstNormal,secondNormal;
            glm::vec2 aTex,bTex,cTex,dTex,eTex;
            a = backPoints.at(secondInd);
            b = backPoints.at(j);
            c = forwardPoints.at(j);
            d = forwardPoints.at(secondInd);
            e = m_points.at(i+1) + (currentVecs.first*forwardThickness/2.0f);

            PxVec3 physA,physB,physC,physD,physE;
            physA = PxVec3(a.x,a.y,a.z);
            physB = PxVec3(b.x,b.y,b.z);
            physC = PxVec3(c.x,c.y,c.z);
            physD = PxVec3(d.x,d.y,d.z);
            physE = PxVec3(e.x,e.y,e.z);

            aTex = glm::vec2(((float)secondInd)/((float)SMOOTH_ITER),0.0f);
            bTex = glm::vec2(((float)j)/((float)SMOOTH_ITER),0.0f);
            cTex = glm::vec2(((float)j)/((float)SMOOTH_ITER),1.0f);
            dTex = glm::vec2(((float)secondInd)/((float)SMOOTH_ITER),1.0f);
            eTex = glm::vec2(.5,.5);
            firstNormal = normals.at(j);
            secondNormal = normals.at(secondInd);
            aVBO.append(a.x);aVBO.append(a.y);aVBO.append(a.z);aVBO.append(secondNormal.x);aVBO.append(secondNormal.y);aVBO.append(secondNormal.z);aVBO.append(aTex.x);aVBO.append(aTex.y);
            bVBO.append(b.x);bVBO.append(b.y);bVBO.append(b.z);bVBO.append(firstNormal.x);bVBO.append(firstNormal.y);bVBO.append(firstNormal.z);bVBO.append(bTex.x);bVBO.append(bTex.y);
            cVBO.append(c.x);cVBO.append(c.y);cVBO.append(c.z);cVBO.append(firstNormal.x);cVBO.append(firstNormal.y);cVBO.append(firstNormal.z);cVBO.append(cTex.x);cVBO.append(cTex.y);
            dVBO.append(d.x);dVBO.append(d.y);dVBO.append(d.z);dVBO.append(secondNormal.x);dVBO.append(secondNormal.y);dVBO.append(secondNormal.z);dVBO.append(dTex.x);dVBO.append(dTex.y);
            eVBO.append(e.x);eVBO.append(e.y);eVBO.append(e.z);eVBO.append(currentVecs.first.x);eVBO.append(currentVecs.first.y);eVBO.append(currentVecs.first.z);eVBO.append(eTex.x);eVBO.append(eTex.y);

            VBOAdd += (aVBO + dVBO + bVBO);
            VBOAdd += (bVBO + dVBO + cVBO);
            VBOAdd += (dVBO + eVBO + cVBO);

            m_vertices.append(physA);
            m_vertices.append(physB);
            m_vertices.append(physC);
            m_vertices.append(physD);
            m_vertices.append(physE);

            int aInd,bInd,cInd,dInd,eInd;
            aInd = m_vertices.size() - 5;
            bInd = m_vertices.size() - 4;
            cInd = m_vertices.size() - 3;
            dInd = m_vertices.size() - 2;
            eInd = m_vertices.size() - 1;

            m_inds.append(aInd);m_inds.append(dInd);m_inds.append(bInd);
            m_inds.append(bInd);m_inds.append(dInd);m_inds.append(cInd);
            m_inds.append(dInd);m_inds.append(eInd);m_inds.append(cInd);

//            m_vertices.append(physB);
//            m_vertices.append(physD);
//            m_vertices.append(physC);

//            m_vertices.append(physD);
//            m_vertices.append(physE);
//            m_vertices.append(physC);
        }
        currentVBO += VBOAdd;
    }

    m_bufferSize = currentVBO.size()/8.0f;
    gl->glGenBuffers(1, &m_buffer);
    gl->glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
    gl->glBufferData(GL_ARRAY_BUFFER, currentVBO.size()*sizeof(float), currentVBO.data(), GL_STATIC_DRAW);
}




