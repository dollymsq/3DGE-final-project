#include "world.h"


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

        if(filterData0.word0 & FilterGroup::eHOLE || filterData1.word0 & FilterGroup::eHOLE)
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
    m_playerController(NULL),
    m_stackZ(10.0f),
    m_puzzleSolved(false),
    contactFlag(0)
{
    m_puzzles = new Puzzles();
    m_puzzles->level = 0;

    Vector4 grey    = Vector4(0.8f, 0.8f, 0.8f, 1.0f);
    Vector4 red     = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
    Vector4 orange  = Vector4(1.0f, 0.5f, 0.0f, 1.0f);
    Vector4 yellow  = Vector4(1.0f, 1.0f, 0.0f, 1.0f);
    Vector4 green   = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
    Vector4 blue    = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
    Vector4 indigo  = Vector4(0.3f, 0.0f, 0.5f, 1.0f);
    Vector4 violet  = Vector4(0.56f, 0.0f, 1.0f, 1.0f);
    Vector4 darkgrey = Vector4(.2f,.2f,.2f,1.0f);
    pallete[0]= grey;
    pallete[1]= red;
    pallete[2]= orange;
    pallete[3]= yellow;
    pallete[4]= green;
    pallete[5]= blue;
    pallete[6]= indigo;
    pallete[7]= violet;
    pallete[8]= darkgrey;

}

World::~World()
{
    //TODO: find a better place
    cleanupPhysics(true);
    delete m_puzzles;
    for(int i = 0; i < m_renderableList.size(); i++)  {
        delete m_renderableList.at(i);
    }
}

void World::init(float aspectRatio)
{
    // camera
    m_camera.setAspectRatio(aspectRatio);

    initializeOpenGLFunctions();

//    m_tree = Tree();
//    m_treeTexId = loadTexture("treebark.jpg");
//    m_tree.generate(LParser::testTree());

    initPhysics(true);

    m_cooking = PxCreateCooking(PX_PHYSICS_VERSION, m_physics->getFoundation(), PxCookingParams(PxTolerancesScale()));

//    if(m_cooking)  {
//        std::cout << "yay!" << std::endl;
//    }
//    else {
//        std::cout << "shit" << std::endl;
//    }

//    createTreeTriMesh(m_tree);
    setUpRoomOne();
    PxTransform pose;
    pose.p = PxVec3(0,0,0);
    pose.q= PxQuat(0,PxVec3(0,1,0));
//    createTriMesh(&m_tree,QString("tree"),pose,m_material);
//    createTriMesh(&apple,QString("apple"),pose,m_material;

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

//    m_dyanmicsCount = 40;
    m_dyanmicsCount = 900;
    m_dynamicsMessage = "Number of Balls Left: " + QString::number(m_dyanmicsCount);

    m_levelinfo = "Level 1 - Find and trigger the red box!";
    initShaders();

//    m_treeTexId = loadTexture("treebark.jpg");
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


//    glColor4f(1.0f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_LIGHTING);

    PxScene* scene;
    PxGetPhysics().getScenes(&scene,1);
    PxU32 nbActors = scene->getNbActors(PxActorTypeSelectionFlag::eRIGID_DYNAMIC | PxActorTypeSelectionFlag::eRIGID_STATIC);
    if(nbActors)
    {
        std::vector<PxRigidActor*> actors(nbActors);
        scene->getActors(PxActorTypeSelectionFlag::eRIGID_DYNAMIC | PxActorTypeSelectionFlag::eRIGID_STATIC, (PxActor**)&actors[0], nbActors);
        renderActors(&actors[0], (PxU32)actors.size(), true);
    }


    glDisable(GL_LIGHTING);

//    // Draw grid
//    glColor4f(0.5f, 0.5f, 0.5f, 0.25f);
//    glBegin(GL_LINES);
//    for (int s = 200, i = -s; i <= s; i += 10) {
//        glVertex3f(i /2.0f,  -0.0f, -s /2.0f);
//        glVertex3f(i /2.0f,  -0.0f, +s /2.0f);
//        glVertex3f(-s /2.0f, -0.0f, i /2.0f);
//        glVertex3f(+s /2.0f, -0.0f, i /2.0f);
//    }
//    glEnd();

    glDisable(GL_DEPTH_TEST);


    showSubtitles(m_puzzles->infoToPrint, m_painter);
    showPermanentStat(m_dynamicsMessage, m_painter);
    showLevelStat(m_levelinfo, m_painter);
}

PxRigidDynamic* World::createDynamic(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity,int color)
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
//        dynamic->setName("sphere");
        m_renderables.insert(dynamic,&sphereMesh);
        m_color.insert(dynamic,color);
        m_scene->addActor(*dynamic);
        setupFiltering(dynamic, FilterGroup::eBALL, FilterGroup::eHOLE | FilterGroup::eRED_BOX | FilterGroup::eGROUND | FilterGroup::eSTEPPING_BOX);
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

                m_renderables.insert(body,&cubeMesh);
                m_color.insert(body,0);
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



PxRigidStatic* World::createBox(const PxTransform& t, PxReal x, PxReal y, PxReal z, int color, bool isTransparent, bool isShadows)
{
    PxShape* shape = m_physics->createShape(PxBoxGeometry(x, y, z), *m_material, true);

    PxRigidStatic* body = m_physics->createRigidStatic(t);
    body->attachShape(*shape);
    m_renderables.insert(body,&cubeMesh);
    m_color.insert(body,color);
    if(isTransparent)
        body->setName("transparent");
    m_scene->addActor(*body);

    if(!isShadows)
        m_shadows.insert(body);
    shape->release();
    return body;
}

PxRigidDynamic* World::createDynamicBox(const PxTransform& t, PxReal x, PxReal y, PxReal z, bool isTransparent, bool isShadows, int colornum)
{
    PxShape* shape = m_physics->createShape(PxBoxGeometry(x, y, z), *m_material, true);

    PxRigidDynamic* body = m_physics->createRigidDynamic(t);
    body->attachShape(*shape);
    PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
    if(!isTransparent)
        m_renderables.insert(body,&cubeMesh);
    if(!isShadows)
        m_shadows.insert(body);
    m_scene->addActor(*body);
    m_color[body] = colornum;

    shape->release();
    return body;
}

PxRigidStatic* World::createTriMesh(Renderable *r,const PxTransform &t,PxMaterial *m_material,int color,bool isTransparent,bool isShadow)  {
    QVector<PxVec3> verts = r->getVerts();
    QVector<PxU32> inds = r->getInds();

    PxVec3* vertData = verts.data();
    PxU32* indData = inds.data();

    PxTriangleMeshDesc desc = PxTriangleMeshDesc();
    desc.setToDefault();
    desc.points.count = verts.size();
    desc.triangles.count = inds.size();

    desc.points.stride = sizeof(PxVec3);
    desc.points.data = vertData;

    desc.triangles.stride = sizeof(PxU32)*3;
    desc.triangles.data = indData;

    PxDefaultMemoryOutputStream stream;

    if(m_cooking->cookTriangleMesh(desc,stream))
    {
        std::cout << "Mesh has been baked successfully" << std::endl;
    }
    else
    {
        std::cout << "Mesh failed baking." << std::endl;
    }

    PxDefaultMemoryInputData istream(stream.getData(),stream.getSize());
    PxTriangleMesh *triangleMesh = m_physics->createTriangleMesh(istream);

    PxRigidStatic* body = m_physics->createRigidStatic(t);
    PxTriangleMeshGeometry geom = PxTriangleMeshGeometry(triangleMesh,PxMeshScale());
    PxShape *triangleMeshShape = m_physics->createShape(geom,*m_material,true);

    body->attachShape(*triangleMeshShape);

    if(!isTransparent)
        m_renderables[body] = r;
    if(!isShadow)
        m_shadows.insert(body);
    m_color.insert(body,color);
    m_scene->addActor(*body);

    triangleMeshShape->release();

    return body;
}

void World::createTreeActors(Tree &t)  {
    glm::mat4 CTM;
    QVector<glm::vec3> points = t.getPoints();
    QVector<float> thickness = t.getThickness();
    for(int i = 0; i < points.size(); i+=2)  {

        CTM = glm::translate(CTM,t.getPosition());
        CTM = glm::translate(CTM,(points.at(i+1) + points.at(i))/2.0f);

        CTM *= glm::orientation(glm::normalize(points.at(i+1) - points.at(i)), glm::vec3(0,1,0));
        glm::vec3 left = glm::normalize(glm::cross(glm::vec3(0,1,0),glm::normalize(points.at(i+1) - points.at(i))));

        CTM = glm::rotate(CTM,(float)(M_PI/2.0f),glm::vec3(0,0,-1));
        glm::vec3 scaleVec = glm::vec3(thickness.at(i)/10.0f,glm::length((points.at(i+1) - points.at(i))),thickness.at(i)/10.0f);
        CTM = glm::scale(CTM,scaleVec);

//        PxShape* cylinder = m_physics->createShape(PxCapsuleGeometry(1.0f,.5f),*m_material,false);
        PxShape * cylinder = m_physics->createShape(PxBoxGeometry(thickness.at(i)/10.0f,glm::length((points.at(i+1) - points.at(i))),thickness.at(i)/10.0f), *m_material, true);

        glm::vec3 pos = t.getPosition() + ((points.at(i+1) + points.at(i))/2.0f);
        float angle = glm::angle(glm::vec3(0,1,0),glm::normalize(points.at(i+1) - points.at(i)));
        PxQuat quat(0,PxVec3(0,1,0));
        if(fabs(angle) > 1e-6)
            quat = PxQuat(angle,PxVec3(left.x,left.y,left.z));
        PxTransform transform(PxVec3(pos.x,pos.y,pos.z),quat);

        PxRigidStatic* body = m_physics->createRigidStatic(transform);
        body->attachShape(*cylinder);

        body->setName("tree");
        m_scene->addActor(*body);

        cylinder->release();
    }
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

//    groundPlane = PxCreatePlane(*m_physics, PxPlane(0,1,0,0), *m_material);
//    m_scene->addActor(*groundPlane);
//    setupFiltering(groundPlane, FilterGroup::eGROUND, FilterGroup::eBALL);

    if(!interactive)
        createDynamic(PxTransform(PxVec3(0,40,100)), PxSphereGeometry(10), PxVec3(0,-50,-100));
}

void World::setUpRoomOne()  {
    createStack(PxTransform(PxVec3(-60,  0,  -60.0f)), 5, 2.0f);
    createStack(PxTransform(PxVec3(-60,  0,  60.0f)), 5, 2.0f);
    createStack(PxTransform(PxVec3(60, 0, -60.0f)), 5, 2.0f);
    createStack(PxTransform(PxVec3(60, 0, 60.0f)), 5, 2.0f);

    //the floor
    createBox(PxTransform(PxVec3(0,-100 -1e-1,-100),PxQuat(0,PxVec3(1,0,0))),150,100,200);

    //the front wall and hole
    createBox(PxTransform(PxVec3(-82.5,  60,  -100.0f)), 67.5, 60, 2);
    createBox(PxTransform(PxVec3(0,  15,  -100.0f)), 15, 15, 2);
    createBox(PxTransform(PxVec3(0,  90,  -100.0f)), 15, 30, 2);
    m_hole = createBox(PxTransform(PxVec3(0,  45,  -100.0f)), 17.5, 17.5, 1.0f);
    setupFiltering(m_hole, FilterGroup::eHOLE, FilterGroup::eBALL);
    createBox(PxTransform(PxVec3(82.5,  60,  -100.0f)), 67.5, 60, 2);

    //the back wall
    createBox(PxTransform(PxVec3(0,60,100.0f)),150,60,2,0,false,false);

    //the left wall
    createBox(PxTransform(PxVec3(-150.0f,60,0)),2,60,100);

    //the front wall
    createBox(PxTransform(PxVec3(150.0f,60,-52.5f)),2,60,45);
    createBox(PxTransform(PxVec3(150.0f,60,52.5f)),2,60,45);
    createBox(PxTransform(PxVec3(150.0f,90,0)),2,30,7.5);
    m_door = createBox(PxTransform(PxVec3(150.0f,30,0)),2,30,7.5);
    m_color.insert(m_door,8);

    //trees
    Tree *t1 = new Tree();
    t1->generate(LParser::testTree());
    createTriMesh(t1,PxTransform(PxVec3(50,0,0),PxQuat(0,PxVec3(0,1,0))),m_material);
    m_renderableList.append(t1);

    Tree *t2 = new Tree();
    t2->generate(LParser::testTree());
    createTriMesh(t2,PxTransform(PxVec3(-60,0,20),PxQuat(0,PxVec3(0,1,0))),m_material);
    m_renderableList.append(t2);


    //create domino
    m_domino = createDynamicBox(PxTransform(PxVec3(0,  30,  -120.0f)), 10, 30, 2, false, 1);
    setupFiltering(m_domino, FilterGroup::eGROUND, FilterGroup::eBALL);
    createDynamicBox(PxTransform(PxVec3(0,  40,  -140.0f)), 10, 40, 2, false, 2);
    createDynamicBox(PxTransform(PxVec3(0,  50,  -160.0f)), 10, 50, 2, false, 3);
    createDynamicBox(PxTransform(PxVec3(0,  50,  -180.0f)), 10, 50, 2, false, 4);
    createDynamicBox(PxTransform(PxVec3(0,  50,  -200.0f)), 10, 50, 2, false, 5);
    createDynamicBox(PxTransform(PxVec3(0,  50,  -220.0f)), 10, 50, 2, false, 6);
    createDynamicBox(PxTransform(PxVec3(0,  50,  -240.0f)), 10, 50, 2, false, 7);

    //create spheres
    for(int i = 0; i < 10; i++)  {
        createDynamic(PxTransform(Calc::random(-90,90),2,Calc::random(-90,90)),PxSphereGeometry(3.0f));
    }

    m_controllerManager = PxCreateControllerManager(*m_scene);

    const float gScaleFactor    	= 1.5f;
	const float gStandingSize		= 1.0f * gScaleFactor;
	const float gCrouchingSize		= 0.25f * gScaleFactor;
	const float gControllerRadius	= 0.3f * gScaleFactor;

    PxCapsuleControllerDesc desc;
	desc.position = PxExtendedVec3(50.0f, 50.0f, 50.0f);
    desc.contactOffset			= 0.05f;
    desc.stepOffset			= 0.01;
    desc.slopeLimit			= 0.5f;
    desc.radius				= 5.0f;
    desc.height				= 20.0f;
    desc.upDirection = PxVec3(0, 1, 0);
    desc.material = m_material;


//    desc.invisibleWallHeight	= 0.0f;
//    desc.maxJumpHeight			= 0.0f;
//    desc.scaleCoeff = 0.0f;
//    desc.volumeGrowth = 0.0f;
//    desc.density = 0.0f;
//    desc.material = m_material;

//	desc.mReportCallback		= this;
//	desc.mBehaviorCallback		= this;

/*
 THE DESC IS VALID IF AND ONLY IF
    if(!PxControllerDesc::isValid())	return false;
	if(radius<=0.0f)					return false;
	if(height<=0.0f)					return false;
	if(stepOffset>height+radius*2.0f)	return false;	// Prevents obvious mistakes
	return true;

    if(		type!=PxControllerShapeType::eBOX
		&&	type!=PxControllerShapeType::eCAPSULE)
		return false;
	if(scaleCoeff<0.0f)		return false;
	if(volumeGrowth<1.0f)	return false;
	if(density<0.0f)		return false;
	if(slopeLimit<0.0f)		return false;
	if(stepOffset<0.0f)		return false;
	if(contactOffset<=0.0f)	return false;
	if(!material)			return false;

*/
    std::cout << "CONTROLLER VALID" << " " << desc.isValid() <<  " (should be 1)" << std::endl;
    m_playerController = m_controllerManager->createController(desc);

    //the stepping box
    m_steppingbox = createBox(PxTransform(PxVec3(0,  2,  0)), 10, 2, 10);
    setupFiltering(m_steppingbox, FilterGroup::eSTEPPING_BOX, FilterGroup::eBALL);
}

void World::setUpRoomTwo()  {
//                        m_camera.m_position = glm::vec3(-15.0,  80.0,  -120.0);

    //path made from a series of boxes
    createBox(PxTransform(PxVec3(-1,28,-117),PxQuat(0,PxVec3(1,0,0))),18,2,15);

    createBox(PxTransform(PxVec3(-15,28,-140),PxQuat(0,PxVec3(1,0,0))),45,2,15);
    createBox(PxTransform(PxVec3(-30,28,-170),PxQuat(0,PxVec3(1,0,0))),10,2,15);
    createBox(PxTransform(PxVec3(25,28,-170),PxQuat(0,PxVec3(1,0,0))),10,2,15);
    createBox(PxTransform(PxVec3(40,28,-230),PxQuat(0,PxVec3(1,0,0))),10,2,60);


    createBox(PxTransform(PxVec3(-25,28,-215),PxQuat(0,PxVec3(1,0,0))),10,2,45);
    createBox(PxTransform(PxVec3(-5,28,-270),PxQuat(0,PxVec3(1,0,0))),10,2,20);

    //the front wall and hole
    createBox(PxTransform(PxVec3(-82.5,  60,  -291.0f)), 67.5, 60, 1);
    createBox(PxTransform(PxVec3(0,  15,  -291.0f)), 15, 15, 1);
    createBox(PxTransform(PxVec3(0,  90,  -291.0f)), 15, 30, 1);
    m_hole = createBox(PxTransform(PxVec3(0,  45,  -291.0f)), 15, 15, 1.0f,false);
    setupFiltering(m_hole, FilterGroup::eHOLE, FilterGroup::eBALL);
    createBox(PxTransform(PxVec3(82.5,  60,  -291.0f)), 67.5, 60, 1);


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
//    glColor4f(0.9f, 0.9f, 0.9f, 1.0f);
//    std::cout << "shadows first: " << tempShadows << std::endl;
    PxShape* shapes[MAX_NUM_ACTOR_SHAPES];
//    std::cout << "number of actors: " << numActors << std::endl;
    bool tempShadows = shadows;
    for(PxU32 i=0;i<numActors;i++)
    {
        tempShadows = shadows;
        const PxU32 nbShapes = actors[i]->getNbShapes();

        PX_ASSERT(nbShapes <= MAX_NUM_ACTOR_SHAPES);
        actors[i]->getShapes(shapes, nbShapes);
        bool sleeping = actors[i]->isRigidDynamic() ? actors[i]->isRigidDynamic()->isSleeping() : false;

        for(PxU32 j=0;j<nbShapes;j++)
        {
            const PxMat44 shapePose(PxShapeExt::getGlobalPose(*shapes[j], *actors[i]));
            PxGeometryHolder h = shapes[j]->getGeometry();
            bool toRender = true;
            // render object

            glPushMatrix();
            glMultMatrixf((float*)&shapePose);
            if (actors[i] == m_redBlock)
                glColor4f(0.9f, 0, 0, 1.0f);
            else if (!m_renderables.contains(actors[i]) || actors[i]->getName() == "transparent")// actors[i]->getName() == "transparent" ||
                toRender = false;
            else if (m_color.contains(actors[i]))
                glColor4fv(pallete[m_color[actors[i]]].xyzw );
            else
                glColor4f(0.8f, 0.8f, 0.8f, 1.0f);

            if(shapePose.getPosition().y < 0 || m_shadows.contains(actors[i]))
                tempShadows = false;
//            if(sleeping)
//                glColor4f(0.9f, 0.9f, 0.9f, 1.0f);
//            else
//                if(actors[i] == m_hole)
//                    glColor4f(0.5f, 0.8f, 0.8f, 1.0f);
            if(toRender)
                renderGeometry(h,m_renderables[actors[i]]);
            glPopMatrix();

            // notice these are really fake shadows
            if(tempShadows && toRender)
            {
                const PxVec3 shadowDir(0.0f, -0.7071067f, -0.7071067f);
                const PxReal shadowMat[]={ 1,0,0,0, -shadowDir.x/shadowDir.y,0,-shadowDir.z/shadowDir.y,0, 0,0,1,0, 0,0,0,1 };
                glPushMatrix();
                glMultMatrixf(shadowMat);
                glMultMatrixf((float*)&shapePose);
                glDisable(GL_LIGHTING);
                glColor4f(0.0f, 0.05f, 0.08f, 1);
                renderGeometry(h,m_renderables[actors[i]]);
                glEnable(GL_LIGHTING);
                glPopMatrix();
            }
        }
    }

}

void World::renderGeometry(const PxGeometryHolder& h, Renderable *r)
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
                r->draw();
            glPopMatrix();
        }
        break;
    case PxGeometryType::eSPHERE:
        {
            float diam = h.sphere().radius * 2;
            glPushMatrix();
                glScalef(diam, diam, diam);
                r->draw();
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
        r->draw();
        break;
    case PxGeometryType::eINVALID:
        qCritical("invalid actor in renderGeometry");
        break;
    }
}

void World::tick(float seconds)
{
    if (m_playerController != NULL) {
        const PxExtendedVec3 &pos = m_playerController->getPosition();
        m_camera.m_position.x = pos.x;
        m_camera.m_position.y = pos.y;
        m_camera.m_position.z = pos.z;

        m_camera.update(seconds);
        PxVec3 disp(m_camera.m_position.x - pos.x,
                    m_camera.m_position.y - pos.y,
                    m_camera.m_position.z - pos.z);
        disp += PxVec3(0, -0.8f, 0);

//        std::cout << glm::to_string(m_camera.m_position) << std::endl;
        m_playerController->move(disp, 0.1f, seconds, NULL, NULL);
    }

    stepPhysics(true);
}

void World::shootDynamic()
{
    PxTransform transform;
    PxVec3 dir(m_camera.m_lookAt.x, m_camera.m_lookAt.y + 20,m_camera.m_lookAt.z);
    PxVec3 eye(m_camera.m_position.x, m_camera.m_position.y + 20, m_camera.m_position.z);

    dir = dir - eye;
    dir.normalize();

    PxVec3 viewY = dir.cross(PxVec3(0,1,0));

    if(viewY.normalize()<1e-6f) {
        transform = PxTransform(eye);
    } else {
        PxMat33 m(dir.cross(viewY), viewY, -dir);
        transform = PxTransform(eye + dir * 8.0f, PxQuat(m));
    }

    createDynamic(transform, PxSphereGeometry(2.5f), dir*100);
}

void World::showSubtitles(QString &info, QPainter* m_painter) // eventually fading away
{
    if(info!= " ")
    {
        m_painter->setPen(QPen(Qt::red));
        m_painter->setFont(QFont("Monospace", 11));
        m_painter->drawText(QRect(20,200,600,100), Qt::AlignLeft,  info);
        if(m_subTimer.elapsed()>4000)
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
    m_painter->setPen(QPen(Qt::white));
    m_painter->drawText(QRect(20,140,600,100), Qt::AlignLeft, "Press space to shoot!");
    m_painter->setPen(QPen(Qt::gray));
    m_painter->setFont(QFont("Monospace", 11));

    m_painter->drawText(QRect(20,80,600,100), Qt::AlignLeft, info);


}

void World::showLevelStat(QString &info, QPainter* m_painter)
{
    m_painter->setPen(QPen(Qt::white));
    m_painter->drawText(QRect(20,120,600,100), Qt::AlignLeft, info);
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
                PxU32 tempFlag = contactFlag;
                if(tempFlag == FilterGroup::eSTEPPING_BOX)
                    emit m_puzzles->puzzlesSolved("You have used the stepping box to pass through the hole");

                contactFlag |= FilterGroup::eHOLE ;
                if(m_puzzles->level == 1)
                {
                    if(contactFlag & FilterGroup::eSTEPPING_BOX)
                        emit m_puzzles->puzzlesSolved("You have used the stepping box to pass through the hole");
                    else
                        emit m_puzzles->puzzlesSolved("You have passed through the hole");
                }
                else
                    emit m_puzzles->puzzlesSolved("Find and hit the red box first!");

            }
            else if((pairHeader.actors[0] == m_domino) || (pairHeader.actors[1] == m_domino))
            {
                if((contactFlag & FilterGroup::eSTEPPING_BOX) && (contactFlag & FilterGroup::eHOLE ))
                {
                    m_puzzles->level = 2;
                    emit m_puzzles->puzzlesSolved("You have finished this level");
//                    m_camera.m_position = glm::vec3(-15.0,  40.0,  -120.0);
                    m_levelinfo = "Level 3 - Find the path to the next door";
                    setUpRoomTwo();

                }
            }
//            else if((pairHeader.actors[0] == groundPlane) || (pairHeader.actors[1] == groundPlane))//the ground
//                contactFlag = FilterGroup::eGROUND ;
            else if((pairHeader.actors[0] == m_steppingbox) || (pairHeader.actors[1] == m_steppingbox))//for the stepping box
            {
                contactFlag = FilterGroup::eSTEPPING_BOX;
                currentBall = (m_steppingbox == pairHeader.actors[0]) ? pairHeader.actors[1] : pairHeader.actors[0];
            }
            else
            {
                emit m_puzzles->puzzlesSolved("You have hit the hidden box");
                m_levelinfo = "Level 2 - Use some tricks to push down the domino walls";
            }
        }
    }
}

void World::onContactModify(PxContactModifyPair *const pairs, PxU32 count)
{
    qDebug()<<m_puzzles->level;
    if(m_puzzles->level > 0 )
    {
        for(PxU32 i=0; i< count; i++)
        {
            pairs->contacts.ignore(i);
        }
    }
}

GLuint World::loadTexture(const QString &path)  {
    QImage img(path);
    img = QGLWidget::convertToGLFormat(img);
    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(), img.height(),
    0, GL_RGBA, GL_UNSIGNED_BYTE, img.bits());
    return id;
}
