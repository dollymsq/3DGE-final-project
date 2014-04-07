#ifndef OPENGLWINDOW_H
#define OPENGLWINDOW_H

#include <QtCore/QCoreApplication>
#include <QtGui/QWindow>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLContext>

#include <QtGui/QOpenGLPaintDevice>
#include <QtGui/QPainter>

#include <QElapsedTimer>

#include <glm/ext.hpp>

class OpenGLWindow : public QWindow, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit OpenGLWindow(QWindow *parent = 0);
    ~OpenGLWindow();

    virtual void renderOpenGL() =0;

    virtual void initialize() =0;

    void setAnimating(bool animating);

public slots:
    void renderLater();
    void renderNow();

protected:
//    virtual void render(QPainter *painter);
    virtual void render();
    virtual void onTick(const float seconds) =0;

    bool event(QEvent *event);
    void exposeEvent(QExposeEvent *event);

private:
    bool m_update_pending;
    bool m_animating;

    QOpenGLContext *m_context;
    QOpenGLPaintDevice *m_device;
    QPainter *m_painter;

    QElapsedTimer m_tickTimer;
    QElapsedTimer m_fpsTimer;
    int m_frames;
    int m_fps;

};

#endif // OPENGLWINDOW_H
