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
    void initWorld();

    GLuint m_vboIds[2];
};

#endif // WORLD_H
