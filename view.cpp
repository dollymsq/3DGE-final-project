#include "view.h"
#include <QApplication>
#include <QKeyEvent>

View::View(QWidget *parent) : QGLWidget(parent)
{
    // View needs all mouse move events, not just mouse drag events
    setMouseTracking(true);

    // Hide the cursor since this is a fullscreen app
    setCursor(Qt::BlankCursor);

    // View needs keyboard focus
    setFocusPolicy(Qt::StrongFocus);

    // The game loop is implemented using a timer
    connect(&timer, SIGNAL(timeout()), this, SLOT(tick()));
}

View::~View()
{
}

void View::initializeGL()
{
    glEnable(GL_NORMALIZE);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    // All OpenGL initialization *MUST* be done during or after this
    // method. Before this method is called, there is no active OpenGL
    // context and all OpenGL calls have no effect.

    // Start a timer that will try to get 60 frames per second (the actual
    // frame rate depends on the operating system and other running programs)
    time.start();
    timer.start(1000 / 60);

    // Center the mouse, which is explained more in mouseMoveEvent() below.
    // This needs to be done here because the mouse may be initially outside
    // the fullscreen window and will not automatically receive mouse move
    // events. This occurs if there are two monitors and the mouse is on the
    // secondary monitor.
    QCursor::setPos(mapToGlobal(QPoint(width() / 2, height() / 2)));

//    cout<<"Press number key 1 or 2 to choose file to load:"<<endl;
//    cout<<"1 for easy model(default), 2 for hard model:"<<endl;

//    int num;
//    cin>>num;

//    if(num == 2)
//    {
//        m_scene.loadModel(":/mesh/level_hard.obj");
//        m_scene.loadTexture(":/texture/level_hard.png");
//    }
//    else
    {
        m_scene.loadModel(":/mesh/level_easy.obj");
        m_scene.loadTexture(":/texture/level_easy.png");
    }
    injump = true;
}

void View::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set up projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(55, (float)width() / (float)height(), 0.01, 1000);

    // set up modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    m_camera.setEyePosition(m_player.start);
    m_camera.lookAt();

    m_scene.render();
    m_player.setDirection(m_camera);
    m_player.render();
}

void View::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void View::mousePressEvent(QMouseEvent *event)
{
}

void View::mouseMoveEvent(QMouseEvent *event)
{
    // This starter code implements mouse capture, which gives the change in
    // mouse position since the last mouse movement. The mouse needs to be
    // recentered after every movement because it might otherwise run into
    // the edge of the screen, which would stop the user from moving further
    // in that direction. Note that it is important to check that deltaX and
    // deltaY are not zero before recentering the mouse, otherwise there will
    // be an infinite loop of mouse move events.
    int deltaX = event->x() - width() / 2;
    int deltaY = event->y() - height() / 2;
    if (!deltaX && !deltaY) return;
    QCursor::setPos(mapToGlobal(QPoint(width() / 2, height() / 2)));

    // TODO: Handle mouse movements here
    m_camera.cameraOrient(deltaX, deltaY);
}

void View::mouseReleaseEvent(QMouseEvent *event)
{
}

void View::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) QApplication::quit();

    // TODO: Handle keyboard presses here
//    else if(event->key() == Qt::Key_1)
//    {
//                m_scene.loadModel(":/mesh/level_easy.obj");
//                m_scene.loadTexture(":/texture/level_easy.png");
//    }
//        else if(event->key() == Qt::Key_2)
//    {
//                m_scene.loadModel(":/mesh/level_hard.obj");
//                m_scene.loadTexture(":/texture/level_hard.png");

//    }

    else if(event->key() == Qt::Key_Space)
    {
        if(!injump)
        {
            m_player.setHeightANDVelocity();
            injump = true;
        }
    }
    else
    {
        if(!injump)
            injump = true;
        if(event->key() == Qt::Key_D)
            m_player.moveRight(m_scene);
        else if(event->key() == Qt::Key_A)
            m_player.moveLeft(m_scene);
        else if(event->key() == Qt::Key_W)
            m_player.moveForward(m_scene);
        else if(event->key() == Qt::Key_S)
            m_player.moveBack(m_scene);

        //FOR DEBUGGING
            else if(event->key() == Qt::Key_L)
                m_player.moveRight(m_scene);
            else if(event->key() == Qt::Key_J)
                m_player.moveLeft(m_scene);
            else if(event->key() == Qt::Key_I)
                m_player.moveForward(m_scene);
            else if(event->key() == Qt::Key_K)
                m_player.moveBack(m_scene);
    }

}

void View::keyReleaseEvent(QKeyEvent *event)
{
}

void View::tick()
{
    // Get the number of seconds since the last tick (variable update rate)
    float seconds = time.restart() * 0.001f;

    // TODO: Implement the game update here
    if(injump)
        m_player.moveUp(m_scene,injump,seconds);

    // Flag this view for repainting (Qt will call paintGL() soon after)
    update();
}
