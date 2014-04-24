#ifndef OPENGLWINDOW_H
#define OPENGLWINDOW_H

#include "graphics/opengl.h"
#include <QtCore/QCoreApplication>
#include <QtGui/QWindow>
#include <QtGui/QOpenGLContext>
#include <QOpenGLDebugMessage>
#include <QtGui/QOpenGLPaintDevice>
#include <QtGui/QPainter>

#include <QElapsedTimer>

#include <glm/ext.hpp>
#include <QDebug>

class OpenGLWindow : public QWindow
{
    Q_OBJECT

public:
    explicit OpenGLWindow(QWindow *parent = 0);
    ~OpenGLWindow();

    virtual void renderOpenGL() =0;

    virtual void initialize() =0;

    void setAnimating(bool animating);

    void showSubtitles(QString &info);
    void showPermanentStat(QString &info);

    GLFunctions *m_gl;
    QOpenGLContext *m_context;
public slots:
    void renderLater();
    void renderNow();

protected:
//    virtual void render(QPainter *painter);
    virtual void render();
    virtual void onTick(const float seconds) =0;

    bool event(QEvent *event);
    void exposeEvent(QExposeEvent *event);
    QPainter *m_painter;


private:
    bool m_update_pending;
    bool m_animating;

    QOpenGLDebugLogger *m_logger;
    QOpenGLPaintDevice *m_device;

    QElapsedTimer m_tickTimer;
    QElapsedTimer m_fpsTimer;
    int m_frames;
    int m_fps;

    void handleLogMessage(const QOpenGLDebugMessage &debugMessage);
};

#endif // OPENGLWINDOW_H
