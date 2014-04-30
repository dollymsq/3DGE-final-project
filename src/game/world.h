#ifndef WORLD_H
#define WORLD_H

//#include <glm/vec2.hpp>
//#include <glm/vec3.hpp>
//#include <glm/ext.hpp>
//#include <glm/gtc/noise.hpp>

#include <QHash>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLFunctions>
#include <QList>

#include "assets/obj.h"
#include "assets/tree.h"
#include "scene/camera.h"
#include "assets/tree.h"
#include "math/lparser.h"
#include "math/calc.h"

#include <iostream>
#include "puzzles.h"
#include <QtGui/QPainter>
#include <QElapsedTimer>

#include <QGLShaderProgram>
#include <math/vector.h>

// connection for physx debugger
#define PVD_HOST "127.0.0.1"
#define MAX_NUM_ACTOR_SHAPES 125
#include <PxPhysicsAPI.h>
#include <characterkinematic/PxController.h>
#include <characterkinematic/PxControllerBehavior.h>
#include <PxSimulationEventCallback.h>
using namespace physx;

struct FilterGroup
{
    enum Enum
    {
        eBALL   		= (1 << 0),
        eRED_BOX		= (1 << 1),
        eORD_BOX		= (1 << 2),
        eHOLE           = (1 << 3),
        eSTEPPING_BOX   = (1 << 4),
        eGROUND         = (1 << 5),
        //
    };
};

struct Color
{
    enum Enum
    {
        cGrey       = 0,
        cRED   		= 1,
        cORANGE		= 2,
        cYELLOW		= 3,
        cGREEN      = 4,
        cBLUE       = 5,
        cINDIGO     = 6,
        cVIOLET     = 7
        //
    };

};

class World : public QObject, protected QOpenGLFunctions,
        public PxSimulationEventCallback,
        public PxContactModifyCallback,
        public PxUserControllerHitReport,
        public PxControllerBehaviorCallback
{
    Q_OBJECT

public:
    World();
    virtual ~World();

    void init(float aspectRatio);
    void draw(QPainter *m_painter);
    void tick(float seconds);

    PxRigidDynamic *createDynamic(const PxTransform &t, const PxGeometry &geometry, const PxVec3 &velocity = PxVec3(0), int color = 0, bool isShadow = true,bool isTrajectory = true);

    Puzzles *m_puzzles;
    int m_puzzleSolved;

    bool m_isBoulder = false;

    bool m_gameOver = false;

    void checkGameOver();

    void shootDynamic();
    void enableForward(bool flag);
    void enableBackward(bool flag);
    void enableLeft(bool flag);
    void enableRight(bool flag);

    glm::mat4 getPMatrix();
    glm::mat4 getVMatrix();
    void rotateMouse(glm::vec2 delta);

    virtual void onShapeHit(const PxControllerShapeHit& hit);
    virtual void onControllerHit(const PxControllersHit& hit);
    virtual void onObstacleHit(const PxControllerObstacleHit& hit);

    virtual PxControllerBehaviorFlags getBehaviorFlags(const PxShape& shape, const PxActor& actor);
    virtual PxControllerBehaviorFlags getBehaviorFlags(const PxController& controller);
    virtual PxControllerBehaviorFlags getBehaviorFlags(const PxObstacle& obstacle);


    virtual void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs);

    virtual void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) {}
    virtual void onWake(PxActor** actors, PxU32 count) {}
    virtual void onSleep(PxActor** actors, PxU32 count) {}
    virtual void onTrigger(PxTriggerPair* pairs, PxU32 count) {}

    virtual void onContactModify(PxContactModifyPair *const pairs, PxU32 count);

private:
    void createStack(const PxTransform& t, PxU32 size, PxReal halfExtent);
    PxRigidStatic *createBox(const PxTransform& t, PxReal x, PxReal y, PxReal z, PxMaterial *m, int color = 0, bool isTransparent = false,bool isShadows = true);
    PxRigidDynamic *createDynamicBox(const PxTransform& t, PxReal x, PxReal y, PxReal z, bool isTransparent = false, bool isShadows = true,int colornum = 0);
    PxRigidStatic *createTriMesh(Renderable *r,const PxTransform &t,PxMaterial *m_material, int color = 0,bool isTransparent = false, bool isShadows = true);
    void createTreeActors(Tree &t);
    void initPhysics(bool interactive);
    void stepPhysics(bool interactive);
    void cleanupPhysics(bool interactive);

    void renderActors(PxRigidActor** actors, const PxU32 numActors, bool shadows);
    void renderGeometry(const PxGeometryHolder& h, Renderable *r);
    void showSubtitles(QString &info, QPainter* m_painter);
    void showPermanentStat(QString &info, QPainter* m_painter);
    void showLevelStat(QString &info, QPainter* m_painter);

    int m_dyanmicsCount;
    QString m_dynamicsMessage;
    QString m_levelinfo;

    void setUpRoomOne();
    void setUpRoomTwo();
    void setUpRoomThree();
    void setUpRoomFour();
    void setUpRoomFive();
    void setUpRoomSix();

    void resetGame();

    float m_aspect;

    GLuint loadTexture(const QString &path);

    GLuint m_treeTexId;

    Obj sphereMesh;
    Obj cubeMesh;
    Obj rimMesh;
    Obj backboardMesh;
    Obj discMesh;
    Obj appleMesh;
    Obj caveMesh;
    Obj level6Mesh;
//    Tree triangleMesh;

    PxU32 contactFlag;//to mark for the last contact actor

    QHash<const PxActor*,Renderable*> m_renderables;
    QSet<const PxActor*> m_shadows;
    QSet<const PxActor*> m_balls;
    QVector<PxRigidActor*> m_renderableList;
    QVector<Tree*> m_trees;
    QHash<const PxActor*,int> m_color;

    PxCooking *m_cooking;

    PxRigidDynamic          *m_redBlock;
    PxRigidDynamic          *m_boulder;
    PxRigidDynamic          *m_domino;
    PxRigidStatic           *m_hole;
    PxRigidStatic           *m_rim;
    PxRigidStatic           *groundPlane;
    PxRigidStatic           *m_steppingbox;
    PxRigidStatic           *m_door;
    PxActor                 *currentBall;
PxActor* m_transptWall;

    Vector4                 pallete[10];
    PxVec3                  m_redBlockPos;
    PxVec3                  m_redBlockOriPos;

    PxDefaultAllocator		m_allocator;
    PxDefaultErrorCallback	m_errorCallback;

    PxFoundation*			m_foundation;
    PxPhysics*				m_physics;

    PxDefaultCpuDispatcher*	m_dispatcher;
    PxScene*				m_scene;

    // player camera controller
    PxControllerManager*    m_controllerManager;
    PxController*           m_playerController;

    PxMaterial*				m_material;

    PxVisualDebuggerConnection* m_connection;

    PxReal m_stackZ;

    Camera m_camera;
    QElapsedTimer m_subTimer;
    QGLShaderProgram m_program;

    void initShaders();
};

#endif // WORLD_H
