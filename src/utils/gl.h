#pragma once

namespace sb
{
    namespace utils
    {
        // returns true on error
        bool GLCheck(const char* file, int line, const char* call);

        void gl_debug();
    }
}

#ifndef NO_CHECK_MACROS
# ifdef _DEBUG

#  define GL_CHECK(funccall) (gLog.debug(#funccall), (funccall)), \
                             sb::utils::GLCheck(__FILE__, __LINE__, #funccall)

# else // NDEBUG

#  define GL_CHECK(funccall) (funccall), (glGetError() != GL_NO_ERROR)

# endif // _DEBUG

// returns retval on failure
# define GL_CHECK_RET(funccall, retval) \
    do { \
        if (GL_CHECK(funccall)) { \
            return retval; \
        } \
    } while (0)


#else // !NO_CHECK_MACROS

// compatibility
# define GL_CHECK(funccall)             (funccall)
# define GL_CHECK_RET(funccall, retval) (funccall)

#endif

