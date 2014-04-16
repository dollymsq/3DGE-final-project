#include "openglwindow.h"
#include <QDebug>

OpenGLWindow::OpenGLWindow(QWindow *parent)
    : QWindow(parent)
    , m_update_pending(false)
    , m_animating(false)
    , m_context(0)
    , m_device(0)
    , m_frames(0)
    , m_fps(0)
{
    setSurfaceType(QWindow::OpenGLSurface);

    QSurfaceFormat format;
    format.setSamples(4); // multi-sampling
    format.setRenderableType(QSurfaceFormat::OpenGL); // change to opengles on mobile
//    format.setProfile(QSurfaceFormat::CoreProfile);
//    format.setMajorVersion(4);
//    format.setMinorVersion(3);
    format.setDepthBufferSize(1);
    setFormat(format);

    // Hide the cursor since this is a fullscreen app
    setCursor(Qt::BlankCursor);

    // ??? : not needed
    // View needs all mouse move events, not just mouse drag events
    //setMouseTracking(true);
    // View needs keyboard focus
    //setFocusPolicy(Qt::StrongFocus);
}

OpenGLWindow::~OpenGLWindow()
{
    delete m_device;
}

void OpenGLWindow::render()
{
    m_painter->beginNativePainting();
    renderOpenGL();
    m_painter->endNativePainting();

    m_painter->setPen(Qt::red);
    m_painter->setFont(QFont("Monospace", 11));
    m_painter->drawText(QRect(0,0,100,100), Qt::AlignCenter, "FPS: " + QString::number(m_fps));

    m_painter->end();
}

void OpenGLWindow::renderOpenGL()
{

}

void OpenGLWindow::onTick(const float seconds)
{
    Q_UNUSED(seconds);

    ++m_frames;

    if (m_fpsTimer.elapsed() > 1000) {
        m_fps = glm::round((m_frames * 1000.0f) / m_fpsTimer.elapsed());
        m_fpsTimer.restart();
        m_frames = 0;
        qDebug() << "FPS:" << m_fps;
    }
}

void OpenGLWindow::renderLater()
{
    if (!m_update_pending) {
        m_update_pending = true;
        QCoreApplication::postEvent(this, new QEvent(QEvent::UpdateRequest));
    }
}

bool OpenGLWindow::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::UpdateRequest:
        m_update_pending = false;
        renderNow();
        return true;
    default:
        return QWindow::event(event);
    }
}

void OpenGLWindow::exposeEvent(QExposeEvent *event)
{
    Q_UNUSED(event);

    if (isExposed())
        renderNow();
}

void OpenGLWindow::renderNow()
{
    if (!isExposed())
        return;

    // initialize opengl context

    bool needsInitialize = false;

    if (!m_context) {
        m_context = new QOpenGLContext(this);
        m_context->setFormat(requestedFormat());
        m_context->create();

        needsInitialize = true;

        m_tickTimer.start();
        m_fpsTimer.start();
    }

    m_context->makeCurrent(this);

    if (needsInitialize) {
        initializeOpenGLFunctions();
        initialize();
    }

    // check initialize qpainter context

    if (!m_device) {
        m_device = new QOpenGLPaintDevice;
        m_device->setSize(size());

        m_painter = new QPainter(m_device);
    }

    if (!m_painter->isActive()) {
        m_painter->begin(m_device);
    }

    // render

    float seconds = m_tickTimer.restart() * 0.001f;
    if (seconds > 1.0f) seconds = 1.0f;

    onTick(seconds);
    render();

    m_context->swapBuffers(this);

    if (m_animating)
        renderLater();
}


void OpenGLWindow::setAnimating(bool animating)
{
    m_animating = animating;

    if (animating)
        renderLater();
}

