#ifndef SCENE_H
#define SCENE_H

#include <stdio.h>
#include <string.h>
#include <vector.h>
#include <QHash>
#include <qgl.h>
#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

#include <QtOpenGL>
#include <QFile>
#include <QTextStream>
#include <iostream>
#include "obj.h"


class Scene
{
public:
    Scene();
    ~Scene();

    GLuint loadTexture(const QString filename);
    void loadModel(const QString filename);

    void render();

    OBJ m_obj;

private:
    GLuint texId;



};

#endif // SCENE_H
