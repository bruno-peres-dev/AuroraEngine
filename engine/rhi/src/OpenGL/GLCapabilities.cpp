#include "GLCapabilities.hpp"
#include <glad/glad.h>

namespace Aurora::RHI::GLCapabilities {

Caps query() {
    Caps c{};
    c.supportsGLSL420 = GLAD_GL_VERSION_4_2 != 0;
    // Se necessário, checar extensão: GL_ARB_shading_language_420pack
    // Glad expõe booleanos GLAD_GL_ARB_shading_language_420pack quando habilitado no generator.
    // Como fallback, marcar false e usar a versão como fonte de verdade.
    c.hasShadingLanguage420Pack = false;
    return c;
}

}


