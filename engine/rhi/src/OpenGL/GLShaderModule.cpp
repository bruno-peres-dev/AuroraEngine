#include "GLShaderModule.hpp"
#include <glad/glad.h>

namespace Aurora::RHI {

GLShaderModule::~GLShaderModule() {
    if (id_) glDeleteShader(id_);
}

}


