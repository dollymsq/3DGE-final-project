    QT += core gui opengl

TARGET = platformer
TEMPLATE = app

# If you add your own folders, add them to INCLUDEPATH and DEPENDPATH, e.g.
# INCLUDEPATH += folder1 folder2
# DEPENDPATH += folder1 folder2

SOURCES += main.cpp \
    mainwindow.cpp \
    view.cpp \
    obj.cpp \
    world.cpp \
    platworld.cpp

HEADERS += mainwindow.h \
    view.h \
    vector.h \
    obj.h \
    world.h \
    platworld.h \
    camera.h \
    player.h \
    vertex.h

FORMS += mainwindow.ui

LIBS += -L/usr/local/lib -lGLU
