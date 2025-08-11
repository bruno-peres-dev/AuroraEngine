#pragma once

#include "Aurora/RHI/RHI.hpp"

namespace Aurora::RHI::GLConversions {

int toGLIndexType(IndexType type);
int toGLTextureTarget(TextureUsage usage);
int toGLTextureInternalFormat(TextureFormat fmt);
int toGLTextureFormat(TextureFormat fmt);
int toGLTextureType(TextureFormat fmt);

}


