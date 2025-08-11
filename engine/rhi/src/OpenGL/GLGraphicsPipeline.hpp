#pragma once

#include "Aurora/RHI/RHI.hpp"

namespace Aurora::RHI {

class GLGraphicsPipeline final : public IGraphicsPipeline {
public:
    explicit GLGraphicsPipeline(unsigned int program, unsigned int vao, const VertexLayoutDesc& layout, const PipelineStateDesc& state)
        : program_(program), vao_(vao), layout_(layout), state_(state) {}
    ~GLGraphicsPipeline() override;
    unsigned int program_{0};
    unsigned int vao_{0};
    VertexLayoutDesc layout_{};
    PipelineStateDesc state_{};
};

}


