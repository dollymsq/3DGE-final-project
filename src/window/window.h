#ifndef WINDOW_H
#define WINDOW_H

#include "window/openglwindow.h"
#include "game/world.h"

#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QMouseEvent>
#include <QScreen>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>

#include <PxPhysicsAPI.h>
using namespace physx;

class Window : public OpenGLWindow
{
public:
    Window();
    ~Window();

    void initialize();
    virtual void renderOpenGL();

private:

    // Camera
    Camera m_camera;

    // World
    World m_world;

    GLuint m_posAttr;
    GLuint m_texAttr;
    GLuint m_matrixUniform;
    GLuint m_textureUniform;
    GLuint m_texture;

    bool m_resetMouse;

    virtual void onTick(const float seconds);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
};

#endif // WINDOW_H
