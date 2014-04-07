#include "world.h"

World::World()
    : m_numCubes(0)
{

}

void World::init()
{
    initializeOpenGLFunctions();

    glGenBuffers(2, m_vboIds);

    initWorld();
}

void World::drawWorld(QOpenGLShaderProgram *program)
{
    glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIds[1]);

    // offset
    quintptr offset = 0;

    // position
    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (const void *)offset);

    // offset
    offset += sizeof(glm::vec3);

    // textures
    int texcoordLocation = program->attributeLocation("a_texcoord");
    program->enableAttributeArray(texcoordLocation);
    glVertexAttribPointer(texcoordLocation, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (const void *)offset);

    // draw
    glDrawElements(GL_TRIANGLE_STRIP, 34 * m_numCubes, GL_UNSIGNED_SHORT, 0); //vbo
}


void World::addCube(int x, int y, int z, QVector<VertexData> &vertices, QVector<GLushort> &indices)
{
    vertices.append({glm::vec3(-1.0f + x, -1.0f + y,  1.0f + z), glm::vec2(0.0f, 0.0f)});
    vertices.append({glm::vec3( 1.0f + x, -1.0f + y,  1.0f + z), glm::vec2(0.33f, 0.0f)});
    vertices.append({glm::vec3(-1.0f + x,  1.0f + y,  1.0f + z), glm::vec2(0.0f, 0.5f)});
    vertices.append({glm::vec3( 1.0f + x,  1.0f + y,  1.0f + z), glm::vec2(0.33f, 0.5f)});

    vertices.append({glm::vec3( 1.0f + x, -1.0f + y,  1.0f + z), glm::vec2( 0.0f, 0.5f)});
    vertices.append({glm::vec3( 1.0f + x, -1.0f + y, -1.0f + z), glm::vec2(0.33f, 0.5f)});
    vertices.append({glm::vec3( 1.0f + x,  1.0f + y,  1.0f + z), glm::vec2(0.0f, 1.0f)});
    vertices.append({glm::vec3( 1.0f + x,  1.0f + y, -1.0f + z), glm::vec2(0.33f, 1.0f)});

    vertices.append({glm::vec3( 1.0f + x, -1.0f + y, -1.0f + z), glm::vec2(0.66f, 0.5f)});
    vertices.append({glm::vec3(-1.0f + x, -1.0f + y, -1.0f + z), glm::vec2(1.0f, 0.5f)});
    vertices.append({glm::vec3( 1.0f + x,  1.0f + y, -1.0f + z), glm::vec2(0.66f, 1.0f)});
    vertices.append({glm::vec3(-1.0f + x,  1.0f + y, -1.0f + z), glm::vec2(1.0f, 1.0f)});

    vertices.append({glm::vec3(-1.0f + x, -1.0f + y, -1.0f + z), glm::vec2(0.66f, 0.0f)});
    vertices.append({glm::vec3(-1.0f + x, -1.0f + y,  1.0f + z), glm::vec2(1.0f, 0.0f)});
    vertices.append({glm::vec3(-1.0f + x,  1.0f + y, -1.0f + z), glm::vec2(0.66f, 0.5f)});
    vertices.append({glm::vec3(-1.0f + x,  1.0f + y,  1.0f + z), glm::vec2(1.0f, 0.5f)});

    vertices.append({glm::vec3(-1.0f + x, -1.0f + y, -1.0f + z), glm::vec2(0.33f, 0.0f)});
    vertices.append({glm::vec3( 1.0f + x, -1.0f + y, -1.0f + z), glm::vec2(0.66f, 0.0f)});
    vertices.append({glm::vec3(-1.0f + x, -1.0f + y,  1.0f + z), glm::vec2(0.33f, 0.5f)});
    vertices.append({glm::vec3( 1.0f + x, -1.0f + y,  1.0f + z), glm::vec2(0.66f, 0.5f)});

    vertices.append({glm::vec3(-1.0f + x,  1.0f + y,  1.0f + z), glm::vec2(0.33f, 0.5f)});
    vertices.append({glm::vec3( 1.0f + x,  1.0f + y,  1.0f + z), glm::vec2(0.66f, 0.5f)});
    vertices.append({glm::vec3(-1.0f + x,  1.0f + y, -1.0f + z), glm::vec2(0.33f, 1.0f)});
    vertices.append({glm::vec3( 1.0f + x,  1.0f + y, -1.0f + z), glm::vec2(0.66f, 1.0f)});

    GLuint offset = 0;
    if (!indices.isEmpty())
        offset = indices.back() + 1;

    indices <<
         0 + offset <<  1 + offset <<  2 + offset <<  3 + offset <<  3 + offset <<
         4 + offset <<  4 + offset <<  5 + offset <<  6 + offset <<  7 + offset <<  7 + offset <<
         8 + offset <<  8 + offset <<  9 + offset << 10 + offset << 11 + offset << 11 + offset <<
        12 + offset << 12 + offset << 13 + offset << 14 + offset << 15 + offset << 15 + offset <<
        16 + offset << 16 + offset << 17 + offset << 18 + offset << 19 + offset << 19 + offset <<
        20 + offset << 20 + offset << 21 + offset << 22 + offset << 23 + offset
    ;
}

void World::initWorld()
{
    QVector<VertexData> vertices;
    QVector<GLushort> indices;

    addCube(0,0,0, vertices, indices);

    // vertices + texture
    glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexData), vertices.data(), GL_STATIC_DRAW);

    // indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIds[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), indices.data(), GL_STATIC_DRAW);
}
