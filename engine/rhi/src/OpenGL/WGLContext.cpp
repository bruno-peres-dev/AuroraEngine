#include "WGLContext.hpp"
#include "Aurora/Core/Log.hpp"
#include <glad/glad.h>
#ifdef _WIN32

// Minimal constant definitions for KHR_debug and GL core enums not in gl.h
#ifndef GL_DEBUG_OUTPUT
#define GL_DEBUG_OUTPUT 0x92E0
#endif
#ifndef GL_DEBUG_SEVERITY_NOTIFICATION
#define GL_DEBUG_SEVERITY_NOTIFICATION 0x826B
#endif
#ifndef GL_DONT_CARE
#define GL_DONT_CARE 0x1100
#endif

typedef HGLRC (APIENTRYP PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC, HGLRC, const int*);
typedef BOOL (APIENTRYP PFNWGLSWAPINTERVALEXTPROC)(int);

namespace Aurora::RHI {

static void APIENTRY glDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* user) {
    (void)source; (void)type; (void)id; (void)length; (void)user;
    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) return; // ignore spam
    Core::log(Core::LogLevel::Warn, std::string("GL Debug: ") + message);
}

bool WGLContext::initialize(HWND targetWindow, bool vsync) {
    hwnd = targetWindow;
    hdc = GetDC(hwnd);
    if (!hdc) return false;

    PIXELFORMATDESCRIPTOR pfd{};
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    pfd.cStencilBits = 8;

    int pf = ChoosePixelFormat(hdc, &pfd);
    if (pf == 0) return false;
    if (!SetPixelFormat(hdc, pf, &pfd)) return false;

    // Legacy temp context
    HGLRC temp = wglCreateContext(hdc);
    if (!temp) return false;
    if (!wglMakeCurrent(hdc, temp)) return false;

    // Load context creation extension
    auto wglCreateContextAttribsARB = reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(wglGetProcAddress("wglCreateContextAttribsARB"));
    HGLRC finalCtx = nullptr;
    if (wglCreateContextAttribsARB) {
        const int attribs[] = {
            0x2091 /*WGL_CONTEXT_MAJOR_VERSION_ARB*/, 4,
            0x2092 /*WGL_CONTEXT_MINOR_VERSION_ARB*/, 5,
            0x9126 /*WGL_CONTEXT_PROFILE_MASK_ARB*/, 0x00000001 /*WGL_CONTEXT_CORE_PROFILE_BIT_ARB*/,
            0x2094 /*WGL_CONTEXT_FLAGS_ARB*/, 0x00000001 /*WGL_CONTEXT_DEBUG_BIT_ARB*/,
            0
        };
        finalCtx = wglCreateContextAttribsARB(hdc, 0, attribs);
    }

    if (finalCtx) {
        wglMakeCurrent(nullptr, nullptr);
        wglDeleteContext(temp);
        hglrc = finalCtx;
        if (!wglMakeCurrent(hdc, hglrc)) return false;
    } else {
        // Fallback: keep legacy context
        hglrc = temp;
    }

    // Load GL functions
    if (!gladLoadGL()) { Core::log(Core::LogLevel::Error, "gladLoadGL falhou"); return false; }
    // Query extensions for 420 pack

    // Setup debug callback if available
    if (glDebugMessageCallback) {
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(glDebugCallback, nullptr);
    }

    // VSync
    auto wglSwapIntervalEXT = reinterpret_cast<PFNWGLSWAPINTERVALEXTPROC>(wglGetProcAddress("wglSwapIntervalEXT"));
    if (wglSwapIntervalEXT) {
        wglSwapIntervalEXT(vsync ? 1 : 0);
    }

    return true;
}

void WGLContext::swapBuffers() {
    if (hdc) SwapBuffers(hdc);
}

void WGLContext::shutdown() {
    if (hglrc) {
        wglMakeCurrent(nullptr, nullptr);
        wglDeleteContext(hglrc);
        hglrc = nullptr;
    }
    if (hdc && hwnd) {
        ReleaseDC(hwnd, hdc);
        hdc = nullptr;
    }
}

}

#endif


