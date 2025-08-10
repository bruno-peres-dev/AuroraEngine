#include "GLLoader.hpp"
#include "Aurora/Core/Log.hpp"

namespace Aurora::RHI::GLLoader {

PFNGLCREATESHADERPROC                glCreateShader = nullptr;
PFNGLSHADERSOURCEPROC                glShaderSource = nullptr;
PFNGLCOMPILESHADERPROC               glCompileShader = nullptr;
PFNGLGETSHADERIVPROC                 glGetShaderiv = nullptr;
PFNGLGETSHADERINFOLOGPROC            glGetShaderInfoLog = nullptr;
PFNGLDELETESHADERPROC                glDeleteShader = nullptr;
PFNGLCREATEPROGRAMPROC               glCreateProgram = nullptr;
PFNGLATTACHSHADERPROC                glAttachShader = nullptr;
PFNGLLINKPROGRAMPROC                 glLinkProgram = nullptr;
PFNGLGETPROGRAMIVPROC                glGetProgramiv = nullptr;
PFNGLGETPROGRAMINFOLOGPROC           glGetProgramInfoLog = nullptr;
PFNGLUSEPROGRAMPROC                  glUseProgram = nullptr;
PFNGLGENVERTEXARRAYSPROC             glGenVertexArrays = nullptr;
PFNGLBINDVERTEXARRAYPROC             glBindVertexArray = nullptr;
PFNGLGENBUFFERSPROC                  glGenBuffers = nullptr;
PFNGLBINDBUFFERPROC                  glBindBuffer = nullptr;
PFNGLBUFFERDATAPROC                  glBufferData = nullptr;
PFNGLVERTEXATTRIBPOINTERPROC         glVertexAttribPointer = nullptr;
PFNGLENABLEVERTEXATTRIBARRAYPROC     glEnableVertexAttribArray = nullptr;
PFNGLGETUNIFORMLOCATIONPROC          glGetUniformLocation = nullptr;
PFNGLDRAWARRAYSPROC                  glDrawArrays = nullptr;
PFNGLBINDBUFFERBASEPROC              glBindBufferBase = nullptr;
PFNGLDEBUGMESSAGECALLBACKPROC        glDebugMessageCallback = nullptr;

static void* loadProc(const char* name) {
#ifdef _WIN32
    void* p = reinterpret_cast<void*>(wglGetProcAddress(name));
    if (!p) {
        HMODULE module = GetModuleHandleA("opengl32.dll");
        p = reinterpret_cast<void*>(GetProcAddress(module, name));
    }
    return p;
#else
    (void)name; return nullptr;
#endif
}

bool load() {
    glCreateShader = reinterpret_cast<PFNGLCREATESHADERPROC>(loadProc("glCreateShader"));
    glShaderSource = reinterpret_cast<PFNGLSHADERSOURCEPROC>(loadProc("glShaderSource"));
    glCompileShader = reinterpret_cast<PFNGLCOMPILESHADERPROC>(loadProc("glCompileShader"));
    glGetShaderiv = reinterpret_cast<PFNGLGETSHADERIVPROC>(loadProc("glGetShaderiv"));
    glGetShaderInfoLog = reinterpret_cast<PFNGLGETSHADERINFOLOGPROC>(loadProc("glGetShaderInfoLog"));
    glDeleteShader = reinterpret_cast<PFNGLDELETESHADERPROC>(loadProc("glDeleteShader"));
    glCreateProgram = reinterpret_cast<PFNGLCREATEPROGRAMPROC>(loadProc("glCreateProgram"));
    glAttachShader = reinterpret_cast<PFNGLATTACHSHADERPROC>(loadProc("glAttachShader"));
    glLinkProgram = reinterpret_cast<PFNGLLINKPROGRAMPROC>(loadProc("glLinkProgram"));
    glGetProgramiv = reinterpret_cast<PFNGLGETPROGRAMIVPROC>(loadProc("glGetProgramiv"));
    glGetProgramInfoLog = reinterpret_cast<PFNGLGETPROGRAMINFOLOGPROC>(loadProc("glGetProgramInfoLog"));
    glUseProgram = reinterpret_cast<PFNGLUSEPROGRAMPROC>(loadProc("glUseProgram"));
    glGenVertexArrays = reinterpret_cast<PFNGLGENVERTEXARRAYSPROC>(loadProc("glGenVertexArrays"));
    glBindVertexArray = reinterpret_cast<PFNGLBINDVERTEXARRAYPROC>(loadProc("glBindVertexArray"));
    glGenBuffers = reinterpret_cast<PFNGLGENBUFFERSPROC>(loadProc("glGenBuffers"));
    glBindBuffer = reinterpret_cast<PFNGLBINDBUFFERPROC>(loadProc("glBindBuffer"));
    glBufferData = reinterpret_cast<PFNGLBUFFERDATAPROC>(loadProc("glBufferData"));
    glVertexAttribPointer = reinterpret_cast<PFNGLVERTEXATTRIBPOINTERPROC>(loadProc("glVertexAttribPointer"));
    glEnableVertexAttribArray = reinterpret_cast<PFNGLENABLEVERTEXATTRIBARRAYPROC>(loadProc("glEnableVertexAttribArray"));
    glGetUniformLocation = reinterpret_cast<PFNGLGETUNIFORMLOCATIONPROC>(loadProc("glGetUniformLocation"));
    glDrawArrays = reinterpret_cast<PFNGLDRAWARRAYSPROC>(loadProc("glDrawArrays"));
    glBindBufferBase = reinterpret_cast<PFNGLBINDBUFFERBASEPROC>(loadProc("glBindBufferBase"));
    glDebugMessageCallback = reinterpret_cast<PFNGLDEBUGMESSAGECALLBACKPROC>(loadProc("glDebugMessageCallback"));

    return glCreateShader && glShaderSource && glCompileShader && glGetShaderiv && glGetShaderInfoLog &&
           glDeleteShader && glCreateProgram && glAttachShader && glLinkProgram && glGetProgramiv &&
           glGetProgramInfoLog && glUseProgram && glGenVertexArrays && glBindVertexArray && glGenBuffers &&
           glBindBuffer && glBufferData && glVertexAttribPointer && glEnableVertexAttribArray && glDrawArrays;
}

}


