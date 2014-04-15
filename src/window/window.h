#ifndef WINDOW_H
#define WINDOW_H

#include "openglwindow.h"
#include "scene/camera.h"
#include "game/platworld.h"

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
    PlatWorld m_world;

    inline void getErrors(QString location);
    GLuint loadShader(GLenum type, const char *source);

    GLuint m_posAttr;
    GLuint m_texAttr;
    GLuint m_matrixUniform;
    GLuint m_textureUniform;
    GLuint m_texture;

    bool m_resetMouse;

    QOpenGLShaderProgram *m_program;

    virtual void onTick(const float seconds);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

//public slots:
//    void showSubtitles(QString info);

};

#endif // WINDOW_H
