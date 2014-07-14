#pragma once

#include <memory>

#if PLATFORM_LINUX
# include <X11/Xlib.h>
# include <X11/Xutil.h>
# include <GL/glx.h>

namespace sb {

class Window;

class NativeWindowHandle
{
public:
    ::Display *const display;
    const ::Window window;
    GLXFBConfig fbConfig;

    static std::unique_ptr<NativeWindowHandle> create(sb::Window &owner,
                                                      unsigned width,
                                                      unsigned height);

    ~NativeWindowHandle();

private:
    Window *owner;

    NativeWindowHandle(Window *owner,
                       ::Display* dpy,
                       ::Window wnd,
                       const GLXFBConfig& fbc):
        display(dpy),
        window(wnd),
        fbConfig(fbc),
        owner(owner)
    {}
};

} // namespace sb

#elif PLATFORM_WIN32
# define WIN32_LEAN_AND_MEAN
# include <Windows.h>

class NativeWindowHandle
{
public:
    const HINSTANCE instance;
    const HWND window;

    static std::unique_ptr<NativeWindowHandle> create(Window &owner,
                                                      unsigned width,
                                                      unsigned height);

    NativeWindowHandle():
        instance(INVALID_HANDLE_VALUE),
        window(INVALID_HANDLE_VALUE),
        owner(nullptr)
    {}

    ~NativeWindowHandle();

private:
    NativeWindowHandle(HINSTANCE hinst,
                       HWND hwnd);
        instance(hinst),
        window(hwnd)
    {}
}
#endif // PLATFORM_*

