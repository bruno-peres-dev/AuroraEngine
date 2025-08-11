#pragma once

#include "Aurora/RHI/RHI.hpp"

namespace Aurora::RHI {

class GLRenderPass final : public IRenderPass {
public:
    explicit GLRenderPass(const RenderPassDesc& desc) : desc_(desc) {}
    RenderPassDesc desc_{};
};

}


