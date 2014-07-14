#pragma once

#include <queue>
#include <string>
#include <memory>

#include "rendering/renderer.h"
#include "rendering/color.h"
#include "rendering/camera.h"
#include "window/event.h"

namespace sb {

struct NativeWindowHandle;

class Window
{
public:
    Window(unsigned width, unsigned height);
    ~Window();

    Window(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator =(const Window&) = delete;
    Window& operator =(Window&&) = delete;

    bool create(unsigned width, unsigned height);
    void resize(unsigned width, unsigned height);
    bool setFullscreen(bool fullscreen = true);
    const Vec2i getSize();
    void close();
    bool getEvent(Event& e);
    bool isOpened();
    bool hasFocus();
    void setTitle(const std::string& str);

    void clear(const Color& c);
    void draw(Drawable& d);
    void display();
    void showCursor(bool show = true);
    void lockCursor(bool lock = true);

    Renderer& getRenderer();
    Camera& getCamera();

private:
    friend class NativeWindowHandle;

    std::unique_ptr<NativeWindowHandle> mHandle;

    bool mLockCursor;
    bool mFullscreen;

    Renderer mRenderer;
    std::queue<Event> mEvents;
};

} // namespace sb

