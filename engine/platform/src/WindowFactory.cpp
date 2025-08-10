#include "Aurora/Platform/Window.hpp"

#ifdef _WIN32
#  include "Windows/Win32Window.hpp"
#endif

namespace Aurora::Platform {

IWindow* createWindow(const WindowDesc& desc) {
#ifdef _WIN32
    return new Win32Window(desc);
#else
    (void)desc;
    return nullptr;
#endif
}

void destroyWindow(IWindow* window) {
    delete window;
}

}


