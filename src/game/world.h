#ifndef WORLD_H
#define WORLD_H

#include <QGLWidget>
#include <QMenuBar>
#include <GL/glu.h>
#include <QTime>
#include <QTimer>

#include "scene/camera.h"
#include "math/vector.h"

/*
 * This class is an abstract superclass for a game world that contains
 * a camera and can draw on a canvas.
 */

class View;
class World
{
public:
    World(View *v);
    virtual ~World();
    virtual GLuint loadTexture(const QString &filename);
    View *m_v;
    virtual void updateCamera()=0;
    virtual void updateStuff(float seconds)=0;
    virtual void mousePressEvent(QMouseEvent *event)=0;
    virtual void mouseMoveEvent(QMouseEvent *event)=0;
    virtual void mouseReleaseEvent(QMouseEvent *event)=0;
    virtual void keyPressEvent(QKeyEvent *event)=0;
    virtual void keyReleaseEvent(QKeyEvent *event)=0;
    virtual void draw()=0;
    virtual void initialize()=0;
};

#endif // WORLD_H
