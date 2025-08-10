#pragma once

#ifdef _WIN32
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>

namespace Aurora::RHI {

struct WGLContext {
    HDC hdc{};
    HGLRC hglrc{};
    HWND hwnd{};

    bool initialize(HWND targetWindow, bool vsync);
    void swapBuffers();
    void shutdown();
};

}

#endif


