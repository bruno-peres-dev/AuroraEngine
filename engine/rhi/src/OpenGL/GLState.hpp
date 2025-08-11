#pragma once

#include "Aurora/RHI/RHI.hpp"

namespace Aurora::RHI::GLState {

struct CachedState {
    bool depthTestEnable{false};
    bool depthWriteEnable{true};
    DepthFunc depthFunc{DepthFunc::Less};
    CullMode cullMode{CullMode::Back};
    bool frontFaceCCW{true};
    bool blendEnable{false};
    BlendFactor srcColor{BlendFactor::SrcAlpha};
    BlendFactor dstColor{BlendFactor::OneMinusSrcAlpha};
    BlendOp colorOp{BlendOp::Add};
    BlendFactor srcAlpha{BlendFactor::One};
    BlendFactor dstAlpha{BlendFactor::OneMinusSrcAlpha};
    BlendOp alphaOp{BlendOp::Add};
    uint8_t colorWriteMask{ColorWrite_All};
    bool initialized{false};
};

// Converte/enforce
int toGLBlendFactor(BlendFactor f);
int toGLBlendOp(BlendOp op);

// Aplica estado com cache (shadowing) para reduzir chamadas redundantes
void applyPipelineState(const PipelineStateDesc& state);

// Reseta cache (por troca de contexto, etc.)
void resetCache();

}


