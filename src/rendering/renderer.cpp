#include <algorithm>

#include "rendering/renderer.h"
#include "utils/gl.h"
#include "utils/logger.h"
#include "utils/string.h"
#include "window/native_window_handle.h"

#if PLATFORM_LINUX
# include <X11/Xlib.h>
# include <GL/glx.h>

// typedef required for Renderer::init
typedef GLXContext (*GLXCREATECTXATTRSARBPROC)(::Display*,
                                               GLXFBConfig,
                                               GLXContext,
                                               Bool,
                                               const int*);

namespace sb {

class NativeContextHandle
{
public:
    ::Display *const display;
    const ::Window window;
    const GLXContext glContext;

    static std::shared_ptr<NativeContextHandle>
    create(const NativeWindowHandle &window);

    ~NativeContextHandle();

private:
    NativeContextHandle(::Display *dpy,
                        ::Window wnd,
                        GLXContext ctx):
        display(dpy),
        window(wnd),
        glContext(ctx)
    {}
};

std::shared_ptr<NativeContextHandle>
NativeContextHandle::create(const NativeWindowHandle &handle)
{
    ::Display* dpy = handle.display;
    ::Window wnd = handle.window;
    const GLXFBConfig &fbc = handle.fbConfig;

    gLog.info("creating GL context...\n");
    int ctxAttribs[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
        GLX_CONTEXT_MINOR_VERSION_ARB, 0,
        None
    };

    GLXCREATECTXATTRSARBPROC glXCreateContextAttribsARB =
            (GLXCREATECTXATTRSARBPROC)glXGetProcAddress(
                (GLubyte*)"glXCreateContextAttribsARB");

    if (!glXCreateContextAttribsARB) {
        gLog.err("glXCreateContextAttribsARB not present\n");
        return {};
    }

    GLXContext ctx = glXCreateContextAttribsARB(dpy, fbc, 0, True, ctxAttribs);
    if (!ctx) {
        gLog.err("glXCreateContextAttribsARB failed\n");
        return false;
    }

    glXMakeCurrent(dpy, wnd, ctx);

    auto ret = new NativeContextHandle(dpy, wnd, ctx);
    return std::shared_ptr<NativeContextHandle>(ret);
}

NativeContextHandle::~NativeContextHandle()
{
    if (glContext) {
        glXMakeCurrent(display, 0, 0);
        glXDestroyContext(display, glContext);
        gLog.info("GL context deleted\n");
    }
}

} // namespace sb

#elif PLATFORM_WIN32

#endif // PLATFORM_*

namespace sb {

bool Renderer::initGLEW()
{
    gLog.info("initializing GLEW...\n");
    GLenum error = glewInit();
    if (error != GLEW_OK)
    {
        gLog.err("glewInit failed: %s\n", glewGetErrorString(error));
        return false;
    }
    else
        gLog.info("using GLEW %s\n", glewGetString(GLEW_VERSION));

    gLog.info("checking GL functions availability\n");
    static struct GLFunc {
        const void* func;
        const char* name;
        const char* errMsg;
        enum {
            SevOptional,
            SevRequired
        } severity;
    } functions[] = {
#define FUNC_OPT(name, fail_msg) { (const void*)(name), #name, (fail_msg), GLFunc::SevOptional }
#define FUNC_REQ(name, fail_msg) { (const void*)(name), #name, (fail_msg), GLFunc::SevRequired }
        FUNC_REQ(glGenVertexArrays, 0),
        FUNC_REQ(glDeleteVertexArrays, 0),
        FUNC_REQ(glBindVertexArray, 0),
        FUNC_REQ(glGetVertexAttribiv, 0),
        FUNC_REQ(glVertexAttribPointer, 0),
        FUNC_REQ(glBindAttribLocation, 0),
        FUNC_OPT(glEnableVertexAttribArray, 0),
        FUNC_OPT(glDisableVertexAttribArray, 0),
        FUNC_REQ(glGenBuffers, 0),
        FUNC_REQ(glDeleteBuffers, 0),
        FUNC_REQ(glBindBuffer, 0),
        FUNC_OPT(glCopyBufferSubData, "in-GPU copying no available, will use RAM\n"), // if present, GL_COPY_READ_BUFFER & GL_COPY_WRITE_BUFFER should be available too
        FUNC_REQ(glBufferSubData, 0),
        FUNC_REQ(glGetBufferParameteriv, 0),
        FUNC_REQ(glActiveTexture, 0),
        FUNC_OPT(glGenerateMipmap, "will use glGenerateMipmapEXT if available\n"),
        FUNC_OPT(glGenerateMipmapEXT, "bye bye mipmaps :(\n"),
        FUNC_REQ(glDrawElements, 0),
        FUNC_REQ(glUseProgram, 0),
        FUNC_REQ(glCreateProgram, 0),
        FUNC_REQ(glLinkProgram, 0),
        FUNC_REQ(glDeleteProgram, 0),
        FUNC_REQ(glCreateShader, 0),
        FUNC_REQ(glShaderSource, 0),
        FUNC_REQ(glCompileShader, 0),
        FUNC_REQ(glAttachShader, 0),
        FUNC_REQ(glDeleteShader, 0),
        FUNC_REQ(glDrawElements, 0),
        FUNC_REQ(glUniform1iv, 0),
        FUNC_REQ(glUniform1fv, 0),
        FUNC_REQ(glUniform2fv, 0),
        FUNC_REQ(glUniform3fv, 0),
        FUNC_REQ(glUniform4fv, 0),
        FUNC_REQ(glUniformMatrix4fv, 0)
#undef FUNC_OPT
#undef FUNC_REQ
    };

    uint32_t requiredFunctionsMissing = 0;
    for (size_t i = 0; i < sizeof(functions) / sizeof(functions[0]); ++i) {
        if (functions[i].func != NULL) {
            gLog.info("%-40sOK\n", functions[i].name);
        } else {
            if (functions[i].severity == GLFunc::SevOptional) {
                gLog.warn("%-40sNOT AVAILABLE\n", functions[i].name);
            } else {
                gLog.err("%-40sNOT AVAILABLE\n", functions[i].name);
                ++requiredFunctionsMissing;
            }

            if (functions[i].errMsg != NULL) {
                gLog.info(functions[i].errMsg);
            }
        }
    }

    if (requiredFunctionsMissing > 0) {
        gLog.err("some critical GL functions missing, app may crash\n");
    }

    return requiredFunctionsMissing == 0;
}

Renderer::Renderer():
    mContext(NULL)
{
}

bool Renderer::init(const NativeWindowHandle& handle)
{
    assert(handle.display);
    mContext = NativeContextHandle::create(handle);

    if (!initGLEW()) {
        return false;
    }

    GL_CHECK(glEnable(GL_DEPTH_TEST));
    GL_CHECK(glDepthFunc(GL_LESS));

    GL_CHECK(glEnable(GL_CULL_FACE));
    GL_CHECK(glCullFace(GL_BACK));

    GL_CHECK(glEnable(GL_BLEND));
    GL_CHECK(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    GL_CHECK(glEnable(GL_TEXTURE_2D));

    return true;
}

void Renderer::setClearColor(const Color& c)
{
    glClearColor(c.r, c.g, c.b, c.a);
}

void Renderer::clear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::swapBuffers()
{
    glXSwapBuffers(mContext->display, mContext->window);
}

void Renderer::setViewport(unsigned x,
                           unsigned y,
                           unsigned width,
                           unsigned height)
{
    glViewport(x, y, width, height);

    // adjust aspect ratio
    // TODO
    //mCamera.setPerspectiveMatrix(PI_3, (float)cx / (float)cy);
}

} // namespace sb

