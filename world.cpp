#include "world.h"
#include <QFile>
#include "view.h"

World::World(View *v) {
    m_v = v;
}

World::~World()  {}

/*
 * Loads a texture!
 */
GLuint World::loadTexture(const QString &filename) {
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
