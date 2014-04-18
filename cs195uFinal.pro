#
#   CS 195u
#

QT += core gui opengl

TARGET = cs195uFinal
TEMPLATE = app

# PHYSX build types:
# -CHECKED debug mode + instruments
# -PROFILE instruments
# -RELEASE

PHYSX = $$(PHYSX_PATH)

isEmpty(PHYSX) {
    # default sunlab path
    PHYSX = /contrib/projects/physX3.3.0
}

INCLUDEPATH += $${PHYSX}/Include

macx:LIBS += -L$${PHYSX}/Lib/osx64
unix:!macx:LIBS += -L$${PHYSX}/Lib/linux64

CONFIG(release, debug|release) {
    DEFINES += QT_NO_DEBUG_OUTPUT QT_NO_WARNING_OUTPUT

    # PHYSX library
    DEFINES += NDEBUG

    !clang:LIBS += -Wl,--start-group
    LIBS += -lPvdRuntime \
            -lSimulationController \
            -lSceneQuery \
            -lLowLevel \
            -lLowLevelCloth \
            -lPhysX3 \
            -lPhysX3Vehicle \
            -lPhysX3Cooking \
            -lPhysX3Extensions \
            -lPhysX3CharacterKinematic \
            -lPhysXProfileSDK \
            -lPhysXVisualDebuggerSDK \
            -lPxTask \
            -lPhysX3Common
    !clang:LIBS += -Wl,--end-group

} else {
    # PHYSX library
    DEFINES += _DEBUG \
        PHYSX_PROFILE_SDK \
        PX_DEBUG \
        PX_CHECKED \
        PX_SUPPORT_VISUAL_DEBUGGER

    !clang:LIBS += -Wl,--start-group
    LIBS += -lPvdRuntimeCHECKED \
        -lSimulationControllerCHECKED \
        -lSceneQueryCHECKED \
        -lLowLevelCHECKED \
        -lLowLevelClothCHECKED \
        -lPhysX3CHECKED \
        -lPhysX3VehicleCHECKED \
        -lPhysX3CookingCHECKED \
        -lPhysX3ExtensionsCHECKED \
        -lPhysX3CharacterKinematicCHECKED \
        -lPhysXProfileSDKCHECKED \
        -lPhysXVisualDebuggerSDKCHECKED \
        -lPxTaskCHECKED \
        -lPhysX3CommonCHECKED
    !clang:LIBS += -Wl,--end-group
}

LIB = lib
RES = res

# remove this eventually
# GLU library
#LIBS += -lGLU

# GLM Library
GLM = $${LIB}/glm-0.9.5.1
DEFINES += GLM_FORCE_RADIANS
INCLUDEPATH += $${GLM}

INCLUDEPATH += src
DEPENDPATH += $$INCLUDEPATH

SOURCES += \
    src/game/platworld.cpp \
    src/game/main.cpp \
    src/assets/obj.cpp \
    src/window/openglwindow.cpp \
    src/window/window.cpp \
    src/scene/camera.cpp \
    src/game/world.cpp \
    src/game/puzzles.cpp

HEADERS += \
    src/game/platworld.h \
    src/game/player.h \
    src/game/vertex.h \
    src/math/vector.h \
    src/assets/obj.h \
    src/window/openglwindow.h \
    src/window/window.h \
    src/scene/camera.h \
    src/game/world.h \
    src/game/puzzles.h

RESOURCES += \
    $${RES}/resources.qrc

#  hide object files and misc
OBJECTS_DIR = $${OUT_PWD}/.obj
MOC_DIR = $${OUT_PWD}/.moc
RCC_DIR = $${OUT_PWD}/.rcc
UI_DIR = $${OUT_PWD}/.ui

clang {
    QMAKE_LFLAGS += -std=c++11 -stdlib=libc++
    QMAKE_CXXFLAGS += -stdlib=libc++
}

macx:QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7

QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3 -fno-strict-aliasing
QMAKE_CXXFLAGS_WARN_ON -= -Wall
QMAKE_CXXFLAGS_WARN_ON += -Waddress -Warray-bounds -Wc++0x-compat -Wchar-subscripts -Wformat\
                          -Wmain -Wmissing-braces -Wparentheses -Wreorder -Wreturn-type \
                          -Wsequence-point -Wsign-compare -Wstrict-aliasing -Wstrict-overflow=1 -Wswitch \
                          -Wtrigraphs -Wuninitialized -Wunused-label -Wunused-variable \
                          -Wvolatile-register-var -Wno-extra
