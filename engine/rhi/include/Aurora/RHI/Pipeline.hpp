#pragma once

#include <cstdint>
#include "Formats.hpp"
#include "Resources.hpp"

namespace Aurora::RHI {

class IGraphicsPipeline {
public:
    virtual ~IGraphicsPipeline() = default;
};

enum class DepthFunc : uint8_t { Never, Less, Equal, LessEqual, Greater, NotEqual, GreaterEqual, Always };
enum class CullMode : uint8_t { None, Front, Back };

enum class BlendFactor : uint8_t {
    Zero, One,
    SrcColor, OneMinusSrcColor,
    DstColor, OneMinusDstColor,
    SrcAlpha, OneMinusSrcAlpha,
    DstAlpha, OneMinusDstAlpha
};

enum class BlendOp : uint8_t { Add, Subtract, ReverseSubtract, Min, Max };

struct RasterState {
    CullMode cullMode{CullMode::Back};
    bool frontFaceCCW{true};
};

struct BlendState {
    bool enable{false};
    BlendFactor srcColor{BlendFactor::SrcAlpha};
    BlendFactor dstColor{BlendFactor::OneMinusSrcAlpha};
    BlendOp colorOp{BlendOp::Add};
    BlendFactor srcAlpha{BlendFactor::One};
    BlendFactor dstAlpha{BlendFactor::OneMinusSrcAlpha};
    BlendOp alphaOp{BlendOp::Add};
    uint8_t colorWriteMask{ColorWrite_All};
};

struct DepthStencilState {
    bool depthTestEnable{false};
    bool depthWriteEnable{true};
    DepthFunc depthFunc{DepthFunc::Less};
    bool stencilEnable{false};
    uint8_t stencilReadMask{0xFF};
    uint8_t stencilWriteMask{0xFF};
};

struct PipelineStateDesc {
    RasterState raster{};
    BlendState blend{};
    DepthStencilState depthStencil{};
};

struct GraphicsPipelineDesc {
    IShaderModule* vertexShader{nullptr};
    IShaderModule* fragmentShader{nullptr};
    VertexLayoutDesc vertexLayout{};
    PipelineStateDesc state{};
};

}


