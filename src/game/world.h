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

#include <iostream>
#include "puzzles.h"
#include <QtGui/QPainter>
#include <QElapsedTimer>

#include <QGLShaderProgram>


// connection for physx debugger
#define PVD_HOST "127.0.0.1"
#define MAX_NUM_ACTOR_SHAPES 125
#include <PxPhysicsAPI.h>
#include "PxSimulationEventCallback.h"
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
        //
    };
};

class World : public QObject, protected QOpenGLFunctions, public PxSimulationEventCallback, public PxContactModifyCallback
{
    Q_OBJECT

public:
    World();
    virtual ~World();

    void init(float aspectRatio);
    void draw(QPainter *m_painter);
    void tick(float seconds);


    PxRigidDynamic *createDynamic(const PxTransform &t, const PxGeometry &geometry, const PxVec3 &velocity = PxVec3(0));

    int m_dyanmicsCount;
    QString m_dynamicsMessage;
    Puzzles *m_puzzles;
    bool m_puzzleSolved;

    void shootDynamic();
    void enableForward(bool flag);
    void enableBackward(bool flag);
    void enableLeft(bool flag);
    void enableRight(bool flag);

    glm::mat4 getPMatrix();
    glm::mat4 getVMatrix();
    void rotateMouse(glm::vec2 delta);

    virtual void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs);
    virtual void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) {}
    virtual void onWake(PxActor** actors, PxU32 count) {}
    virtual void onSleep(PxActor** actors, PxU32 count) {}
    virtual void onTrigger(PxTriggerPair* pairs, PxU32 count) {}

    virtual void onContactModify(PxContactModifyPair *const pairs, PxU32 count);



private:
    void createStack(const PxTransform& t, PxU32 size, PxReal halfExtent);
    PxRigidStatic *createBox(const PxTransform& t, PxReal x, PxReal y, PxReal z);
    void createTreeActors(Tree &t);
    void initPhysics(bool interactive);
    void stepPhysics(bool interactive);
    void cleanupPhysics(bool interactive);

    void renderActors(PxRigidActor** actors, const PxU32 numActors, bool shadows);
    void renderGeometry(const PxGeometryHolder& h);
    void showSubtitles(QString &info, QPainter* m_painter);
    void showPermanentStat(QString &info, QPainter* m_painter);

    GLuint loadTexture(const QString &path);

    GLuint m_treeTexId;

    Obj sphereMesh;
    Obj cubeMesh;

    PxRigidDynamic          *m_redBlock;
    PxRigidStatic           *m_hole;

    PxVec3                  m_redBlockPos;
    PxVec3                  m_redBlockOriPos;

    PxDefaultAllocator		m_allocator;
    PxDefaultErrorCallback	m_errorCallback;

    PxFoundation*			m_foundation;
    PxPhysics*				m_physics;

    PxDefaultCpuDispatcher*	m_dispatcher;
    PxScene*				m_scene;

    PxControllerManager*    m_manager;

    PxMaterial*				m_material;

    PxVisualDebuggerConnection* m_connection;


    PxReal m_stackZ;

    Camera m_camera;
    QElapsedTimer m_subTimer;
    QGLShaderProgram m_program;

    void initShaders();
};

#endif // WORLD_H
