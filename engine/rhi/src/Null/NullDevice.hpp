#pragma once

#include "Aurora/RHI/RHI.hpp"

namespace Aurora::RHI {

class NullDevice final : public IDevice {
public:
    const char* getName() const override { return "NullDevice"; }
    void beginFrame() override {}
    void endFrame() override {}
    std::unique_ptr<ISwapchain> createSwapchain(const SwapchainDesc&) override { return nullptr; }
    std::unique_ptr<IRenderPass> createRenderPass(const RenderPassDesc&) override { return nullptr; }
    void beginRenderPass(IRenderPass*, ISwapchain*) override {}
    void endRenderPass() override {}
    std::unique_ptr<IShaderModule> createShaderModule(const ShaderModuleDesc&) override { return nullptr; }
    std::unique_ptr<IBuffer> createBuffer(const void*, size_t, BufferUsage) override { return nullptr; }
    std::unique_ptr<IGraphicsPipeline> createGraphicsPipeline(const GraphicsPipelineDesc&) override { return nullptr; }
    std::unique_ptr<IDescriptorSet> createDescriptorSet(const DescriptorSetDesc&) override { return nullptr; }
    void updateBuffer(IBuffer*, const void*, size_t, size_t) override {}
    std::unique_ptr<ITexture> createTexture(const TextureDesc&, const void*) override { return nullptr; }
    std::unique_ptr<ISampler> createSampler(const SamplerDesc&) override { return nullptr; }
    void setGraphicsPipeline(IGraphicsPipeline*) override {}
    void setVertexBuffer(IBuffer*) override {}
    void setIndexBuffer(IBuffer*) override {}
    void bindDescriptorSet(IDescriptorSet*) override {}
    void draw(uint32_t, uint32_t) override {}
    void drawIndexed(uint32_t, uint32_t, IndexType) override {}
    void setDebugWireframe(bool) override {}
    std::unique_ptr<ICommandList> createCommandList() override { return nullptr; }
    void submit(ICommandList*) override {}
    Capabilities getCapabilities() const override { return {}; }
};

}


 