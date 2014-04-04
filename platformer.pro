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
    camera.cpp \
    scene.cpp \
    player.cpp

HEADERS += mainwindow.h \
    view.h \
    vector.h \
    obj.h \
    camera.h \
    scene.h \
    player.h

FORMS += mainwindow.ui

LIBS += \-lGLU

RESOURCES += \
    source.qrc
