#ifndef PLATWORLD_H
#define PLATWORLD_H

#include "world.h"
#include "camera.h"
#include "player.h"
#include "obj.h"
#include "vertex.h"
#include <QHash>
#include <QPair>

class View;

class PlatWorld : public World
{
public:
    PlatWorld(View *v);
    virtual ~PlatWorld();
    void updateCamera();
    void updateStuff(float seconds);
    void draw();
    void initialize();
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

private:

    void collideEllipses(Player* &p,Vector3 &posAdd, int iters, bool bounce);
    void collideHelp(OBJ::Triangle &tri,Vector3 &curPoint, Vector3 &contactPoint,const Vector3 &endPoint, const Vector3 &startPoint, float &t, const Vector3 &squash);
    void generateGraph();
    void bfs(OBJ::Triangle * &ti, OBJ::Triangle * &tdest);
    void doBfs();
    void doFunnel();
    void doCollisions(float time);
    float toRound(float toRound, float epsilon);
    float m_counter;
    int lose;
    OBJ::Triangle* findTri(Vector3 pt);
    Vector2 quadratic(float a, float b, float c);
    Camera* m_camera;
    Player* m_player,* m_ball, * m_enemy;
    QString m_levelPath;
    OBJ* m_objectReader,* m_navReader;
    QVector<GLuint> m_textures;
    QVector<Vector3> m_path;
    QVector<Vertex *> m_bfs;
    QHash<QPair<int, int>, QVector<OBJ::Triangle *> > m_edges;
    const float m_grav = -9.8,yawConst = 400;
    bool m_drawPath=true,m_gameOver=false,m_showHint = true;
};

#endif // PLATWORLD_H
