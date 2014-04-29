#include "obj.h"


Obj::Obj(const QString &path)
{
    if (!read(path)) {
        qCritical("unable to load obj!");
    }
}

void Obj::draw() const
{
//    quintptr offset = 0;

//    gl->glEnableVertexAttribArray(VAO_POSITION);
//    gl->glVertexAttribPointer(VAO_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(MeshBuffer), (const void *) offset);

//    offset += sizeof(glm::vec3);

//    gl->glEnableVertexAttribArray(VAO_NORMAL);
//    gl->glVertexAttribPointer(VAO_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(MeshBuffer), (const void *) offset);

//    offset += sizeof(glm::vec3);

//    gl->glVertexAttribPointer(VAO_TEX_COORD, 3, GL_FLOAT, GL_FALSE, sizeof(MeshBuffer), (const void *) offset);
//    gl->glEnableVertexAttribArray(VAO_TEX_COORD);

    glBegin(GL_TRIANGLES);
    foreach (const Triangle &tri, triangles) {
        drawIndex(tri.a);
        drawIndex(tri.b);
        drawIndex(tri.c);
    }
    glEnd();
}

bool Obj::read(const QString &path)
{
    // Open the file
    QFile file(":/meshes/" + path);
    if (!file.open(QFile::ReadOnly | QFile::Text)) return false;
    QTextStream f(&file);
    QString line;

    // Read the file
    QRegExp spaces("\\s+");
    do {
        line = f.readLine().trimmed();
        QStringList parts = line.split(spaces);
        if (parts.isEmpty()) continue;

        if (parts[0] == "v" && parts.count() >= 4) {
            vertices += glm::vec3(parts[1].toFloat(), parts[2].toFloat(), parts[3].toFloat());
        } else if (parts[0] == "vt" && parts.count() >= 3) {
            coords += glm::vec2(parts[1].toFloat(), parts[2].toFloat());
        } else if (parts[0] == "vn" && parts.count() >= 4) {
            normals += glm::vec3(parts[1].toFloat(), parts[2].toFloat(), parts[3].toFloat());
        } else if (parts[0] == "f" && parts.count() >= 4) {
            // Convert polygons into triangle fans
            Index a = getIndex(parts[1]);
            Index b = getIndex(parts[2]);
            for (int i = 3; i < parts.count(); i++) {
                Index c = getIndex(parts[i]);
                triangles += Triangle(a, b, c);
                b = c;
            }
        }
    } while (!line.isNull());

    return true;
}

static QString str(const glm::vec2 &v) { return QString("%1 %2").arg(v.x).arg(v.y); }
static QString str(const glm::vec3 &v) { return QString("%1 %2 %3").arg(v.x).arg(v.y).arg(v.z); }

QVector<float> Obj::transform(const glm::mat4 &transform) {
    QVector<float> toReturn;
    glm::mat4 itrans = glm::inverseTranspose(transform);
    for(int i = 0; i < triangles.size(); i++)  {
        Triangle tri = triangles.at(i);

        Index a = tri.a;
        Index b = tri.b;
        Index c = tri.c;

        QVector<Index> v;
        v.append(a);
        v.append(b);
        v.append(c);

        for(int j = 0; j < 3; j++)  {
            Index curr = v.at(j);
            glm::vec3 currPoint = vertices[curr.vertex];
            glm::vec3 currNormal = normals[curr.normal];
            glm::vec2 currTex = coords[curr.coord];

            glm::vec4 currPointGLM = glm::vec4(currPoint.x,currPoint.y,currPoint.z,1);
            glm::vec4 currNormalGLM = glm::vec4(currNormal.x,currNormal.y,currNormal.z,1);

            glm::vec3 transPoint = transformPoint(currPointGLM,transform);
            toReturn.append(transPoint.x);toReturn.append(transPoint.y);toReturn.append(transPoint.z);

            glm::vec3 transNormal = transformPoint(currNormalGLM,itrans);
            toReturn.append(transNormal.x);toReturn.append(transNormal.y);toReturn.append(transNormal.z);

            toReturn.append(currTex.x);toReturn.append(currTex.y);
        }
    }
    return toReturn;
}

glm::vec3 Obj::transformPoint(const glm::vec4 &point, const glm::mat4 &transform)  {
    glm::vec4 worldPoint = transform*point;
    return glm::vec3(worldPoint.x,worldPoint.y,worldPoint.z);
}

static QString str(const Obj::Index &i)
{
    if (i.normal >= 0) {
        if (i.coord >= 0) return QString("%1/%2/%3").arg(i.vertex + 1).arg(i.coord + 1).arg(i.normal + 1);
        return QString("%1//%2").arg(i.vertex + 1).arg(i.normal + 1);
    } else {
        if (i.coord >= 0) return QString("%1/%2").arg(i.vertex + 1).arg(i.coord + 1);
        return QString("%1").arg(i.vertex + 1);
    }
}

void Obj::vbo()
{
    GLuint meshSize = triangles.size() * 3;
    QVector<MeshBuffer> data;
    data.reserve(meshSize);

    for(Obj::Triangle tri : triangles) {
        MeshBuffer a;
        a.position = vertices[tri.a.vertex];
        a.normal = normals[tri.a.normal];
        a.texcoord = coords[tri.a.coord];
        MeshBuffer b;
        b.position = vertices[tri.b.vertex];
        b.normal = normals[tri.b.normal];
        b.texcoord = coords[tri.b.coord];
        MeshBuffer c;
        c.position = vertices[tri.c.vertex];
        c.normal = normals[tri.c.normal];
        c.texcoord = coords[tri.c.coord];

        data.append(a);
        data.append(b);
        data.append(c);
    }

    gl->glGenVertexArrays(1, &m_vao);
    gl->glBindVertexArray(m_vao);

    gl->glGenBuffers(1, &m_buffer);

    gl->glBindBuffer(GL_ARRAY_BUFFER, m_buffer);

    gl->glBufferData(GL_ARRAY_BUFFER, sizeof(MeshBuffer) * data.size(), data.data(), GL_STATIC_DRAW);

}

bool Obj::write(const QString &path) const
{
    // Open the file
    QFile file(path);
    if (!file.open(QFile::WriteOnly | QFile::Text)) return false;
    QTextStream f(&file);

    // Write the file
    foreach (const glm::vec3 &vertex, vertices) f << "v " << str(vertex) << '\n';
    foreach (const glm::vec2 &coord, coords) f << "vt " << str(coord) << '\n';
    foreach (const glm::vec3 &normal, normals) f << "vn " << str(normal) << '\n';
    foreach (const Triangle &tri, triangles) f << "f " << str(tri.a) << ' ' << str(tri.b) << ' ' << str(tri.c) << '\n';

    return true;
}

inline int relativeIndex(int index, int count)
{
    return index >= 0 ? index - 1 : count + index;
}

Obj::Index Obj::getIndex(const QString &str) const
{
    QStringList parts = str.split('/');
    int vertex = parts.count() > 0 ? relativeIndex(parts[0].toInt(), vertices.count()) : -1;
    int coord = parts.count() > 1 ? relativeIndex(parts[1].toInt(), coords.count()) : -1;
    int normal = parts.count() > 2 ? relativeIndex(parts[2].toInt(), normals.count()) : -1;
    return Index(vertex, coord, normal);
}

void Obj::drawIndex(const Index &index) const
{
    if (index.coord >= 0 && index.coord < coords.count()) glTexCoord2fv(glm::value_ptr(coords[index.coord]));
    if (index.normal >= 0 && index.normal < normals.count()) glNormal3fv(glm::value_ptr(normals[index.normal]));
    if (index.vertex >= 0 && index.vertex < vertices.count()) glVertex3fv(glm::value_ptr(vertices[index.vertex]));
}

QVector<PxVec3> Obj::getVerts()  {
    QVector<PxVec3> verts;
    for(int i = 0; i < vertices.size(); i++)  {
        glm::vec3 currentVec = vertices[i];
        verts.append(PxVec3(currentVec.x,currentVec.y,currentVec.z));
    }
    return verts;
}

QVector<PxU32> Obj::getInds()  {
    QVector<PxU32> inds;
    for(int i = 0; i < triangles.size(); i++)  {
        inds.append(triangles[i].a.vertex);
        inds.append(triangles[i].b.vertex);
        inds.append(triangles[i].c.vertex);
    }
    return inds;
}
