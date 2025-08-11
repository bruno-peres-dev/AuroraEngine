#include "GLGraphicsPipeline.hpp"
#include <glad/glad.h>

namespace Aurora::RHI {

GLGraphicsPipeline::~GLGraphicsPipeline() {
    if (vao_) glDeleteVertexArrays(1, &vao_);
    if (program_) glDeleteProgram(program_);
}

}


