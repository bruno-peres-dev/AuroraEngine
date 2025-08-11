#include "GLState.hpp"
#include <glad/glad.h>

namespace Aurora::RHI::GLState {

static CachedState g_cached{};

int toGLBlendFactor(BlendFactor f) {
    switch (f) {
        case BlendFactor::Zero: return 0; // GL_ZERO
        case BlendFactor::One: return 1; // GL_ONE
        case BlendFactor::SrcColor: return 0x0300; // GL_SRC_COLOR
        case BlendFactor::OneMinusSrcColor: return 0x0301; // GL_ONE_MINUS_SRC_COLOR
        case BlendFactor::DstColor: return 0x0306; // GL_DST_COLOR
        case BlendFactor::OneMinusDstColor: return 0x0307; // GL_ONE_MINUS_DST_COLOR
        case BlendFactor::SrcAlpha: return 0x0302; // GL_SRC_ALPHA
        case BlendFactor::OneMinusSrcAlpha: return 0x0303; // GL_ONE_MINUS_SRC_ALPHA
        case BlendFactor::DstAlpha: return 0x0304; // GL_DST_ALPHA
        case BlendFactor::OneMinusDstAlpha: return 0x0305; // GL_ONE_MINUS_DST_ALPHA
    }
    return 1;
}

int toGLBlendOp(BlendOp op) {
    switch (op) {
        case BlendOp::Add: return 0x8006; // GL_FUNC_ADD
        case BlendOp::Subtract: return 0x800A; // GL_FUNC_SUBTRACT
        case BlendOp::ReverseSubtract: return 0x800B; // GL_FUNC_REVERSE_SUBTRACT
        case BlendOp::Min: return 0x8007; // GL_MIN
        case BlendOp::Max: return 0x8008; // GL_MAX
    }
    return 0x8006;
}

void applyPipelineState(const PipelineStateDesc& state) {
    const auto& rs = state.raster;
    const auto& ds = state.depthStencil;
    const auto& bs = state.blend;

    // Depth
    if (!g_cached.initialized || g_cached.depthTestEnable != ds.depthTestEnable) {
        if (ds.depthTestEnable) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
        g_cached.depthTestEnable = ds.depthTestEnable;
    }
    if (!g_cached.initialized || g_cached.depthWriteEnable != ds.depthWriteEnable) {
        glDepthMask(ds.depthWriteEnable ? GL_TRUE : GL_FALSE);
        g_cached.depthWriteEnable = ds.depthWriteEnable;
    }
    if (!g_cached.initialized || g_cached.depthFunc != ds.depthFunc) {
        switch (ds.depthFunc) {
            case DepthFunc::Less: glDepthFunc(GL_LESS); break;
            case DepthFunc::LessEqual: glDepthFunc(GL_LEQUAL); break;
            case DepthFunc::Greater: glDepthFunc(GL_GREATER); break;
            case DepthFunc::GreaterEqual: glDepthFunc(GL_GEQUAL); break;
            case DepthFunc::Equal: glDepthFunc(GL_EQUAL); break;
            case DepthFunc::NotEqual: glDepthFunc(GL_NOTEQUAL); break;
            case DepthFunc::Always: glDepthFunc(GL_ALWAYS); break;
            case DepthFunc::Never: default: glDepthFunc(GL_NEVER); break;
        }
        g_cached.depthFunc = ds.depthFunc;
    }

    // Raster
    if (!g_cached.initialized || g_cached.cullMode != rs.cullMode) {
        if (rs.cullMode == CullMode::None) glDisable(GL_CULL_FACE); else glEnable(GL_CULL_FACE);
        if (rs.cullMode == CullMode::Back) glCullFace(GL_BACK); else if (rs.cullMode == CullMode::Front) glCullFace(GL_FRONT);
        g_cached.cullMode = rs.cullMode;
    }
    if (!g_cached.initialized || g_cached.frontFaceCCW != rs.frontFaceCCW) {
        glFrontFace(rs.frontFaceCCW ? GL_CCW : GL_CW);
        g_cached.frontFaceCCW = rs.frontFaceCCW;
    }

    // Blend
    if (!g_cached.initialized || g_cached.blendEnable != bs.enable) {
        if (bs.enable) glEnable(GL_BLEND); else glDisable(GL_BLEND);
        g_cached.blendEnable = bs.enable;
    }
    if (bs.enable) {
        if (!g_cached.initialized || g_cached.srcColor != bs.srcColor || g_cached.dstColor != bs.dstColor ||
            g_cached.srcAlpha != bs.srcAlpha || g_cached.dstAlpha != bs.dstAlpha) {
            glBlendFuncSeparate(
                toGLBlendFactor(bs.srcColor), toGLBlendFactor(bs.dstColor),
                toGLBlendFactor(bs.srcAlpha), toGLBlendFactor(bs.dstAlpha));
            g_cached.srcColor = bs.srcColor; g_cached.dstColor = bs.dstColor;
            g_cached.srcAlpha = bs.srcAlpha; g_cached.dstAlpha = bs.dstAlpha;
        }
        if (!g_cached.initialized || g_cached.colorOp != bs.colorOp || g_cached.alphaOp != bs.alphaOp) {
            glBlendEquationSeparate(toGLBlendOp(bs.colorOp), toGLBlendOp(bs.alphaOp));
            g_cached.colorOp = bs.colorOp; g_cached.alphaOp = bs.alphaOp;
        }
        if (!g_cached.initialized || g_cached.colorWriteMask != bs.colorWriteMask) {
            GLboolean r = (bs.colorWriteMask & ColorWrite_R) ? GL_TRUE : GL_FALSE;
            GLboolean g = (bs.colorWriteMask & ColorWrite_G) ? GL_TRUE : GL_FALSE;
            GLboolean b = (bs.colorWriteMask & ColorWrite_B) ? GL_TRUE : GL_FALSE;
            GLboolean a = (bs.colorWriteMask & ColorWrite_A) ? GL_TRUE : GL_FALSE;
            glColorMask(r, g, b, a);
            g_cached.colorWriteMask = bs.colorWriteMask;
        }
    } else if (!g_cached.initialized) {
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        g_cached.colorWriteMask = ColorWrite_All;
    }

    g_cached.initialized = true;
}

void resetCache() {
    g_cached = CachedState{};
}

}


