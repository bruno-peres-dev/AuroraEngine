#include "GLBuffer.hpp"
#include <glad/glad.h>

namespace Aurora::RHI {

GLBuffer::~GLBuffer() {
    if (id_) glDeleteBuffers(1, &id_);
}

}


