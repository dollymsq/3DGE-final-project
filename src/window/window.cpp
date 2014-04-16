#include "window.h"

Window::Window()
    : m_resetMouse(true)
{

}

Window::~Window()
{

}

void Window::onTick(const float seconds)
{
    OpenGLWindow::onTick(seconds);
    m_camera.update(seconds);
    m_world.tick(seconds);
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
    m_world.init();


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

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glLoadMatrixf(glm::value_ptr(m_camera.pMatrix));
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glLoadMatrixf(glm::value_ptr(m_camera.vMatrix));

    m_world.draw();

    getErrors("rendering END");
}

void Window::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
}

void Window::mouseMoveEvent(QMouseEvent *event)
{
    // This starter code implements mouse capture, which gives the change in
    // mouse position since the last mouse movement.

    // Note that it is important to check that deltaX and
    // deltaY are not zero before recentering the mouse, otherwise there will
    // be an infinite loop of mouse move events.
    QPoint viewCenter(width() / 2, height() / 2);

    if (m_resetMouse) {
        QCursor::setPos(mapToGlobal(viewCenter));
        if (event->x() == viewCenter.x() && event->y() == viewCenter.y())
            m_resetMouse = false;
        return;
    }

    int deltaX = event->x() - viewCenter.x();
    int deltaY = event->y() - viewCenter.y();

    if (!deltaX && !deltaY) return;
    QCursor::setPos(mapToGlobal(viewCenter));

    m_camera.mouseRotation(glm::vec2(deltaX, deltaY));
}

void Window::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
}

void Window::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) QWindow::close();

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


    if (event->key() == Qt::Key_Space) {
        PxTransform transform;
        PxVec3 dir(m_camera.m_lookAt.x, m_camera.m_lookAt.y,m_camera.m_lookAt.z);
        PxVec3 eye(m_camera.m_position.x, m_camera.m_position.y,m_camera.m_position.z);

        dir = dir - eye;
        dir.normalize();

        PxVec3 viewY = dir.cross(PxVec3(0,1,0));

        if(viewY.normalize()<1e-6f) {
            transform = PxTransform(eye);
        } else {
            PxMat33 m(dir.cross(viewY), viewY, -dir);
            transform = PxTransform(eye, PxQuat(m));
        }

        m_world.createDynamic(transform, PxSphereGeometry(3.0f), dir*100);
    }
}

void Window::keyReleaseEvent(QKeyEvent *event)
{
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
