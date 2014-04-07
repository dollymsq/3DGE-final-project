#
#   CS 195u
#

CONFIG += c++11
QT += core gui opengl

TARGET = cs195uFinal
TEMPLATE = app

CONFIG(release, debug|release) {
    DEFINES += QT_NO_DEBUG_OUTPUT QT_NO_WARNING_OUTPUT
}

LIB = lib
RES = res

# GLM Library
GLM = $${LIB}/glm-0.9.5.1
DEFINES += GLM_FORCE_RADIANS
INCLUDEPATH += $${GLM}

INCLUDEPATH += src
DEPENDPATH += $$INCLUDEPATH

SOURCES += \
    src/game/world.cpp \
    src/game/platworld.cpp \
    src/game/main.cpp \
    src/window/mainwindow.cpp \
    src/assets/obj.cpp \
    src/scene/view.cpp

HEADERS += \
    src/game/platworld.h \
    src/game/player.h \
    src/game/vertex.h \
    src/game/world.h \
    src/window/mainwindow.h \
    src/math/vector.h \
    src/assets/obj.h \
    src/scene/view.h \
    src/scene/camera.h

RESOURCES += \
    $${RES}/resources.qrc

#  hide object files and misc
OBJECTS_DIR = $${OUT_PWD}/.obj
MOC_DIR = $${OUT_PWD}/.moc
RCC_DIR = $${OUT_PWD}/.rcc
UI_DIR = $${OUT_PWD}/.ui

#QMAKE_CXXFLAGS += -std=c++11
