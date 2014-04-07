#include "oldworld.h"
#include <QFile>
#include "scene/view.h"

OldWorld::OldWorld(View *v) {
    m_v = v;
}

OldWorld::~OldWorld()  {}

/*
 * Loads a texture!
 */
GLuint OldWorld::loadTexture(const QString &filename) {
    QFile file(filename);
    if (!file.exists())
        return -1;

    // Load the file into memory
    QImage image;
    image.load(file.fileName());
    image = image.mirrored(false, true);

    GLuint id;
    id = m_v->bindTexture(image,GL_TEXTURE_2D, GL_RGBA, QGLContext::MipmapBindOption);
    return id;
}
