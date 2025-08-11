#pragma once

#include "Aurora/RHI/RHI.hpp"
#include <unordered_map>
#include <string>
#include <vector>
#include <functional>

#include "GLRenderPass.hpp"
#include "GLSwapchain.hpp"
#include "GLShaderModule.hpp"
#include "GLBuffer.hpp"
#include "GLGraphicsPipeline.hpp"
#include "GLDescriptorSet.hpp"
#include "GLTexture.hpp"
#include "GLSampler.hpp"

namespace Aurora::RHI {

class GLDevice final : public IDevice {
public:
    const char* getName() const override { return "OpenGL"; }
    void beginFrame() override {}
    void endFrame() override {}
    std::unique_ptr<ISwapchain> createSwapchain(const SwapchainDesc& desc) override;
    std::unique_ptr<IRenderPass> createRenderPass(const RenderPassDesc& desc) override {
        return std::make_unique<GLRenderPass>(desc);
    }
    void beginRenderPass(IRenderPass* renderPass, ISwapchain* target) override;
    void endRenderPass() override;

private:
    // Tipos específicos OpenGL movidos para headers dedicados

public:
    std::unique_ptr<IShaderModule> createShaderModule(const ShaderModuleDesc& desc) override;
    std::unique_ptr<IBuffer> createBuffer(const void* data, size_t bytes, BufferUsage usage) override;
    std::unique_ptr<IGraphicsPipeline> createGraphicsPipeline(const GraphicsPipelineDesc& desc) override;
    std::unique_ptr<IDescriptorSet> createDescriptorSet(const DescriptorSetDesc& desc) override;
    void updateBuffer(IBuffer* buffer, const void* data, size_t bytes, size_t dstOffset = 0) override;
    void setGraphicsPipeline(IGraphicsPipeline* pipeline) override;
    void setVertexBuffer(IBuffer* buffer) override;
    void setIndexBuffer(IBuffer* buffer) override;
    void bindDescriptorSet(IDescriptorSet* set) override;
    void draw(uint32_t vertexCount, uint32_t firstVertex) override;
    void drawIndexed(uint32_t indexCount, uint32_t firstIndex, IndexType indexType) override;
    std::unique_ptr<ICommandList> createCommandList() override;
    void submit(ICommandList* list) override;
    Capabilities getCapabilities() const override { return caps_; }

    // Textures/samplers
    std::unique_ptr<ITexture> createTexture(const TextureDesc& desc, const void* initialPixelsRGBA8) override;
    std::unique_ptr<ISampler> createSampler(const SamplerDesc& desc) override;
    void setDebugWireframe(bool enable) override;

    private:
    GLGraphicsPipeline* currentPipeline_{nullptr};
    GLBuffer* currentVertexBuffer_{nullptr};
    GLBuffer* currentIndexBuffer_{nullptr};
    Capabilities caps_{};
    // Framebuffer atual quando usando attachments (criamos e destruímos por render pass, MVP)
    unsigned int currentFBO_{0};
    bool tempFBOCreated_{false};

        // Caches simples para reduzir chamadas GL caras
        // Cache: program -> (blockName -> blockIndex)
        std::unordered_map<unsigned int, std::unordered_map<std::string, int>> programToUniformBlockIndexCache_{};
        // Cache: program -> (uniformName -> location)
        std::unordered_map<unsigned int, std::unordered_map<std::string, int>> programToUniformSamplerLocationCache_{};
        // Último binding aplicado para (program, blockIndex) => binding
        std::unordered_map<unsigned long long, unsigned int> uniformBlockBindingApplied_{};
        // Último valor aplicado para (program, uniformLocation) => sampler unit
        std::unordered_map<unsigned long long, int> samplerUniformApplied_{};

    class GLCommandList final : public ICommandList {
    public:
        explicit GLCommandList(GLDevice& dev) : device_(dev) {}
        void begin() override { operations_.clear(); recording_ = true; }
        void end() override { recording_ = false; }
        void beginRenderPass(IRenderPass* renderPass, ISwapchain* target) override {
            operations_.emplace_back([this, renderPass, target]{ device_.beginRenderPass(renderPass, target); });
        }
        void endRenderPass() override { operations_.emplace_back([this]{ device_.endRenderPass(); }); }
        void setGraphicsPipeline(IGraphicsPipeline* pipeline) override { operations_.emplace_back([this, pipeline]{ device_.setGraphicsPipeline(pipeline); }); }
        void setVertexBuffer(IBuffer* buffer) override { operations_.emplace_back([this, buffer]{ device_.setVertexBuffer(buffer); }); }
        void setIndexBuffer(IBuffer* buffer) override { operations_.emplace_back([this, buffer]{ device_.setIndexBuffer(buffer); }); }
        void bindDescriptorSet(IDescriptorSet* set) override { operations_.emplace_back([this, set]{ device_.bindDescriptorSet(set); }); }
        void draw(uint32_t vertexCount, uint32_t firstVertex) override { operations_.emplace_back([this, vertexCount, firstVertex]{ device_.draw(vertexCount, firstVertex); }); }
        void drawIndexed(uint32_t indexCount, uint32_t firstIndex, IndexType indexType) override { operations_.emplace_back([this, indexCount, firstIndex, indexType]{ device_.drawIndexed(indexCount, firstIndex, indexType); }); }
        void setDebugWireframe(bool enable) override { operations_.emplace_back([this, enable]{ device_.setDebugWireframe(enable); }); }
    private:
        GLDevice& device_;
        std::vector<std::function<void()>> operations_{};
        bool recording_{false};

        friend class GLDevice;
    };
};

}


