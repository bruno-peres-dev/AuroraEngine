#pragma once

namespace Aurora::RHI::GLCapabilities {

struct Caps {
    bool supportsGLSL420{false};
    bool hasShadingLanguage420Pack{false};
};

// Preenche capacidades usando o contexto GL atual (glad já carregado)
Caps query();

}


