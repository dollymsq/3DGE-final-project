#include "window.h"

Window::Window()
    : m_resetMouse(true)
    , m_program(0)
{

}

Window::~Window()
{

}

// TODO: move shaders to resources

static const char *vertexShaderSource =
    "#ifdef GL_ES\n"
    "precision highp float;\n"
    "precision highp int;\n"
    "precision lowp sampler2D;\n"
    "#endif\n"
    "attribute highp vec3 a_position;\n"
    ""
    "attribute lowp vec2 a_texcoord;\n"
    "varying lowp vec2 v_texcoord;\n"
    ""
    "uniform highp mat4 matrix;\n"
    ""
    "void main() {\n"
    "   v_texcoord = a_texcoord;\n"
    "   gl_Position = matrix * vec4(a_position, 1);\n"
    "}\n";

static const char *fragmentShaderSource =
    "#ifdef GL_ES\n"
    "precision highp float;\n"
    "precision highp int;\n"
    "precision lowp sampler2D;\n"
    "#endif\n"
    "uniform lowp sampler2D texture;\n"
    "varying lowp vec2 v_texcoord;\n"
    ""
    "void main() {\n"
    "   vec4 c = texture2D(texture, v_texcoord);\n"
    "   gl_FragColor = c;\n"
    "}\n";

GLuint Window::loadShader(GLenum type, const char *source)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, 0);
    glCompileShader(shader);
    return shader;
}

void Window::onTick(const float seconds)
{
    OpenGLWindow::onTick(seconds);
    m_world.update(seconds);
}

void Window::initialize()
{
    glClearColor(1,1,1,1);

    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Enable back face culling
    glEnable(GL_CULL_FACE);

    // camera
    m_camera.setAspectRatio(width()/height());

    // world
    m_world.initialize(width(), height());

    // shaders
    m_program = new QOpenGLShaderProgram(this);
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    m_program->link();

    m_posAttr = m_program->attributeLocation("a_position");
    m_texAttr = m_program->attributeLocation("a_texcoord");

    m_matrixUniform = m_program->uniformLocation("matrix");
    m_textureUniform = m_program->uniformLocation("texture");

    // textures

    QImage img(":res/textures/level_plain.png");

    if (img.isNull()) {
        qCritical("Unable to load texture!");
        return;
    }

    img = img.convertToFormat(QImage::Format_RGBA8888);

    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(), img.height(),
                 0, GL_RGBA, GL_UNSIGNED_BYTE, img.bits());

    getErrors("initialize");
}

void Window::getErrors(QString location) {
#ifndef QT_NO_DEBUG_OUTPUT

    GLenum error = glGetError();

    if (error != GL_NO_ERROR)
        qDebug() << "OpenGL Error: " + location;

    switch(error) {
    case GL_INVALID_ENUM:
        qDebug() << "GL_INVALID_ENUM";
        break;
    case GL_INVALID_VALUE:
        qDebug() << "GL_INVALID_VALUE";
        break;
    case GL_INVALID_OPERATION:
        qDebug() << "GL_INVALID_OPERATION";
        break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
        qDebug() << "GL_INVALID_FRAMEBUFFER_OPERATION";
        break;
    case GL_OUT_OF_MEMORY:
        qDebug() << "GL_OUT_OF_MEMORY";
        break;
    }

#endif
}

void Window::renderOpenGL()
{

    getErrors("rendering START");

    const qreal retinaScale = devicePixelRatio();
    glViewport(0, 0, width() * retinaScale, height() * retinaScale);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    m_world.draw();

    getErrors("rendering END");



//    m_program->bind();

//    glm::mat4 mvp = m_camera.pMatrix * m_camera.vMatrix * glm::rotate(glm::mat4(1.0f),glm::radians(270.0f),glm::vec3(1.0f, 0.0f, 0.0f));
//    glUniformMatrix4fv(m_matrixUniform, 1, GL_FALSE, glm::value_ptr(mvp));

//    m_program->setUniformValue("texture", 0);

//    m_world.drawWorld(m_program);

//    glDisableVertexAttribArray(m_posAttr);
//    glDisableVertexAttribArray(m_texAttr);

//    m_program->release();

//    getErrors("rendering END");
}

void Window::mousePressEvent(QMouseEvent *event)
{
    m_world.mousePressEvent(event);
}

void Window::mouseMoveEvent(QMouseEvent *event)
{
    // This starter code implements mouse capture, which gives the change in
    // mouse position since the last mouse movement.

    // Note that it is important to check that deltaX and
    // deltaY are not zero before recentering the mouse, otherwise there will
    // be an infinite loop of mouse move events.
    m_world.mouseMoveEvent(event);

//    QPoint viewCenter(width() / 2, height() / 2);

//    if (m_resetMouse) {
//        QCursor::setPos(mapToGlobal(viewCenter));
//        if (event->x() == viewCenter.x() && event->y() == viewCenter.y())
//            m_resetMouse = false;
//        return;
//    }

//    int deltaX = event->x() - viewCenter.x();
//    int deltaY = event->y() - viewCenter.y();

//    if (!deltaX && !deltaY) return;
//    QCursor::setPos(mapToGlobal(viewCenter));

//    m_camera.mouseRotation(glm::vec2(deltaX, deltaY));
}

void Window::mouseReleaseEvent(QMouseEvent *event)
{
    m_world.mouseReleaseEvent(event);
}

void Window::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) QWindow::close();

    m_world.keyPressEvent(event);

    switch(event->key()) {
    case Qt::Key_W:
        m_camera.pressingForward = true;
        break;
    case Qt::Key_S:
        m_camera.pressingBackward = true;
        break;
    case Qt::Key_A:
        m_camera.pressingLeft = true;
        break;
    case Qt::Key_D:
        m_camera.pressingRight = true;
        break;
    }

    if (event->key() == Qt::Key_Space) m_camera.pressingJump = true;
}

void Window::keyReleaseEvent(QKeyEvent *event)
{
    m_world.keyReleaseEvent(event);

    switch(event->key()) {
    case Qt::Key_W:
        m_camera.pressingForward = false;
        break;
    case Qt::Key_S:
        m_camera.pressingBackward = false;
        break;
    case Qt::Key_A:
        m_camera.pressingLeft = false;
        break;
    case Qt::Key_D:
        m_camera.pressingRight = false;
        break;
    }

    if (event->key() == Qt::Key_Space) m_camera.pressingJump = false;
}
