#include "window/native_window_handle.h"

#include <cassert>

#include "utils/logger.h"

#if PLATFORM_LINUX
#include <X11/Xlib.h>

namespace sb {
namespace {

GLXFBConfig chooseBestFBConfig(::Display* dpy,
                               const GLXFBConfig* configs,
                               unsigned numConfigs)
{
    assert(numConfigs && "no configs to choose from");

    int bestFbcId = 0;
    int bestNumSamp = 0;

    for (int i = 0; i < numConfigs; ++i) {
        XVisualInfo* vi = glXGetVisualFromFBConfig(dpy, configs[i]);

        if (vi) {
            int sampBuf, samples;
            glXGetFBConfigAttrib(dpy, configs[i], GLX_SAMPLE_BUFFERS, &sampBuf);
            glXGetFBConfigAttrib(dpy, configs[i], GLX_SAMPLES, &samples);

            gLog.trace("matching fbconfig %d, visual id 0x%2x: "
                       "SAMPLE_BUFFERS = %d, SAMPLES = %d\n",
                       i, vi->visualid, sampBuf, samples);

            if (sampBuf && samples > bestNumSamp) {
                bestFbcId = i;
                bestNumSamp = samples;
            }
        }

        XFree(vi);
    }

    return configs[bestFbcId];
}

GLXFBConfig getBestFBConfig(::Display* dpy)
{
    static const int VISUAL_ATTRIBS[] = {
        GLX_X_RENDERABLE,  True,
        GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
        GLX_RENDER_TYPE,   GLX_RGBA_BIT,
        GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
        GLX_RED_SIZE,      8,
        GLX_GREEN_SIZE,    8,
        GLX_BLUE_SIZE,     8,
        GLX_ALPHA_SIZE,    8,
        GLX_DEPTH_SIZE,    24,
        GLX_STENCIL_SIZE,  8,
        GLX_DOUBLEBUFFER,  True,
        None
    };

    gLog.trace("getting framebuffer config\n");

    int fbCount;
    GLXFBConfig* fbc = glXChooseFBConfig(dpy, DefaultScreen(dpy),
                                         VISUAL_ATTRIBS, &fbCount);

    assert(fbc && fbCount && "no available framebuffer configs");

    GLXFBConfig ret = chooseBestFBConfig(dpy, fbc, (unsigned)fbCount);

    XFree(fbc);
    return ret;
}

} // namespace

std::unique_ptr<NativeWindowHandle> NativeWindowHandle::create(sb::Window &owner,
                                                               unsigned width,
                                                               unsigned height)
{
    ::Display* dpy = XOpenDisplay(0);
    if (dpy == nullptr) {
        return {};
    }

    GLXFBConfig bestFbc = getBestFBConfig(dpy);
    XVisualInfo* vi = glXGetVisualFromFBConfig(dpy, bestFbc);
    gLog.trace("chosen visual id = 0x%x\n", vi->visualid);

    ::Window rootWnd = RootWindow(dpy, vi->screen);

    XSetWindowAttributes swa;
    swa.colormap = XCreateColormap(dpy, rootWnd, vi->visual, AllocNone);
    swa.background_pixmap = None;
    swa.border_pixel = 0;
    swa.event_mask = StructureNotifyMask
                     | KeyPressMask | KeyReleaseMask
                     | ButtonPressMask | ButtonReleaseMask | PointerMotionMask;

    gLog.trace("creating window\n");
    ::Window wnd = XCreateWindow(dpy, rootWnd, 0, 0, width, height,
                                 0, vi->depth, InputOutput, vi->visual,
                                 CWBorderPixel | CWColormap | CWEventMask,
                                 &swa);
    XFree(vi);

    if (!wnd) {
        gLog.err("failed to create window\n");
        return {};
    }

    XStoreName(dpy, wnd, "Window");

    gLog.trace("mapping window\n");
    XMapWindow(dpy, wnd);

    auto ret = new NativeWindowHandle(&owner, dpy, wnd, bestFbc);
    return std::unique_ptr<NativeWindowHandle>(ret);
}

NativeWindowHandle::~NativeWindowHandle()
{
    if (display && window) {
        XDestroyWindow(display, window);
    }
}

} // namespace sb

#elif PLATFORM_WIN32
# define WIN32_LEAN_AND_MEAN
# include <windows.h>

namespace sb {
namespace {

Key translateKey(WPARAM keyCode)
{
    if (keyCode != VK_SHIFT) {
        return (Key)keyCode;
    }

    // it seems that windows assumes no one really cares about *which*
    // shift was actually presed - well, we do
    bool wasItLeftShift = !!(::GetKeyState(Key::LShift) >> 4);
    return wasItLeftShift ? Key::LShift : Key::RShift;
}

LRESULT __stdcall wndProc(HWND hwnd, UINT msg, WPARAM w, LPARAM l)
{
    Window* wndPtr = (Window*)::GetWindowLongPtrA(hwnd, 0);

    // these messages do not need wndPtr to be set
    switch (msg) {
        case WM_CREATE:
            ::SetWindowLongPtrA(hwnd, 0,
                                (LONG)((CREATESTRUCT*)l)->lpCreateParams);
            break;
        case WM_DESTROY:
            ::PostQuitMessage(0);
            break;
        default:
            break;
    }

    if (!wndPtr) {
        gLog.warn("got message %x without Window object set", msg);
        return ::DefWindowProcA(hwnd, msg, w, l);
    }

    switch (msg) {
    case WM_KEYDOWN:
        wndPtr->mEvents.push(Event::keyPressedEvent(translateKey(w));
        break;
    case WM_KEYUP:
        wndPtr->mEvents.push(Event::keyReleasedEvent(translateKey(w)));
        break;
    case WM_LBUTTONDOWN:
        wndPtr->mEvents.push(Event::mousePressedEvent(LOWORD(l), HIWORD(l),
                                                      Mouse::Button::Left));
        break;
    case WM_LBUTTONUP:
        wndPtr->mEvents.push(Event::mouseReleasedEvent(LOWORD(l), HIWORD(l),
                                                       Mouse::Button::Left));
        break;
    case WM_RBUTTONDOWN:
        wndPtr->mEvents.push(Event::mousePressedEvent(LOWORD(l), HIWORD(l),
                                                      Mouse::Button::Right));
        break;
    case WM_RBUTTONUP:
        wndPtr->mEvents.push(Event::mouseReleasedEvent(LOWORD(l), HIWORD(l),
                                                       Mouse::Button::Right));
        break;
    case WM_MBUTTONDOWN:
        wndPtr->mEvents.push(Event::mousePressedEvent(LOWORD(l), HIWORD(l),
                                                      Mouse::Button::Middle));
        break;
    case WM_MBUTTONUP:
        wndPtr->mEvents.push(Event::mouseReleasedEvent(LOWORD(l), HIWORD(l),
                                                       Mouse::Button::Middle));
        break;
    case WM_XBUTTONDOWN:
        if (w & 0x60) {
            Mouse::Button btn = (w & 0x20) ? Mouse::Button::X1
                                           : Mouse::Button::X2;
            wndPtr->mEvents.push(
                    Event::mousePressedEvent(LOWORD(l), HIWORD(l), btn));
        }
        break;
    case WM_XBUTTONUP:
        if (w & 0x60) {
            Mouse::Button btn = (w & 0x20) ? Mouse::Button::X1
                                           : Mouse::Button::X2;
            wndPtr->mEvents.push(
                    Event::mouseReleasedEvent(LOWORD(l), HIWORD(l), ));
        }
        break;
    case WM_MOUSEWHEEL:
        if (wndPtr) {
            wndPtr->mEvents.push(
                    Event::mouseWheelEvent(LOWORD(l), HIWORD(l),
                                           HIWORD(w) * WHEEL_DELTA));
        }
        break;
    case WM_MOUSEMOVE:
        {
            // HACK: SetCursorPos triggers another WM_MOUSEMOVE event, so
            // ignore it to prevent infinite loop
            static bool ignore = false;
            if (ignore) {
                ignore = false;
            } else {
                wndPtr->mEvents.push(Event::mouseMovedEvent(LOWORD(l),
                                                            HIWORD(l)));
                if (wndPtr->mLockCursor) {
                    RECT rect;
                    POINT center;
                    ::GetClientRect(wndPtr->mWnd, &rect);
                    center.x = (rect.left + rect.right) / 2;
                    center.y = (rect.top + rect.bottom) / 2;
                    ::ClientToScreen(wndPtr->mWnd, &center);
                    ::SetCursorPos(center.x, center.y);
                    ignore = true;
                }
            }
            break;
        }
    case WM_ACTIVATEAPP:
        wndPtr->mEvents.push(Event::windowFocusEvent(!!w));
        break;
    case WM_SIZE:
        if (w == SIZE_RESTORED || w == SIZE_MAXIMIZED) {
            Vec2i size = wndPtr->getSize();
            wndPtr->mEvents.push(Event::windowResizedEvent(size[0], size[1]));
            wndPtr->mRenderer.setViewport(0, 0, size[0], size[1]);
        }
        break;
    case WM_CLOSE:
        wndPtr->mEvents.push(Event::windowClosedEvent());
        wndPtr->mHandle = {};
        break;
    default:
        return ::DefWindowProcA(hwnd, msg, w, l);
    }

    return 0;
}

LPCTSTR getWindowClass(HINSTANCE instance)
{
    static LPCTSTR CLASS_NAME = _T("ILikeTrains8@3");
    static bool classRegistered = false;

    if (classRegistered) {
        return CLASS_NAME;
    }

    WNDCLASSEX wcex;
    memset(&wcex, 0, sizeof(wcex));

    wcex.cbSize = sizeof(wcex);
    wcex.hInstance = instance;
    wcex.lpfnWndProc = wndProc;
    wcex.lpszClassName = CLASS_NAME;
    wcex.cbWndExtra = 8;
    wcex.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;

    classRegistered = !!RegisterClassEx(&wcex);

    if (classRegistered) {
        return CLASS_NAME;
    }
    return nullptr;
}

bool setupPixelFormat(HWND hwnd)
{
    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA,
        32,
        0, 0, 0, 0, 0, 0,
        0, 0, 0,
        0, 0, 0, 0,
        16,
        0, 0,
        PFD_MAIN_PLANE,
        0, 0, 0, 0
    };

    HDC dc = ::GetDC(hwnd);
    int pixelFormat = ::ChoosePixelFormat(dc, &pfd);

    return pixelFormat != 0
            && ::SetPixelFormat(dc, pixelFormat, &pfd);
}

} // namespace

unique_ptr<NativeWindowHandle> create(Window &owner,
                                      unsigned width,
                                      unsigned height)
{
    HINSTANCE instance = ::GetModuleHandleA(0);
    LPCTSTR className = getWindowClass(instance);

    if (!className) {
        return {};
    }

    RECT rect;
    rect.left = rect.top = 0;
    rect.right = width;
    rect.bottom = height;
    ::AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, FALSE, NULL);

    HWND hwnd = ::CreateWindowEx(NULL, className, _T("Window"),
                                 WS_OVERLAPPEDWINDOW,
                                 CW_USEDEFAULT, CW_USEDEFAULT,
                                 rect.right - rect.left, rect.bottom - rect.top,
                                 NULL, NULL, instance, (void*)&windowObject);

    if (!setupPixelFormat(hwnd)) {
        return {};
    }

    ::ShowWindow(hwnd, SW_SHOW);
    ::UpdateWindow(hwnd);

    return new NativeWindowHandle(owner, instance, hwnd);
}

NativeWindowHandle::~NativeWindowHandle()
{
    if (window != INVALID_HANDLE_VALUE) {
        ::DestroyWindow(window);
    }
}

} // namespace sb

#endif // PLATFORM_*

