#include "world.h"

World::World() :
    sphereMesh("sphere.obj"),
    cubeMesh("cube.obj"),
    gFoundation(NULL),
    gPhysics(NULL),
    gDispatcher(NULL),
    gScene(NULL),
    gMaterial(NULL),
    gConnection(NULL),
    stackZ(10.0f)
{

}

World::~World()
{
    //find a better place
    cleanupPhysics(true);
}

void World::init()
{
    initializeOpenGLFunctions();
    initPhysics(true);

    // Setup default render states
    glClearColor(0.3f, 0.4f, 0.5f, 1.0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);

    // Setup lighting
    glEnable(GL_LIGHTING);
    PxReal ambientColor[]	= { 0.0f, 0.1f, 0.2f, 0.0f };
    PxReal diffuseColor[]	= { 1.0f, 1.0f, 1.0f, 0.0f };
    PxReal specularColor[]	= { 0.0f, 0.0f, 0.0f, 0.0f };
    PxReal position[]		= { 100.0f, 100.0f, 400.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientColor);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseColor);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularColor);
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    glEnable(GL_LIGHT0);

}

void World::draw()
{
    glColor4f(0.4f, 0.4f, 0.4f, 1.0f);


    PxScene* scene;
    PxGetPhysics().getScenes(&scene,1);
    PxU32 nbActors = scene->getNbActors(PxActorTypeSelectionFlag::eRIGID_DYNAMIC | PxActorTypeSelectionFlag::eRIGID_STATIC);
    if(nbActors)
    {
        std::vector<PxRigidActor*> actors(nbActors);
        scene->getActors(PxActorTypeSelectionFlag::eRIGID_DYNAMIC | PxActorTypeSelectionFlag::eRIGID_STATIC, (PxActor**)&actors[0], nbActors);
        renderActors(&actors[0], (PxU32)actors.size(), true);
    }

    // Draw grid
//    glColor4f(0, 0, 0, 0.25);
//    glBegin(GL_LINES);
//    for (int s = 40, i = -s; i <= s; i++) {
//        glVertex3f(i /2.0f, 0, -s /2.0f);
//        glVertex3f(i /2.0f, 0, +s /2.0f);
//        glVertex3f(-s /2.0f, 0, i /2.0f);
//        glVertex3f(+s /2.0f, 0, i /2.0f);
//    }
//    glEnd();
}

PxRigidDynamic* World::createDynamic(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity)
{
    PxRigidDynamic* dynamic = PxCreateDynamic(*gPhysics, t, geometry, *gMaterial, 10.0f);
    dynamic->setAngularDamping(0.5f);
    dynamic->setLinearVelocity(velocity);
    gScene->addActor(*dynamic);
    return dynamic;
}


void World::createStack(const PxTransform& t, PxU32 size, PxReal halfExtent)
{
    PxShape* shape = gPhysics->createShape(PxBoxGeometry(halfExtent, halfExtent, halfExtent), *gMaterial);
    for(PxU32 i=0; i<size;i++)
    {
        for(PxU32 j=0;j<size-i;j++)
        {
            PxTransform localTm(PxVec3(PxReal(j*2) - PxReal(size-i), PxReal(i*2+1), 0) * halfExtent);
            PxRigidDynamic* body = gPhysics->createRigidDynamic(t.transform(localTm));
            body->attachShape(*shape);
            PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
            gScene->addActor(*body);
        }
    }
    shape->release();
}

void World::initPhysics(bool interactive)
{
    gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);
    PxProfileZoneManager* profileZoneManager = &PxProfileZoneManager::createProfileZoneManager(gFoundation);
    gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(),true,profileZoneManager);
    if(gPhysics->getPvdConnectionManager())
    {
        gPhysics->getVisualDebugger()->setVisualizeConstraints(true);
        gPhysics->getVisualDebugger()->setVisualDebuggerFlag(PxVisualDebuggerFlag::eTRANSMIT_CONTACTS, true);
        gPhysics->getVisualDebugger()->setVisualDebuggerFlag(PxVisualDebuggerFlag::eTRANSMIT_SCENEQUERIES, true);
        gConnection = PxVisualDebuggerExt::createConnection(gPhysics->getPvdConnectionManager(), PVD_HOST, 5425, 10);
    }

    PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
    sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
    gDispatcher = PxDefaultCpuDispatcherCreate(2);
    sceneDesc.cpuDispatcher	= gDispatcher;
    sceneDesc.filterShader	= PxDefaultSimulationFilterShader;
    gScene = gPhysics->createScene(sceneDesc);

    gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

    PxRigidStatic* groundPlane = PxCreatePlane(*gPhysics, PxPlane(0,1,0,0), *gMaterial);
    gScene->addActor(*groundPlane);

    for(PxU32 i=0;i<5;i++)
        createStack(PxTransform(PxVec3(0,0,stackZ-=10.0f)), 10, 2.0f);

    if(!interactive)
        createDynamic(PxTransform(PxVec3(0,40,100)), PxSphereGeometry(10), PxVec3(0,-50,-100));
}

void World::stepPhysics(bool interactive)
{
    PX_UNUSED(interactive)
    gScene->simulate(1.0f/30.0f);
    gScene->fetchResults(true);
}

void World::cleanupPhysics(bool interactive)
{
    PX_UNUSED(interactive)
    gScene->release();
    gDispatcher->release();
    PxProfileZoneManager* profileZoneManager = gPhysics->getProfileZoneManager();
    if(gConnection != NULL)
        gConnection->release();
    gPhysics->release();
    profileZoneManager->release();
    gFoundation->release();
}

void World::renderActors(PxRigidActor** actors, const PxU32 numActors, bool shadows)
{
    glEnable(GL_DEPTH_TEST);

    PxShape* shapes[MAX_NUM_ACTOR_SHAPES];
    for(PxU32 i=0;i<numActors;i++)
    {
        const PxU32 nbShapes = actors[i]->getNbShapes();
        PX_ASSERT(nbShapes <= MAX_NUM_ACTOR_SHAPES);
        actors[i]->getShapes(shapes, nbShapes);
        bool sleeping = actors[i]->isRigidDynamic() ? actors[i]->isRigidDynamic()->isSleeping() : false;

        for(PxU32 j=0;j<nbShapes;j++)
        {
            const PxMat44 shapePose(PxShapeExt::getGlobalPose(*shapes[j], *actors[i]));
            PxGeometryHolder h = shapes[j]->getGeometry();

            // render object
            glPushMatrix();
            glMultMatrixf((float*)&shapePose);
            if(sleeping)
                glColor4f(0.75f, 1.0f, 0.75f, 1.0f);
            else
                glColor4f(0.0f, 0.75f, 0.0f, 1.0f);
            renderGeometry(h);
            glPopMatrix();

            // notice this are really fake sahdows
            if(shadows)
            {
                const PxVec3 shadowDir(0.0f, -0.7071067f, -0.7071067f);
                const PxReal shadowMat[]={ 1,0,0,0, -shadowDir.x/shadowDir.y,0,-shadowDir.z/shadowDir.y,0, 0,0,1,0, 0,0,0,1 };
                glPushMatrix();
                glMultMatrixf(shadowMat);
                glMultMatrixf((float*)&shapePose);
                glDisable(GL_LIGHTING);
                glColor4f(0.1f, 0.2f, 0.3f, 1.0f);
                renderGeometry(h);
                glEnable(GL_LIGHTING);
                glPopMatrix();
            }
        }
    }

    glDisable(GL_DEPTH_TEST);
}

void World::renderGeometry(const PxGeometryHolder& h)
{
    switch(h.getType())
    {
    case PxGeometryType::eBOX:
        {
            glScalef(h.box().halfExtents.x, h.box().halfExtents.y, h.box().halfExtents.z);
            glPushMatrix();
                glScalef(2,2,2);
                cubeMesh.draw();
            glPopMatrix();
        }
        break;
    case PxGeometryType::eSPHERE:
        {
            float diam = h.sphere().radius * 2;
            glPushMatrix();
                glScalef(diam, diam, diam);
                sphereMesh.draw();
            glPopMatrix();
        }
        break;
    }
}

void World::tick(float seconds)
{
    stepPhysics(true);
}
