#include "scene.h"

Scene::Scene()
{

}

Scene::~Scene()
{

}

void Scene::loadModel(const QString filename)
{
    m_obj.read(filename);
}

GLuint Scene::loadTexture(const QString filename)
{
    QImage img(filename);
    img = QGLWidget::convertToGLFormat(img);

    texId = 0;
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//change GL_NEASREST to GL_LINEAR
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(), img.height(),
                 0, GL_RGBA, GL_UNSIGNED_BYTE, img.bits());

    glBindTexture(GL_TEXTURE_2D, 0);

    return texId;
}

void Scene::render()
{
    //bind texture
    glBindTexture(GL_TEXTURE_2D, texId);
    glEnable(GL_TEXTURE_2D);

    m_obj.draw();

    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
}



