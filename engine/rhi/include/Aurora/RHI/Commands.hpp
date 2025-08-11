#pragma once

#include <memory>
#include "Resources.hpp"
#include "Pipeline.hpp"
#include "Descriptors.hpp"

namespace Aurora::RHI {

class ISwapchain; // fwd
class IRenderPass; // fwd

class ICommandList {
public:
    virtual ~ICommandList() = default;
    virtual void begin() = 0;
    virtual void end() = 0;
    virtual void beginRenderPass(IRenderPass* renderPass, ISwapchain* target) = 0;
    virtual void endRenderPass() = 0;
    virtual void setGraphicsPipeline(IGraphicsPipeline* pipeline) = 0;
    virtual void setVertexBuffer(IBuffer* buffer) = 0;
    virtual void setIndexBuffer(IBuffer* buffer) = 0;
    virtual void bindDescriptorSet(IDescriptorSet* set) = 0;
    virtual void draw(uint32_t vertexCount, uint32_t firstVertex) = 0;
    virtual void drawIndexed(uint32_t indexCount, uint32_t firstIndex, IndexType indexType) = 0;
    // Debug helpers
    virtual void setDebugWireframe(bool enable) = 0;
};

}


