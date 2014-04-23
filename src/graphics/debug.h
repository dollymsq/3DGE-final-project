#ifndef GRAPHICS_DEBUG_H
#define GRAPHICS_DEBUG_H

#ifdef DEBUG_OPENGL
    #include <iostream>
    static void getOpenGLErrors(const char* stmt,
                                const char* function,
                                const char* file,
                                int line) {

        GLenum error = glGetError();

        if (error != GL_NO_ERROR)
            fprintf(stderr, "OpenGL Error: Statement %s "
                   "Function %s "
                   "( File %s, "
                   "Line %i )\n",
                   stmt,
                   function,
                   file,
                   line);

        switch(error) {
        case GL_INVALID_ENUM:
            fprintf(stderr, "\tGL_INVALID_ENUM\n");
            break;
        case GL_INVALID_VALUE:
            fprintf(stderr, "\tGL_INVALID_VALUE\n");
            break;
        case GL_INVALID_OPERATION:
            fprintf(stderr, "\tGL_INVALID_OPERATION\n");
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            fprintf(stderr, "\tGL_INVALID_FRAMEBUFFER_OPERATION\n");
            break;
        case GL_OUT_OF_MEMORY:
            fprintf(stderr, "\tGL_OUT_OF_MEMORY\n");
            break;
        }
    }

    #define DEBUG_ERRORS(stmt) do { \
        stmt; \
        getOpenGLErrors(#stmt, Q_FUNC_INFO, __FILE__, __LINE__); \
        } while(0)
#else
    #define DEBUG_ERRORS(stmt) stmt
#endif

#endif //GRAPHICS_DEBUG_H
