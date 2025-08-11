#include "GLSampler.hpp"
#include <glad/glad.h>

namespace Aurora::RHI {

GLSampler::~GLSampler() {
    if (id_) glDeleteSamplers(1, &id_);
}

}


