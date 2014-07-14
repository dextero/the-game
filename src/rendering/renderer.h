#pragma once

#include <memory>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "rendering/color.h"

namespace sb {

class NativeWindowHandle;
class NativeContextHandle;

class Renderer
{
public:
    Renderer();

    Renderer(const Renderer&) = delete;
    Renderer(Renderer&&) = delete;
    Renderer& operator =(const Renderer&) = delete;
    Renderer& operator =(Renderer&&) = delete;

    bool init(const NativeWindowHandle &handle);
    void setClearColor(const Color& c);
    void clear();
    void swapBuffers();
    void setViewport(unsigned x,
                     unsigned y,
                     unsigned width,
                     unsigned height);

private:
    // HACK: semantically should be unique_ptr, but that does not work with
    // incomplete typer
    std::shared_ptr<const NativeContextHandle> mContext;

    bool initGLEW();
};

} // namespace sb

