#include "world.h"
#include "assets/tree.h"
#include "math/lparser.h"

Tree m_tree;

PxFilterFlags WorldFilterShader(
    PxFilterObjectAttributes attributes0, PxFilterData filterData0,
    PxFilterObjectAttributes attributes1, PxFilterData filterData1,
    PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
    // let triggers through
    if(PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
    {
        pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
        return PxFilterFlag::eDEFAULT;
    }
    // generate contacts for all that were not filtered above
    pairFlags = PxPairFlag::eCONTACT_DEFAULT;

    // trigger the contact callback for pairs (A,B) where
    // the filtermask of A contains the ID of B and vice versa.

//    if((filterData0.word0 == FilterGroup::eBALL && filterData1.word0 == FilterGroup::eRED_BOX )
//            || (filterData0.word0 == FilterGroup::eRED_BOX && filterData1.word0 == FilterGroup::eBALL ))
//    {
//        qDebug()<<"box box";
//    }
    if((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
    {
        pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;

        if(filterData0.word0 & FilterGroup::eHOLE || filterData0.word1 & FilterGroup::eHOLE)
            pairFlags |= PxPairFlag::eMODIFY_CONTACTS;
    }

    return PxFilterFlag::eDEFAULT;
}

void setupFiltering(PxRigidActor* actor, PxU32 filterGroup, PxU32 filterMask)
{
        PxFilterData filterData;
        filterData.word0 = filterGroup; // word0 = own ID
        filterData.word1 = filterMask;  // word1 = ID mask to filter pairs that trigger a contact callback;
        const PxU32 numShapes = actor->getNbShapes();
        PxShape** shapes = (PxShape**) malloc(sizeof(PxShape*)*numShapes);
        actor->getShapes(shapes, numShapes);
        for(PxU32 i = 0; i < numShapes; i++)
        {
                PxShape* shape = shapes[i];
                shape->setSimulationFilterData(filterData);
        }
        delete[] shapes;
}

World::World() :
    sphereMesh("sphere.obj"),
    cubeMesh("cube.obj"),
    m_foundation(NULL),
    m_physics(NULL),
    m_dispatcher(NULL),
    m_scene(NULL),
    m_material(NULL),
    m_connection(NULL),
    m_redBlock(NULL),
    m_stackZ(10.0f),
    m_puzzleSolved(false)
{
    m_puzzles = new Puzzles();
}

World::~World()
{
    //TODO: find a better place
    cleanupPhysics(true);
    delete m_puzzles;
}

void World::init(float aspectRatio)
{
    // camera
    m_camera.setAspectRatio(aspectRatio);

    initializeOpenGLFunctions();
    initPhysics(true);

    // Setup default render states
    glClearColor(0.1f, 0.1f, 0.1f, 1);
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

    m_subTimer.start();

    m_dyanmicsCount = 40;
    m_dynamicsMessage = "Number of Balls Left: " + QString::number(m_dyanmicsCount);

    initShaders();

    m_tree = Tree();
    m_tree.generate(LParser::testTree());
//    m_tree.generate("F(2)[\(45)&(45)F(2)]/(45)&(45)F(2)e");
}

void World::initShaders()
{
    // Override system locale until shaders are compiled
    setlocale(LC_NUMERIC, "C");

    if (!m_program.addShaderFromSourceFile(QGLShader::Vertex, ":/shaders/simple.vert"))
        exit(EXIT_FAILURE);

    if (!m_program.addShaderFromSourceFile(QGLShader::Fragment, ":/shaders/simple.frag"))
        exit(EXIT_FAILURE);

    if (!m_program.link())
        exit(EXIT_FAILURE);

    if (!m_program.bind())
        exit(EXIT_FAILURE);

    setlocale(LC_ALL, "");
}

void World::draw(QPainter *m_painter)
{
    glEnable(GL_DEPTH_TEST);


    glColor4f(1.0f, 0.1f, 0.1f, 1.0f);

    PxScene* scene;
    PxGetPhysics().getScenes(&scene,1);
    PxU32 nbActors = scene->getNbActors(PxActorTypeSelectionFlag::eRIGID_DYNAMIC | PxActorTypeSelectionFlag::eRIGID_STATIC);
    if(nbActors)
    {
        std::vector<PxRigidActor*> actors(nbActors);
        scene->getActors(PxActorTypeSelectionFlag::eRIGID_DYNAMIC | PxActorTypeSelectionFlag::eRIGID_STATIC, (PxActor**)&actors[0], nbActors);
        renderActors(&actors[0], (PxU32)actors.size(), true);
    }

    glEnable(GL_LIGHTING);

    m_tree.drawLines();
    m_tree.draw();

    glDisable(GL_LIGHTING);

    // Draw grid
    glColor4f(0.5f, 0.5f, 0.5f, 0.25f);
    glBegin(GL_LINES);
    for (int s = 200, i = -s; i <= s; i += 10) {
        glVertex3f(i /2.0f,  -0.0f, -s /2.0f);
        glVertex3f(i /2.0f,  -0.0f, +s /2.0f);
        glVertex3f(-s /2.0f, -0.0f, i /2.0f);
        glVertex3f(+s /2.0f, -0.0f, i /2.0f);
    }
    glEnd();

    glDisable(GL_DEPTH_TEST);


    showSubtitles(m_puzzles->infoToPrint, m_painter);
    showPermanentStat(m_dynamicsMessage, m_painter);
}

PxRigidDynamic* World::createDynamic(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity)
{
    if(m_dyanmicsCount <= 0)
    {
        m_dynamicsMessage = "Number of Dynamics: " + QString::number(0);
    }

    else if(m_dyanmicsCount > 0)
    {
        m_dyanmicsCount--;

        PxRigidDynamic* dynamic = PxCreateDynamic(*m_physics, t, geometry, *m_material, 10.0f);
        dynamic->setAngularDamping(0.5f);
        dynamic->setLinearVelocity(velocity);
        m_scene->addActor(*dynamic);
        setupFiltering(dynamic, FilterGroup::eBALL, FilterGroup::eHOLE | FilterGroup::eRED_BOX);

        m_dynamicsMessage = "Number of Dynamics: " + QString::number(m_dyanmicsCount);
        return dynamic;
    }
}

void World::createStack(const PxTransform& t, PxU32 size, PxReal halfExtent)
{
    for(PxU32 i=0; i<size;i++)
    {
	    for(PxU32 j=0;j<size-i;j++)
	    {
		    for(PxU32 k=0;k<size-i;k++)
		    {
                PxShape* shape = m_physics->createShape(PxBoxGeometry(halfExtent, halfExtent, halfExtent), *m_material, true);
                PxTransform localTm(PxVec3(PxReal(j*2) - PxReal(size-i),
                                           PxReal(i*2+1),
                                           PxReal(k*2) - PxReal(size-i))
                                           * halfExtent);
//                qDebug() <<PxReal(j*2) - PxReal(size-i)<<","<<PxReal(i*2+1)<<","<<-PxReal(k) + PxReal(size-k);
			    PxRigidDynamic* body = m_physics->createRigidDynamic(t.transform(localTm));

                body->attachShape(*shape);
			    PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
                m_scene->addActor(*body);
                if (i == 2 && j == 1 && k == 1) {
                    if (!m_redBlock)
                    {
                        m_redBlock = body;
                        setupFiltering(body, FilterGroup::eRED_BOX, FilterGroup::eBALL);

                    }
                }

                shape->release();
		    }
	    }
    }
}

PxRigidStatic* World::createBox(const PxTransform& t, PxReal x, PxReal y, PxReal z)
{
    PxShape* shape = m_physics->createShape(PxBoxGeometry(x, y, z), *m_material, true);

    PxRigidStatic* body = m_physics->createRigidStatic(t);
    body->attachShape(*shape);
    m_scene->addActor(*body);

    shape->release();
    return body;
}

void World::initPhysics(bool interactive)
{
    m_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_allocator, m_errorCallback);
    PxProfileZoneManager* profileZoneManager = &PxProfileZoneManager::createProfileZoneManager(m_foundation);
    m_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_foundation, PxTolerancesScale(),true,profileZoneManager);
    if(m_physics->getPvdConnectionManager())
    {
        m_physics->getVisualDebugger()->setVisualizeConstraints(true);
        m_physics->getVisualDebugger()->setVisualDebuggerFlag(PxVisualDebuggerFlag::eTRANSMIT_CONTACTS, true);
        m_physics->getVisualDebugger()->setVisualDebuggerFlag(PxVisualDebuggerFlag::eTRANSMIT_SCENEQUERIES, true);
        m_connection = PxVisualDebuggerExt::createConnection(m_physics->getPvdConnectionManager(), PVD_HOST, 5425, 10);
    }

    PxSceneDesc sceneDesc(m_physics->getTolerancesScale());
    sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
    m_dispatcher = PxDefaultCpuDispatcherCreate(2);
    sceneDesc.cpuDispatcher	= m_dispatcher;
    sceneDesc.filterShader	= WorldFilterShader;
    sceneDesc.simulationEventCallback	= this;
    sceneDesc.contactModifyCallback = this;
    m_scene = m_physics->createScene(sceneDesc);

    m_material = m_physics->createMaterial(0.5f, 0.5f, 0.6f);

    PxRigidStatic* groundPlane = PxCreatePlane(*m_physics, PxPlane(0,1,0,0), *m_material);
    m_scene->addActor(*groundPlane);

    createStack(PxTransform(PxVec3(0,  0,  -40.0f)), 5, 2.0f);
    createStack(PxTransform(PxVec3(0,  0,  40.0f)), 5, 2.0f);
    createStack(PxTransform(PxVec3(-40, 0, -40.0f)), 5, 2.0f);
    createStack(PxTransform(PxVec3(-40, 0, 40.0f)), 5, 2.0f);

    createBox(PxTransform(PxVec3(-60,  60,  -80.0f)), 30, 60, 2);
    createBox(PxTransform(PxVec3(-15,  15,  -80.0f)), 15, 15, 2);
    createBox(PxTransform(PxVec3(-15,  90,  -80.0f)), 15, 30, 2);
    m_hole = createBox(PxTransform(PxVec3(-15,  45,  -80.0f)), 15, 15, 2);
    setupFiltering(m_hole, FilterGroup::eHOLE, FilterGroup::eBALL);
    createBox(PxTransform(PxVec3(30,  60,  -80.0f)), 30, 60, 2);


    if(!interactive)
        createDynamic(PxTransform(PxVec3(0,40,100)), PxSphereGeometry(10), PxVec3(0,-50,-100));
}

void World::stepPhysics(bool interactive)
{
    PX_UNUSED(interactive)
    m_scene->simulate(1.0f/20.0f);
    m_scene->fetchResults(true);
}

void World::cleanupPhysics(bool interactive)
{
    PX_UNUSED(interactive)
    m_scene->release();
    m_dispatcher->release();
    PxProfileZoneManager* profileZoneManager = m_physics->getProfileZoneManager();
    if(m_connection != NULL)
        m_connection->release();
    m_physics->release();
    profileZoneManager->release();
    m_foundation->release();
}

void World::renderActors(PxRigidActor** actors, const PxU32 numActors, bool shadows)
{

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

            if (actors[i] == m_redBlock)
                glColor4f(0.9f, 0, 0, 1.0f);
            else if (actors[i] == m_hole)
                glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
            else
                glColor4f(0.9f, 0.9f, 0.9f, 1.0f);

//            if(sleeping)
//                glColor4f(0.9f, 0.9f, 0.9f, 1.0f);
//            else
//                glColor4f(0.0f, 0.75f, 0.0f, 1.0f);
            renderGeometry(h);
            glPopMatrix();

            // notice this are really fake shadows
            if(shadows)
            {
                const PxVec3 shadowDir(0.0f, -0.7071067f, -0.7071067f);
                const PxReal shadowMat[]={ 1,0,0,0, -shadowDir.x/shadowDir.y,0,-shadowDir.z/shadowDir.y,0, 0,0,1,0, 0,0,0,1 };
                glPushMatrix();
                glMultMatrixf(shadowMat);
                glMultMatrixf((float*)&shapePose);
                glDisable(GL_LIGHTING);
                glColor4f(0.0f, 0.05f, 0.08f, 1);
                renderGeometry(h);
                glEnable(GL_LIGHTING);
                glPopMatrix();
            }
        }
    }

}

void World::renderGeometry(const PxGeometryHolder& h)
{
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable( GL_BLEND );

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
    case PxGeometryType::eCAPSULE:
    case PxGeometryType::eCONVEXMESH:
    case PxGeometryType::eGEOMETRY_COUNT:
    case PxGeometryType::eHEIGHTFIELD:
    case PxGeometryType::ePLANE:
    case PxGeometryType::eTRIANGLEMESH:
        // TODO: implement
        break;
    case PxGeometryType::eINVALID:
        qCritical("invalid actor in renderGeometry");
        break;
    }
}

void World::tick(float seconds)
{
    m_camera.update(seconds);
    stepPhysics(true);
}

void World::shootDynamic()
{
    PxTransform transform;
    PxVec3 dir(m_camera.m_lookAt.x, m_camera.m_lookAt.y,m_camera.m_lookAt.z);
    PxVec3 eye(m_camera.m_position.x, m_camera.m_position.y,m_camera.m_position.z);

    dir = dir - eye;
    dir.normalize();

    PxVec3 viewY = dir.cross(PxVec3(0,1,0));

    if(viewY.normalize()<1e-6f) {
        transform = PxTransform(eye);
    } else {
        PxMat33 m(dir.cross(viewY), viewY, -dir);
        transform = PxTransform(eye, PxQuat(m));
    }

    createDynamic(transform, PxSphereGeometry(3.0f), dir*100);
}

void World::showSubtitles(QString &info, QPainter* m_painter) // eventually fading away
{
    if(info!= " ")
    {
        m_painter->setPen(QPen(Qt::red));
        m_painter->setFont(QFont("Monospace", 11));
        m_painter->drawText(QRect(20,200,600,100), Qt::AlignLeft,  info);
        if(m_subTimer.elapsed()>3000)
        {
            info = " ";
        }
    }
    else
    {
        m_subTimer.restart();
    }

}

void World::showPermanentStat(QString &info, QPainter* m_painter)
{
    m_painter->setPen(QPen(Qt::gray));
    m_painter->setFont(QFont("Monospace", 11));
    m_painter->drawText(QRect(20,80,600,100), Qt::AlignLeft, info);

    m_painter->setPen(QPen(Qt::white));
    m_painter->drawText(QRect(20,120,600,100), Qt::AlignLeft, "Find the red box!");
    m_painter->drawText(QRect(20,140,600,100), Qt::AlignLeft, "Press space to shoot!");
}

glm::mat4 World::getPMatrix()
{
    return m_camera.pMatrix;
}

glm::mat4 World::getVMatrix()
{
    return m_camera.vMatrix;
}

void World::rotateMouse(glm::vec2 delta)
{
    m_camera.mouseRotation(delta);
}

void World::enableForward(bool flag)
{
    m_camera.pressingForward = flag;
}

void World::enableBackward(bool flag)
{
    m_camera.pressingBackward = flag;
}

void World::enableLeft(bool flag)
{
    m_camera.pressingLeft = flag;
}

void World::enableRight(bool flag)
{
    m_camera.pressingRight = flag;
}

void World::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
{
    for(PxU32 i=0; i < nbPairs; i++)
    {
        const PxContactPair& cp = pairs[i];
        if(cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
        {
            if((pairHeader.actors[0] == m_hole) || (pairHeader.actors[1] == m_hole))
            {
//                PxActor* otherActor = (mSubmarineActor == pairHeader.actors[0]) ? pairHeader.actors[1] : pairHeader.actors[0];
//                Seamine* mine =  reinterpret_cast<Seamine*>(otherActor->userData);
//                // insert only once
//                if(std::find(mMinesToExplode.begin(), mMinesToExplode.end(), mine) == mMinesToExplode.end())
//                    mMinesToExplode.push_back(mine);
//                break;
                qDebug() << "hit the hole";
                emit m_puzzles->puzzlesSolved("You have passed through the hole");

            }
            else
            {
                emit m_puzzles->puzzlesSolved("You have hit the hidden box");
                qDebug() << "hit the box";

            }
        }
    }
}

void World::onContactModify(PxContactModifyPair *const pairs, PxU32 count)
{
    for(PxU32 i=0; i< count; i++)
    {
        pairs->contacts.ignore(i);
    }

}
