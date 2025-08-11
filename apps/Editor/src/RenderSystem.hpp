#pragma once

#include <cstdint>
#include <memory>
#include "Aurora/RHI/RHI.hpp"

namespace Aurora::EditorAppNS {

struct ViewportResources {
    std::unique_ptr<RHI::ITexture> color;
    std::unique_ptr<RHI::ITexture> depth;
    std::unique_ptr<RHI::IRenderPass> renderPass;
    uint32_t width{0};
    uint32_t height{0};
};

class RenderSystem {
public:
    explicit RenderSystem(RHI::IDevice& device) : device_(device) {}

    void ensureViewport(ViewportResources& vp, uint32_t width, uint32_t height);

private:
    RHI::IDevice& device_;
};

}


