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
//    m_camera.update(seconds);
    m_world.tick(seconds);
}

void Window::initialize()
{
    glClearColor(1,1,1,1);

    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Enable back face culling
    glEnable(GL_CULL_FACE);

    // world
    m_world.init(width()/height());
}

void Window::renderOpenGL()
{

    const qreal retinaScale = devicePixelRatio();
    glViewport(0, 0, width() * retinaScale, height() * retinaScale);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glLoadMatrixf(glm::value_ptr(m_world.getPMatrix()));
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glLoadMatrixf(glm::value_ptr(m_world.getVMatrix()));

    m_world.draw(m_painter);

}

void Window::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        m_world.shootDynamic();
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

    m_world.rotateMouse(glm::vec2(deltaX, deltaY));
//    m_camera.mouseRotation(glm::vec2(deltaX, deltaY));
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
        m_world.enableForward(true);
        break;
    case Qt::Key_S:
        m_world.enableBackward(true);
        break;
    case Qt::Key_A:
        m_world.enableLeft(true);
        break;
    case Qt::Key_D:
        m_world.enableRight(true);
        break;
    }

//    if (event->key() == Qt::Key_Space)
//        m_world.shootDynamic();
}

void Window::keyReleaseEvent(QKeyEvent *event)
{
    switch(event->key()) {
    case Qt::Key_W:
        m_world.enableForward(false);
        break;
    case Qt::Key_S:
        m_world.enableBackward(false);
        break;
    case Qt::Key_A:
        m_world.enableLeft(false);
        break;
    case Qt::Key_D:
        m_world.enableRight(false);
        break;
    }
}
