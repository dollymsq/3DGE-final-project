#
#   CS 195u
#

QT += core gui opengl

TARGET = cs195uFinal
TEMPLATE = app

win32:CONFIG += win
win {
    contains(QMAKE_TARGET.arch, x86_64) {
        CONFIG += winx64
    } else {
        CONFIG += winx32
    }
}

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
winx64:LIBS += -L$${PHYSX}/Lib/win64

CONFIG(release, debug|release) {
    DEFINES += QT_NO_DEBUG_OUTPUT QT_NO_WARNING_OUTPUT

    # PHYSX library
    unix:DEFINES += NDEBUG

    !clang:gcc:LIBS += -Wl,--start-group

    unix {
        LIBS += \
            -lPvdRuntime \
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
    }

    win {
        LIBS += \
            -lPhysX3CharacterKinematic_x64 \
            -lPhysX3_x64 \
            -lPhysX3Common_x64 \
            -lPhysX3Cooking_x64 \
            -lPhysX3Extensions \
            -lPhysX3Vehicle \
            -lPhysXProfileSDK \
            -lPhysXVisualDebuggerSDK \
            -lPxTask
    }

    !clang:gcc:LIBS += -Wl,--end-group

    # THIS IS BAD
    msvc {
        QMAKE_LFLAGS += \
            /NODEFAULTLIB:libc.lib \
            /NODEFAULTLIB:libcmt.lib \
            /NODEFAULTLIB:libcd.lib \
            /NODEFAULTLIB:libcmtd.lib \
            /NODEFAULTLIB:msvcrtd.lib
    }

} else {
    # PHYSX library
    unix:DEFINES += _DEBUG
    DEFINES += \
        PHYSX_PROFILE_SDK \
        PX_DEBUG \
        PX_CHECKED \
        PX_SUPPORT_VISUAL_DEBUGGER

    !clang:gcc:LIBS += -Wl,--start-group

    unix {
        LIBS += \
            -lPvdRuntimeCHECKED \
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
    }

    win {
        LIBS += \
            -lPhysX3CharacterKinematicCHECKED_x64 \
            -lPhysX3CHECKED_x64 \
            -lPhysX3CommonCHECKED_x64 \
            -lPhysX3CookingCHECKED_x64 \
            -lPhysX3ExtensionsCHECKED \
            -lPhysX3VehicleCHECKED \
            -lPhysXProfileSDKCHECKED \
            -lPhysXVisualDebuggerSDKCHECKED \
            -lPxTaskCHECKED
    }

    !clang:gcc:LIBS += -Wl,--end-group

    # THIS IS BAD
    msvc {
        QMAKE_LFLAGS += \
            /NODEFAULTLIB:libc.lib \
            /NODEFAULTLIB:libcmt.lib \
            /NODEFAULTLIB:msvcrt.lib \
            /NODEFAULTLIB:libcd.lib \
            /NODEFAULTLIB:libcmtd.lib
    }
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
    src/game/platworld.cpp \
    src/game/main.cpp \
    src/assets/obj.cpp \
    src/window/openglwindow.cpp \
    src/window/window.cpp \
    src/scene/camera.cpp \
    src/game/world.cpp \
    src/game/puzzles.cpp \
    src/assets/tree.cpp \
    src/math/lparser.cpp

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
    src/game/puzzles.h \
    src/graphics/debug.h \
    src/math/lparser.h \
    src/assets/tree.h \
    src/math/common.h \
    src/math/levent.h \
    src/math/lcommand.h

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

!clang:gcc {
    QMAKE_CXXFLAGS += -std=c++11
    QMAKE_CXXFLAGS_RELEASE -= -O2
    QMAKE_CXXFLAGS_RELEASE += -O3 -fno-strict-aliasing
    QMAKE_CXXFLAGS_WARN_ON -= -Wall
    QMAKE_CXXFLAGS_WARN_ON += -Waddress -Warray-bounds -Wc++0x-compat -Wchar-subscripts -Wformat\
                              -Wmain -Wmissing-braces -Wparentheses -Wreorder -Wreturn-type \
                              -Wsequence-point -Wsign-compare -Wstrict-aliasing -Wstrict-overflow=1 -Wswitch \
                              -Wtrigraphs -Wuninitialized -Wunused-label -Wunused-variable \
                              -Wvolatile-register-var -Wno-extra
}
