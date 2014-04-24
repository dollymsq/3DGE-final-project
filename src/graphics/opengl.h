#ifndef OPENGL_H
#define OPENGL_H

#include <QOpenGLFunctions_3_2_Compatibility>
typedef QOpenGLFunctions_3_2_Compatibility GLFunctions;
extern GLFunctions* g_glFunctions;

enum {
    VAO_POSITION,
    VAO_NORMAL,
    VAO_TEX_COORD
};

#ifdef DEBUG_OPENGL
#define gl Q_ASSERT_X(g_glFunctions != NULL, __FILE__, "invalid opengl call"); g_glFunctions
#else
#define gl g_glFunctions
#endif

#endif // OPENGL_H
