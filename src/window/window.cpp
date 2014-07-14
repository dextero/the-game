#include "window.h"

#include <cstring>

#include "utils/string.h"
#include "utils/logger.h"
#include "window/native_window_handle.h"

namespace sb {

Window::Window(unsigned width, unsigned height):
    mHandle(nullptr),
    mLockCursor(false),
    mFullscreen(false),
    mRenderer(),
    mEvents()

{
    if (!create(width, height)) {
        gLog.err("cannot create window");
        return;
    }

    mRenderer.init(*mHandle);
    mRenderer.setViewport(0, 0, width, height);
}

Window::~Window()
{
    close();
}

bool Window::create(unsigned width, unsigned height)
{
    mHandle = NativeWindowHandle::create(*this, width, height);
    return !!mHandle;
}

void Window::close()
{
    mHandle = {};
}

bool Window::isOpened()
{
    return !!mHandle;
}

void Window::lockCursor(bool lock)
{
    mLockCursor = lock;
}

Renderer& Window::getRenderer()
{
    return mRenderer;
}

#if PLATFORM_LINUX

void Window::resize(unsigned width, unsigned height)
{
    (void)width;
    (void)height;

    assert(!"Window::resize not yet implemented");
}

bool Window::setFullscreen(bool fullscreen)
{
    assert(!"Window::setFullscreen not implemented!");

    mFullscreen = fullscreen;
    return true;
}

const Vec2i Window::getSize()
{
    XWindowAttributes attribs;
    XGetWindowAttributes(mHandle->display, mHandle->window, &attribs);

    return Vec2i(attribs.width, attribs.height);
}

bool Window::getEvent(Event& e)
{
    while (XPending(mHandle->display)) {
        XEvent event;
        XNextEvent(mHandle->display, &event);

        switch (event.type) {
        case KeyPress:
            {
                Key key = (Key)XLookupKeysym((XKeyEvent*)&event, 0);
                mEvents.push(Event::keyPressedEvent(key));
            }
            break;
        case KeyRelease:
            {
                Key key = (Key)XLookupKeysym((XKeyEvent*)&event, 0);
                mEvents.push(Event::keyReleasedEvent(key));
            }
            break;
        case ButtonPress:
            if (event.xbutton.button < Button4) {
                unsigned x = event.xbutton.x;
                unsigned y = event.xbutton.y;
                Mouse::Button btn = (Mouse::Button)event.xbutton.button;
                mEvents.push(Event::mousePressedEvent(x, y, btn));
            } else {
                int wheelDelta = 1;
                if ((Mouse::Button)event.xbutton.button == Mouse::Button::X1) {
                    wheelDelta = -1;
                }
                mEvents.push(Event::mouseWheelEvent(wheelDelta));
            }
            break;
        case ButtonRelease:
            {
                unsigned x = event.xbutton.x;
                unsigned y = event.xbutton.y;
                Mouse::Button btn = (Mouse::Button)event.xbutton.button;
                mEvents.push(Event::mouseReleasedEvent(x, y, btn));
            }
            break;
        case MotionNotify:
            mEvents.push(Event::mouseMovedEvent(event.xmotion.x,
                                                event.xmotion.y));

            if (mLockCursor) {
                // HACK: XWarpPointer triggers another MotionNotify event, so
                // temporarily ignore it to prevent infinite loop
                static bool ignore = false;
                if (!ignore) {
                    Vec2i wndSize = getSize();
                    XWarpPointer(mHandle->display, None, mHandle->window,
                                 0, 0, 0, 0, wndSize.x / 2, wndSize.y / 2);
                }
                ignore = !ignore;
            }
            break;
        case FocusIn:
            mEvents.push(Event::windowFocusEvent(true));
            break;
        case FocusOut:
            mEvents.push(Event::windowFocusEvent(false));
            break;
        case DestroyNotify:
            mEvents.push(Event::windowClosedEvent());
            break;
        default:
            break;
        }
    }

    if (mEvents.size() > 0) {
        e = mEvents.front();
        mEvents.pop();
        return true;
    }

    return false;
}

bool Window::hasFocus()
{
    ::Window focused;
    int focusState;
    XGetInputFocus(mHandle->display, &focused, &focusState);
    return mHandle->window == focused;
}

void Window::setTitle(const std::string& str)
{
    XStoreName(mHandle->display, mHandle->window, str.c_str());
}


void Window::clear(const Color& c)
{
    mRenderer.setClearColor(c);
    mRenderer.clear();
}

void Window::display()
{
    mRenderer.swapBuffers();
}

void Window::showCursor(bool show)
{
    if (show) {
        XDefineCursor(mHandle->display, mHandle->window, 0);
        return;
    }

    char bm[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    Pixmap pix = XCreateBitmapFromData(mHandle->display, mHandle->window,
                                       bm, 1, 1);
    XColor black;
    memset(&black, 0, sizeof(XColor));
    black.flags = DoRed | DoGreen | DoBlue;

    Cursor pointer = XCreatePixmapCursor(mHandle->display, pix, pix,
                                         &black, &black, 0, 0);
    XFreePixmap(mHandle->display, pix);

    XDefineCursor(mHandle->display, mHandle->window, pointer);
    XSync(mHandle->display, False); // again, optional
}


#elif PLATFORM_WIN32

void Window::resize(unsigned width, unsigned height)
{
    assert(mHandle && mHandle->window);

    RECT rect;
    rect.left = rect.top = 0;
    rect.right = width;
    rect.bottom = height;
    ::AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, FALSE, NULL);
    ::SetWindowPos(mHandle->window, NULL, 0, 0,
                   rect.right - rect.left, rect.bottom - rect.top,
                   SWP_NOMOVE | SWP_NOZORDER);
    mRenderer.setViewport(0, 0, width, height);
}

bool Window::setFullscreen(bool fullscreen)
{
    assert(!"Window::setFullscreen not implemented!");

    if (fullscreen == mFullscreen) {
        return true;
    }

    unsigned error = DISP_CHANGE_SUCCESSFUL;

    if (fullscreen) {
        Vec2i size = getSize();

        DEVMODE settings;
        ::EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &settings);
        settings.dmPelsWidth = size[0];
        settings.dmPelsHeight = size[1];
        settings.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;

        error = ::ChangeDisplaySettings(&settings, CDS_FULLSCREEN);
    } else {
        error = ::ChangeDisplaySettings(NULL, 0);
    }

    if (error == DISP_CHANGE_SUCCESSFUL) {
        mFullscreen = fullscreen;
        return true;
    }

    gLog.err("couldn't set fullscreen mode (0x%x)\n", error);
    return false;
}

const Vec2i Window::getSize()
{
    assert(mHandle && mHandle->window);

    RECT rect;
    ::GetClientRect(mHandle->window, &rect);

    return Vec2i(rect.right - rect.left, rect.bottom - rect.top);
}

bool Window::getEvent(Event& e)
{
    assert(mHandle && mHandle->window);

    MSG msg;
    while (PeekMessage(&msg, mHandle->window, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    size_t eventsPending = mEvents.size();
    if (eventsPending)
    {
        e = mEvents.front();
        mEvents.pop();
    }

    return eventsPending > 0;
}

bool Window::hasFocus()
{
    assert(mHandle && mHandle->window);

    return ::GetActiveWindow() == mHandle->window;
}

void Window::setTitle(const std::string& str)
{
    assert(mHandle && mHandle->window);

#ifdef UNICODE
    ::SetWindowText(mHandle->window, utils::toWString(str).c_str());
#else // !UNICODE
    ::SetWindowText(mHandle->window, str.c_str());
#endif // UNICODE
}

void Window::clear(const Color& c)
{
    mRenderer.setClearColor(c);
    mRenderer.clear();
}

void Window::draw(Drawable& d)
{
    mRenderer.draw(d);
}

void Window::display()
{
    assert(mHandle && mHandle->window);

    mRenderer.drawAll();
    ::SwapBuffers(::GetDC(mHandle->window));
}

void Window::showCursor(bool show)
{
    ::ShowCursor(show);
}

#endif // PLATFORM_*

} // namespace sb

