#ifndef NO_CHECK_MACROS
# define NO_CHECK_MACROS
#endif

#include "utils/gl.h"
#include "utils/logger.h"

#ifdef PLATFORM_WIN32
# define WIN32_LEAN_AND_MEAN
# include <Windows.h>
#endif // PLATFORM_WIN32

#include <GL/glu.h>

namespace sb
{
    namespace utils
    {
        // returns true on error
        bool GLCheck(const char* file, int line, const char* call)
        {
            GLuint err = glGetError();

            if (err != GL_NO_ERROR) {
                gLog.err("GL error: \"%s\" at file %s, line %d\n>> %s\n", gluErrorString(err), file, line, call);
            }

            return !!err;
        }

        void print_integer(const char* name,
                           GLuint binding)
        {
            GLuint id;
            glGetIntegerv(binding, (GLint*)&id);
            gLog.info("%s: %d\n", name, id);
        }

        void gl_debug()
        {
            print_integer("ARRAY_BUFFER", GL_ARRAY_BUFFER_BINDING);
            print_integer("ELEMENT_ARRAY_BUFFER", GL_ELEMENT_ARRAY_BUFFER_BINDING);
            print_integer("TEXTURE_2D", GL_TEXTURE_BINDING_2D);
        }
    }
}

