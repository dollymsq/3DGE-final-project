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

#include <iostream>
#include "puzzles.h"

// connection for physx debugger
#define PVD_HOST "127.0.0.1"
#define MAX_NUM_ACTOR_SHAPES 125
#include <PxPhysicsAPI.h>
using namespace physx;
#include <QDebug>

class World : public QObject, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    World();
    virtual ~World();

    void init();
    void draw();
    void tick(float seconds);


    PxRigidDynamic *createDynamic(const PxTransform &t, const PxGeometry &geometry, const PxVec3 &velocity = PxVec3(0));

    int numberOfDynamic;
    QString dynamicstring;
    Puzzles *m_puzzles;

    bool m_puzzleSolved;

private:
    void createStack(const PxTransform& t, PxU32 size, PxReal halfExtent);
    void initPhysics(bool interactive);
    void stepPhysics(bool interactive);
    void cleanupPhysics(bool interactive);

    void renderActors(PxRigidActor** actors, const PxU32 numActors, bool shadows);
    void renderGeometry(const PxGeometryHolder& h);

    Obj sphereMesh;
    Obj cubeMesh;

    PxRigidDynamic         *m_redBlock;
    PxVec3                  m_redBlockPos;
    PxVec3                  m_redBlockOriPos;
    bool m_redBlockPosInit;


    PxDefaultAllocator		gAllocator;
    PxDefaultErrorCallback	gErrorCallback;

    PxFoundation*			gFoundation;
    PxPhysics*				gPhysics;

    PxDefaultCpuDispatcher*	gDispatcher;
    PxScene*				gScene;

    PxMaterial*				gMaterial;

    PxVisualDebuggerConnection* gConnection;


    PxReal stackZ;

};

#endif // WORLD_H
