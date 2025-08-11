#include "GLTexture.hpp"
#include <glad/glad.h>

namespace Aurora::RHI {

GLTexture::~GLTexture() {
    if (id_) glDeleteTextures(1, &id_);
}

}


