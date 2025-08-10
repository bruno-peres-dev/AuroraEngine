#pragma once

#ifdef _WIN32
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#endif

#include <gl/GL.h>
#include <cstddef>

#ifndef GLchar
typedef char GLchar;
#endif

// Provide APIENTRY macros if not present
#ifndef APIENTRY
#  define APIENTRY __stdcall
#endif
#ifndef APIENTRYP
#  define APIENTRYP APIENTRY *
#endif

namespace Aurora::RHI::GLLoader {

// Minimal function pointer typedefs
typedef GLuint (APIENTRYP PFNGLCREATESHADERPROC)(GLenum);
typedef void (APIENTRYP PFNGLSHADERSOURCEPROC)(GLuint, GLsizei, const GLchar* const*, const GLint*);
typedef void (APIENTRYP PFNGLCOMPILESHADERPROC)(GLuint);
typedef void (APIENTRYP PFNGLGETSHADERIVPROC)(GLuint, GLenum, GLint*);
typedef void (APIENTRYP PFNGLGETSHADERINFOLOGPROC)(GLuint, GLsizei, GLsizei*, GLchar*);
typedef void (APIENTRYP PFNGLDELETESHADERPROC)(GLuint);
typedef GLuint (APIENTRYP PFNGLCREATEPROGRAMPROC)(void);
typedef void (APIENTRYP PFNGLATTACHSHADERPROC)(GLuint, GLuint);
typedef void (APIENTRYP PFNGLLINKPROGRAMPROC)(GLuint);
typedef void (APIENTRYP PFNGLGETPROGRAMIVPROC)(GLuint, GLenum, GLint*);
typedef void (APIENTRYP PFNGLGETPROGRAMINFOLOGPROC)(GLuint, GLsizei, GLsizei*, GLchar*);
typedef void (APIENTRYP PFNGLUSEPROGRAMPROC)(GLuint);
typedef void (APIENTRYP PFNGLGENVERTEXARRAYSPROC)(GLsizei, GLuint*);
typedef void (APIENTRYP PFNGLBINDVERTEXARRAYPROC)(GLuint);
typedef void (APIENTRYP PFNGLGENBUFFERSPROC)(GLsizei, GLuint*);
typedef void (APIENTRYP PFNGLBINDBUFFERPROC)(GLenum, GLuint);
typedef void (APIENTRYP PFNGLBUFFERDATAPROC)(GLenum, ptrdiff_t, const void*, GLenum);
typedef void (APIENTRYP PFNGLVERTEXATTRIBPOINTERPROC)(GLuint, GLint, GLenum, GLboolean, GLsizei, const void*);
typedef void (APIENTRYP PFNGLENABLEVERTEXATTRIBARRAYPROC)(GLuint);
typedef GLint (APIENTRYP PFNGLGETUNIFORMLOCATIONPROC)(GLuint, const GLchar*);
typedef void (APIENTRYP PFNGLDRAWARRAYSPROC)(GLenum, GLint, GLsizei);
typedef void (APIENTRYP PFNGLBINDBUFFERBASEPROC)(GLenum, GLuint, GLuint);

typedef void (APIENTRYP PFNGLDEBUGMESSAGECALLBACKPROC)(
    void (APIENTRY *callback)(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar*, const void*),
    const void* userParam);

extern PFNGLCREATESHADERPROC                glCreateShader;
extern PFNGLSHADERSOURCEPROC                glShaderSource;
extern PFNGLCOMPILESHADERPROC               glCompileShader;
extern PFNGLGETSHADERIVPROC                 glGetShaderiv;
extern PFNGLGETSHADERINFOLOGPROC            glGetShaderInfoLog;
extern PFNGLDELETESHADERPROC                glDeleteShader;
extern PFNGLCREATEPROGRAMPROC               glCreateProgram;
extern PFNGLATTACHSHADERPROC                glAttachShader;
extern PFNGLLINKPROGRAMPROC                 glLinkProgram;
extern PFNGLGETPROGRAMIVPROC                glGetProgramiv;
extern PFNGLGETPROGRAMINFOLOGPROC           glGetProgramInfoLog;
extern PFNGLUSEPROGRAMPROC                  glUseProgram;
extern PFNGLGENVERTEXARRAYSPROC             glGenVertexArrays;
extern PFNGLBINDVERTEXARRAYPROC             glBindVertexArray;
extern PFNGLGENBUFFERSPROC                  glGenBuffers;
extern PFNGLBINDBUFFERPROC                  glBindBuffer;
extern PFNGLBUFFERDATAPROC                  glBufferData;
extern PFNGLVERTEXATTRIBPOINTERPROC         glVertexAttribPointer;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC     glEnableVertexAttribArray;
extern PFNGLGETUNIFORMLOCATIONPROC          glGetUniformLocation;
extern PFNGLDRAWARRAYSPROC                  glDrawArrays;
extern PFNGLBINDBUFFERBASEPROC              glBindBufferBase;
extern PFNGLDEBUGMESSAGECALLBACKPROC        glDebugMessageCallback;

bool load();

}


