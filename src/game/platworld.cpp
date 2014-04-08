#include "platworld.h"

#include <QDebug>

PlatWorld::PlatWorld() {
}

PlatWorld::~PlatWorld()  {
    delete m_camera;
    delete m_player;
    delete m_ball;
    delete m_enemy;
    delete m_objectReader;
//    delete m_navReader;
}

void PlatWorld::initialize(int w, int h)  {
    initializeOpenGLFunctions();

    m_viewSize.x = w;
    m_viewSize.y = h;

    glEnable(GL_TEXTURE_2D);
    glEnable (GL_DEPTH_TEST);
    m_levelPath = QString::fromStdString("level_plain");
    m_objectReader = new OBJ();
//    m_navReader = new OBJ();

    m_camera = new Camera();
    m_camera->eye.x = 0.0f, m_camera->eye.y = 33.0f, m_camera->eye.z = 0.0f;
    m_camera->center.x = 0.0f, m_camera->center.y = 33.0f, m_camera->center.z = 1.0f;
    m_camera->up.x = 0.0f, m_camera->up.y = 1.0f, m_camera->up.z = 0.0f;
    m_camera->angle = 45.0f, m_camera->near = .01f, m_camera->far = 1000.0f;
    m_camera->yaw = (M_PI);

    m_edges = QHash<QPair<int, int>, QVector<OBJ::Triangle *> >();

    m_enemy = new Player();
    m_enemy->pos = Vector3(0,10,0);
    m_enemy->color = Vector4(.1,.1,.1,1);

    m_ball = new Player();
    m_ball->radius = Vector3(.5,.5,.5);
    m_ball->pos = Vector3(2,2,0);

    m_player = new Player();
    m_player->pos = Vector3(0,2,0);
    m_player->lives = 3;
    m_objectReader->read(m_levelPath.mid(0).append(QString::fromStdString(".obj")));
//    m_navReader->read(m_levelPath.mid(0).append(QString::fromStdString("_navmesh.obj")));

//    generateGraph();

    m_textures.append(loadTexture(m_levelPath.mid(0).append(QString::fromStdString(".png"))));
    m_textures.append(loadTexture(QString::fromStdString("level_easy_channels.png")));
}

/*
 * Loads a texture!
 */
GLuint PlatWorld::loadTexture(const QString &name) {
    QImage img(":res/textures/" + name);

    if (img.isNull()) {
        qCritical("Unable to load texture!");
        return -1;
    }

    img = img.convertToFormat(QImage::Format_RGBA8888);

    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(), img.height(),
                 0, GL_RGBA, GL_UNSIGNED_BYTE, img.bits());

    return id;
}

void PlatWorld::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) QApplication::quit();
    if (event->key() == Qt::Key_W) m_player->vel.z=m_player->vertVelConst;
    if (event->key() == Qt::Key_D) m_player->vel.x=-m_player->horVelConst ;
    if (event->key() == Qt::Key_A)  m_player->vel.x=m_player->horVelConst;
    if (event->key() == Qt::Key_S)  m_player->vel.z=-m_player->vertVelConst;
    if (event->key() == Qt::Key_Space && !m_player->m_jump) {m_player->vel.y = m_player->jumpVelConst;m_player->m_jump = true;}

}

void PlatWorld::keyReleaseEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_W) m_player->vel.z=0;
    if (event->key() == Qt::Key_D) m_player->vel.x=0 ;
    if (event->key() == Qt::Key_A)  m_player->vel.x=0;
    if (event->key() == Qt::Key_S)  m_player->vel.z=0;
}

void PlatWorld::mouseMoveEvent(QMouseEvent *event) {
    int x = event->x();
    int y = event->y();

    float diffX = m_viewSize.x/2.0f - x;
    float diffY = m_viewSize.y/2.0f - y;
    m_camera->pitch = (-diffY/(m_viewSize.y/2.0f))*((M_PI/2)-.0001f);

    m_camera->yaw += diffX/yawConst;
    m_camera->yaw = fmod(m_camera->yaw,2.0f*M_PI);
    qDebug() << m_camera->pitch;
    QCursor::setPos(m_viewSize.x / 2.0f, QCursor::pos().y());
}

void PlatWorld::mousePressEvent(QMouseEvent *event)  {
    Q_UNUSED(event);
}

void PlatWorld::mouseReleaseEvent(QMouseEvent *event)  {
    Q_UNUSED(event);
}

void PlatWorld::updateCamera()  {
    float ratio = 1.0f * (m_viewSize.x / m_viewSize.y);

    m_camera->center.x = m_player->pos.x;
    m_camera->center.y = m_player->pos.y;
    m_camera->center.z = m_player->pos.z;

    m_camera->eye.x = m_camera->center.x - 7.0f*cosf(m_camera->pitch)*sinf(m_camera->yaw);
    m_camera->eye.y = m_camera->center.y + 7.0f*sinf(m_camera->pitch);
    m_camera->eye.z = m_camera->center.z - 7.0f*cosf(m_camera->pitch)*cosf(m_camera->yaw);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(m_camera->angle, ratio, m_camera->near, m_camera->far);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(m_camera->eye.x, m_camera->eye.y, m_camera->eye.z,
              m_camera->center.x, m_camera->center.y, m_camera->center.z,
              m_camera->up.x, m_camera->up.y, m_camera->up.z);
}

void PlatWorld::update(float time)  {
    Vector3 posAdd = Vector3(0,0,0);

    m_player->vel.y += m_grav*time;
    posAdd.y+= time*m_player->vel.y + .5*(m_grav)*(time*time);
    collideEllipses(m_player,posAdd,1,false);
    posAdd = Vector3(0,0,0);
    posAdd.z+= -(m_player->vel.x*time)*sinf(m_camera->yaw) + (m_player->vel.z*time)*cosf(m_camera->yaw);
    posAdd.x+= (m_player->vel.x*time)*cosf(m_camera->yaw) + (m_player->vel.z*time)*sinf(m_camera->yaw);

    collideEllipses(m_player,posAdd,3,false);

    posAdd = Vector3(0,0,0);
    m_ball->vel.y += m_grav*time;
    posAdd.y+= time*m_ball->vel.y + .5*(m_grav)*(time*time);
    collideEllipses(m_ball,posAdd,1,true);
    posAdd = Vector3(0,0,0);
    posAdd.z+= m_ball->vel.z*time;
    posAdd.x+= m_ball->vel.x*time;
    collideEllipses(m_ball,posAdd,3,true);

    if(!m_ball->m_jump) {
        m_ball->vel.x -= ((m_ball->vel.x > 0) - (m_ball->vel.x < 0))*(sqrt(time*sqrt(time)*fabs(m_ball->vel.x)));
        m_ball->vel.z -= ((m_ball->vel.z > 0) - (m_ball->vel.z < 0))*(sqrt(time*sqrt(time)*fabs(m_ball->vel.z)));
    }

    doCollisions(time);

//    doBfs();

//    //if(m_drawPath) doFunnel();

//    doFunnel();

    updateCamera();
}

void PlatWorld::collideEllipses(Player* &p, Vector3 &posAdd, int iters, bool bounce)  {

    float rx = p->radius.x;
    float ry = p->radius.y;
    float rz = p->radius.z;

    Vector3 squash = Vector3(1.0f/rx,1.0f/ry,1.0f/rz);

    for(int k = 0; k < iters; k++)  {
        QVector<OBJ::Triangle>::iterator itr = m_objectReader->triangles.begin();
        Vector3 currentInterPt = p->pos + posAdd;
        Vector3 start = p->pos;
        start*=squash;
        float t = INFINITY;
        Vector3 contactPoint;
        while (itr != m_objectReader->triangles.end())  {
            OBJ::Triangle tri = *itr;
            collideHelp(tri,currentInterPt,contactPoint,p->pos + posAdd, start,t,squash);
            itr++;
        }
        if(t == INFINITY) {
            currentInterPt = p->pos + posAdd;
            p->pos = currentInterPt;
            break;
        }
        else  {
            Vector3 contactNorm = (((currentInterPt - contactPoint)*squash)*squash).unit();
            if(contactNorm.y > 0)  {
                p->m_jump = false;
                p->vel.y = -2;
            }
            Vector3 curPos = currentInterPt + (.001)*contactNorm;
            Vector3 diff = ((p->pos + posAdd) - curPos);
            Vector3 up = Vector3(0,1.0,0);
            Vector3 newSub;
            if(fabs(contactNorm.dot(up)) <= .001)  {
                newSub = diff.dot(contactNorm)*(contactNorm);
                posAdd = diff - newSub;
            }
            else  {
                Vector3 newDist = diff - diff.dot(contactNorm)*contactNorm;
                newSub = (diff.dot(contactNorm)/(contactNorm.dot(up)))*up;
                posAdd = (diff - newSub).unit() * newDist.length();
            }

            p->pos = curPos;
        }
    }
}

void PlatWorld::collideHelp(OBJ::Triangle &tri,Vector3 &curPoint, Vector3 &contactPoint, const Vector3 &endPoint, const Vector3 &startPoint, float &t, const Vector3 &squash)  {
    float testT;
    // sphere-plane
    Vector3 A,B,C;
    A = m_objectReader->vertices[tri.a.vertex]*squash;
    B = m_objectReader->vertices[tri.b.vertex]*squash;
    C = m_objectReader->vertices[tri.c.vertex]*squash;

    Vector3 norm = (B/squash-A/squash).cross(C/squash-A/squash).unit();
    Vector3 norm_w = (B - A).cross(C - A);
    Vector3 cur_w = endPoint*squash;

    norm_w = norm_w.unit();
    if(norm_w.dot(cur_w - startPoint) > 0)  {
        norm_w *= -1.0;
        norm *= -1.0;
    }
    Vector3 top = startPoint - norm_w - A;QVector<OBJ::Triangle>::iterator itr = m_objectReader->triangles.begin();
    Vector3 bottom = cur_w - startPoint;
    testT = -1.0*(norm_w.dot(top)/norm_w.dot(bottom));
    if(!(testT >= 0) || !(testT < 1.0f)) {testT = INFINITY;}
    if (testT < INFINITY && testT < t)  {
        Vector3 point = (startPoint - norm_w) + (cur_w - startPoint)*testT;
        Vector3 normA = (B-A).cross(point-B);
        Vector3 normB = (C-B).cross(point-C);
        Vector3 normC = (A-C).cross(point-A);
        if(normA.dot(normB) > 0 && normA.dot(normC) > 0) {
            t = testT;
            curPoint = (startPoint/squash) + (cur_w/squash - startPoint/squash)*t;
            contactPoint = (startPoint-norm_w) + (endPoint*squash - startPoint)*t;
            contactPoint/=squash;
            return;
        }
    }
        Vector3 D;
        QVector<Vector3> qv;
        qv.append(A);
        qv.append(B);
        qv.append(C);
        A = startPoint;
        B = cur_w;
        for(int i = 0; i < 3; i++)  {
            C = qv[(i+1)%3];
            D = qv[i];

            // sphere-edge
            float a,b,c;
            a = (B-A).cross(D-C).lengthSquared();
            b = 2.0f*(((B-A).cross(D-C)).dot((A-C).cross(D-C)));
            c = ((A-C).cross(D-C).lengthSquared()) - ((D-C).lengthSquared());
            Vector2 possT = quadratic(a,b,c);
            testT = fmin(possT.x, possT.y);
            Vector3 testPoint = A + (B-A)*testT;
            if(0 < (testPoint - C).dot(D-C) && (testPoint - C).dot(D-C) < (D-C).lengthSquared()) {
                if (testT < t && !isnan(testT) && testT >= 0 && testT <= 1.0f)  {
                    t = testT;
                    curPoint = (startPoint/squash) + (cur_w/squash - startPoint/squash)*t;
                    contactPoint = ((testPoint - C).dot(D-C)/((D-C).length()*(D-C).length()))*(D - C) + C;
                    contactPoint/=squash;
                }
            }

            // sphere-point

            a = (B-A).lengthSquared();
            b = -2.0f*(B-A).dot(D-A);
            c = (D-A).lengthSquared() - 1.0f;
            possT = quadratic(a,b,c);
            testT = fmin(possT.x, possT.y);
            if(!isnan(testT) && testT < t && testT >= 0 && testT <= 1.0f) {
                t = testT;
                curPoint = (startPoint/squash) + (cur_w/squash - startPoint/squash)*t;
                contactPoint = D;
                contactPoint/=squash;
            }
        }

}

Vector2 PlatWorld::quadratic(float a, float b, float c)  {
    float root = sqrt(b*b - 4.0f*a*c);
    float x1 = (-1.0f*b + root)/(2.0f*a);
    float x2 = (-1.0f*b - root)/(2.0f*a);
    return Vector2(x1,x2);
}

void PlatWorld::draw() {
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);

    glBindTexture(GL_TEXTURE_2D,m_textures.at(0));
    glColor3f(1, 1, 1);
    m_objectReader->draw();
    glBindTexture(GL_TEXTURE_2D,m_textures.at(1));

    //draw player
    glPushMatrix();
    glTranslatef(m_player->pos.x, m_player->pos.y, m_player->pos.z);
    glScalef(m_player->radius.x,m_player->radius.y,m_player->radius.z);
    GLUquadric *quad = gluNewQuadric();
    gluQuadricNormals(quad,GLU_SMOOTH);
    gluQuadricTexture(quad,GL_TRUE);
    gluSphere(quad, 1, 32, 16);
    glPopMatrix();

    //draw snowball
    glDisable(GL_TEXTURE_2D);
    glPushMatrix();
    glColor3f(.1,.1,.8);
    glTranslatef(m_ball->pos.x, m_ball->pos.y, m_ball->pos.z);
    glScalef(m_ball->radius.x,m_ball->radius.y,m_ball->radius.z);
    quad = gluNewQuadric();
    gluQuadricNormals(quad,GLU_SMOOTH);
    gluQuadricTexture(quad,GL_TRUE);
    gluSphere(quad, 1, 32, 16);
    glColor3f(1,1,1);
    glPopMatrix();

}

void PlatWorld::generateGraph()  {
    QVector<OBJ::Triangle>::iterator tri = m_navReader->triangles.begin();
    while (tri != m_navReader->triangles.end()) {
        int A,B,C;
        A = tri->a.vertex;
        B = tri->b.vertex;
        C = tri->c.vertex;
        QVector<int> qv;
        qv.append(A);
        qv.append(B);
        qv.append(C);
        for(int i = 0; i < 3; i++) {
            int vo = qv[i];
            int v1 = qv[(i+1)%3];
            QPair<int,int> key = QPair<int,int>(std::min(vo,v1),std::max(vo,v1));
            m_edges[key].append(tri);
        }
        ++tri;
    }
}

void PlatWorld::doBfs()  {
    if (m_drawPath)  {
        QPair<OBJ::Triangle *,OBJ::Triangle *> tris = QPair<OBJ::Triangle *,OBJ::Triangle *>();
        tris.first = findTri(m_enemy->pos);
        tris.second = findTri(m_player->pos);
        bfs(tris.first,tris.second);
    }
}

void PlatWorld::bfs(OBJ::Triangle * &ti,OBJ::Triangle * &tdest)  {
    m_bfs.clear();
    QVector<OBJ::Triangle>::iterator itr = m_navReader->triangles.begin();
    while (itr != m_navReader->triangles.end()) {
        itr->visited = false;
        if(itr->v) {
            delete itr->v;
        }
        itr->v = 0;
        ++itr;
    }


    QVector<OBJ::Triangle *> queue;
    queue.push_back(ti);
    ti->visited = true;
    ti->v = new Vertex();
    ti->v->tri = ti;
    int count = 0;
    while(!queue.isEmpty()) {
        count++;
        OBJ::Triangle *tri = queue.front();
        if(tri == tdest) {
            break;
        }
        queue.pop_front();
        int A,B,C;
        A = tri->a.vertex;
        B = tri->b.vertex;
        C = tri->c.vertex;
        QVector<int> qv;
        qv.append(A);
        qv.append(B);
        qv.append(C);
        for(int i = 0; i < 3; i++) {
            int vo = qv[i];
            int v1 = qv[(i+1)%3];
            QPair<int,int> key = QPair<int,int>(std::min(vo,v1),std::max(vo,v1));
            QVector<OBJ::Triangle *> adjacentTris = m_edges[key];
            foreach(OBJ::Triangle * adjTri,adjacentTris) {
                if(!adjTri->visited) {
                    adjTri->v = new Vertex();
                    adjTri->v->tri = adjTri;
                    adjTri->v->prev = tri->v;
                    adjTri->visited = true;
                    adjTri->v->v0 = m_navReader->vertices[vo];
                    adjTri->v->v1 = m_navReader->vertices[v1];
                    queue.push_back(adjTri);
                }
            }
        }

    }
    if(tdest->visited) {
        Vertex *v = tdest->v;
        m_bfs.append(v);
        while(v->prev != 0) {
            m_bfs.push_front(v->prev);
            v = v->prev;
        }
    }
}

void PlatWorld::doFunnel()  {
    //beginning stuff
    Vector3 src = m_enemy->pos;
    Vector3 target = m_player->pos;
    m_path.clear();
    //m_path.append(src);
    //funnel stuff
    Vertex* v = new Vertex();
    v->v0 = target;
    v->v1 = target;
    v->prev = m_bfs.last();
    m_bfs.push_back(v);
    QVector<Vertex *>::iterator left = m_bfs.begin();
    QVector<Vertex *>::iterator right = m_bfs.begin();
    left++;
    right++;
    Vertex ** leftCheck = left;
    Vertex ** rightCheck = right;
    if(left == m_bfs.end() || right == m_bfs.end()) {
        m_path.push_back(target);
        m_bfs.pop_back();
        delete v;
        return;
    }
    Vector3 leftVector = (*left)->v0 - src + Vector3(0,.5,0);
    Vector3 rightVector = (*right)->v1 - src + Vector3(0,.5,0);
    Vector3 leftCross = leftVector;
    Vector3 rightCross = rightVector;
    Vector3 cross = leftCross.cross(rightCross);
    int sign = (0 < cross.y) - (cross.y < 0);
    bool toMove = true;
    bool toLeft = true;
    while (toMove)  {
        if((*leftCheck) == v && (*rightCheck) == v) {
            toMove = false;
            continue;
        }
        if((left == m_bfs.end() || right == m_bfs.end())) {
            toMove = false;
            continue;
        }
        leftVector = (*left)->v0 - src + Vector3(0,.5,0);
        rightVector = (*right)->v1 - src + Vector3(0,.5,0);
        float angle = acos((leftVector.unit()).dot(rightVector.unit()));
        if(toLeft) {
            if((*leftCheck) != v) {
                ++leftCheck;
            }
            if((*leftCheck) == v && (*rightCheck) == v) {
                toMove = false;
                continue;
            }
            leftVector = .99*(*leftCheck)->v0 - .01*(*right)->v1 - src + Vector3(0,.5,0);
            Vector3 leftCross = leftVector;
            Vector3 rightCross = rightVector;
            Vector3 tempCross = leftCross.cross(rightCross);
            int tempSign = (0 < tempCross.y) - (tempCross.y < 0);
            float toTest = tempCross.y;
            float round = toRound(fabs(toTest),1e-4);
            if(round == 0) {
                tempSign = sign;
            }
            if(tempSign != sign) {
                src = (*right)->v1 + Vector3(0,.5,0);
                m_path.push_back(.7*(*right)->v1 + .3*(*right)->v0 + Vector3(0,.5,0));
                left = right;
                ++right;
                ++left;
                ++left;
                ++right;
                leftCheck = left;
                rightCheck = right;
            }
            else  {
                float tempAngle = acos((leftVector.unit()).dot(rightVector.unit()));
                if(fabs(tempAngle) < fabs(angle)) {
                    left = leftCheck;
                }
            }
        }
        else  {
            if((*rightCheck) != v) {
                ++rightCheck;
            }
            if((*leftCheck) == v && (*rightCheck) == v) {
                toMove = false;
                continue;
            }
            rightVector = .99*(*rightCheck)->v1 - .01*(*left)->v0 - src + Vector3(0,.5,0);
            Vector3 leftCross = leftVector;
            Vector3 rightCross = rightVector;
            Vector3 tempCross = leftCross.cross(rightCross);
            int tempSign = (0 < tempCross.y) - (tempCross.y < 0);
            float toTest = tempCross.y;
            float round = toRound(fabs(toTest),1e-4);
            if(round == 0) {
                tempSign = sign;
            }
            if(tempSign != sign) {
                src = (*left)->v0 + Vector3(0,.5,0);
                m_path.push_back(.7*(*left)->v0 + .3*(*left)->v1 + Vector3(0,.5,0));
                right = left;
                ++right;
                ++left;
                ++left;
                ++right;
                rightCheck = right;
                leftCheck = left;
            }
            else  {
                float tempAngle = acos((leftVector.unit()).dot(rightVector.unit()));
                if(fabs(tempAngle) < fabs(angle)) {
                    right = rightCheck;
                }
            }
        }
        toLeft = !toLeft;
    }

    //end stuff
    m_path.push_back(target);
    m_bfs.pop_back();
    delete v;
}

float PlatWorld::toRound(float toRound, float epsilon) {
    if (toRound < epsilon) {
        toRound = 0;
    }
    return toRound;
}

OBJ::Triangle* PlatWorld::findTri(Vector3 pt) {
    Vector3 point = pt;
    QVector<OBJ::Triangle *> posTris;
    OBJ::Triangle* toReturn;
    QVector<OBJ::Triangle>::iterator itr = m_navReader->triangles.begin();
    while (itr != m_navReader->triangles.end()) {
        OBJ::Triangle *tri = itr;
        Vector3 A,B,C;
        A = m_navReader->vertices[tri->a.vertex];
        B = m_navReader->vertices[tri->b.vertex];
        C = m_navReader->vertices[tri->c.vertex];
        A.y = 0;
        B.y = 0;
        C.y = 0;
        point.y = 0;
        Vector3 normA = (B-A).cross(point-B);
        Vector3 normB = (C-B).cross(point-C);
        Vector3 normC = (A-C).cross(point-A);
        if(normA.dot(normB) > 0 && normA.dot(normC) > 0) {
            posTris.append(tri);
        }
        ++itr;
    }
    itr = m_navReader->triangles.begin();
    if(posTris.size() == 0) {
        while(itr != m_navReader->triangles.end()) {
            OBJ::Triangle *tri = itr;
            posTris.append(tri);
            ++itr;
        }
    }
    float dist = INFINITY;
    foreach(OBJ::Triangle* tri,posTris) {
        Vector3 A,B,C,center;
        A = m_navReader->vertices[tri->a.vertex];
        B = m_navReader->vertices[tri->b.vertex];
        C = m_navReader->vertices[tri->c.vertex];
        center = (A + B + C)/3.0f;
        float tempDist = (center-pt).lengthSquared();
        if(tempDist < dist) {
            toReturn = tri;
            dist = tempDist;
        }
    }
    return toReturn;
}

void PlatWorld::doCollisions(float seconds) {
    Vector3 vel;
    vel.z+= -(m_player->vel.x)*sinf(m_camera->yaw) + (m_player->vel.z)*cosf(m_camera->yaw);
    vel.x+= (m_player->vel.x)*cosf(m_camera->yaw) + (m_player->vel.z)*sinf(m_camera->yaw);
    vel.y = 5;
    if(((m_player->pos - m_ball->pos).length()) < (2*m_ball->radius.x)) {
        m_ball->vel = vel;
        m_ball->m_jump = true;
        m_counter = 0;
    }
    else   {
        m_counter+=seconds;
        if(m_counter > 10) {
            m_ball->vel = Vector3(0,0,0);
            m_ball->pos = Vector3(2,2,0);
            m_counter = 0;
        }
    }
}
