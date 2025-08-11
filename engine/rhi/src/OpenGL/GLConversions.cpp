#include "GLConversions.hpp"
#include <glad/glad.h>

namespace Aurora::RHI::GLConversions {

int toGLIndexType(IndexType type) {
    switch (type) {
        case IndexType::Uint16: return 0x1403; // GL_UNSIGNED_SHORT
        case IndexType::Uint32: return 0x1405; // GL_UNSIGNED_INT
    }
    return 0x1405;
}

int toGLTextureTarget(TextureUsage usage) {
    (void)usage;
    return 0x0DE1; // GL_TEXTURE_2D
}

int toGLTextureInternalFormat(TextureFormat fmt) {
    switch (fmt) {
        case TextureFormat::RGBA8: return 0x8058; // GL_RGBA8
        case TextureFormat::RGB8:  return 0x8051; // GL_RGB8
        case TextureFormat::R8:    return 0x8229; // GL_R8
        case TextureFormat::RGBA16F: return 0x881A; // GL_RGBA16F
        case TextureFormat::R16F: return 0x822D; // GL_R16F
        case TextureFormat::Depth24Stencil8: return 0x88F0; // GL_DEPTH24_STENCIL8
        case TextureFormat::Depth32F: return 0x8CAC; // GL_DEPTH_COMPONENT32F
    }
    return 0x8058;
}

int toGLTextureFormat(TextureFormat fmt) {
    switch (fmt) {
        case TextureFormat::RGBA8: return 0x1908; // GL_RGBA
        case TextureFormat::RGB8:  return 0x1907; // GL_RGB
        case TextureFormat::R8:    return 0x1903; // GL_RED
        case TextureFormat::RGBA16F: return 0x1908; // GL_RGBA
        case TextureFormat::R16F: return 0x1903; // GL_RED
        case TextureFormat::Depth24Stencil8: return 0x84F9; // GL_DEPTH_STENCIL
        case TextureFormat::Depth32F: return 0x1902; // GL_DEPTH_COMPONENT
    }
    return 0x1908;
}

int toGLTextureType(TextureFormat fmt) {
    switch (fmt) {
        case TextureFormat::RGBA8:
        case TextureFormat::RGB8:
        case TextureFormat::R8:
            return 0x1401; // GL_UNSIGNED_BYTE
        case TextureFormat::RGBA16F:
        case TextureFormat::R16F:
            return 0x140B; // GL_HALF_FLOAT
        case TextureFormat::Depth24Stencil8:
            return 0x84FA; // GL_UNSIGNED_INT_24_8
        case TextureFormat::Depth32F:
            return 0x1406; // GL_FLOAT
    }
    return 0x1401;
}

}


