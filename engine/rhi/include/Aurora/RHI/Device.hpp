#pragma once

#include <memory>
#include <cstdint>
#include <vector>
#include "Resources.hpp"
#include "Pipeline.hpp"
#include "Descriptors.hpp"
#include "Commands.hpp"

namespace Aurora::RHI {

enum class BackendType {
    Null,
    OpenGL,
};

struct SwapchainDesc {
    void* windowHandle{nullptr};
    uint32_t width{0};
    uint32_t height{0};
    bool vsync{true};
};

class ISwapchain {
public:
    virtual ~ISwapchain() = default;
    virtual void present() = 0;
    virtual void resize(uint32_t width, uint32_t height) = 0;
    virtual uint32_t getWidth() const = 0;
    virtual uint32_t getHeight() const = 0;
    virtual void setVsync(bool enabled) = 0;
};

struct RenderPassDesc {
    // Clear e depth
    float clearColor[4]{0.1f, 0.1f, 0.1f, 1.0f};
    bool clearColorEnabled{true};
    bool clearDepthEnabled{true};
    float clearDepth{1.0f};
    // Attachments opcionais (MVP: ignorados no OpenGL e tratamos backbuffer)
    struct Attachment {
        ITexture* texture{nullptr};
        uint32_t mipLevel{0};
    };
    std::vector<Attachment> colorAttachments;
    Attachment depthAttachment{};
};

class IRenderPass {
public:
    virtual ~IRenderPass() = default;
};

class IDevice {
public:
    virtual ~IDevice() = default;
    virtual const char* getName() const = 0;
    virtual void beginFrame() = 0;
    virtual void endFrame() = 0;
    virtual std::unique_ptr<ISwapchain> createSwapchain(const SwapchainDesc& desc) = 0;
    virtual std::unique_ptr<IRenderPass> createRenderPass(const RenderPassDesc& desc) = 0;
    virtual void beginRenderPass(IRenderPass* renderPass, ISwapchain* target) = 0;
    virtual void endRenderPass() = 0;

    // Resources
    virtual std::unique_ptr<IShaderModule> createShaderModule(const ShaderModuleDesc& desc) = 0;
    virtual std::unique_ptr<IBuffer> createBuffer(const void* data, size_t bytes, BufferUsage usage) = 0;
    virtual std::unique_ptr<IGraphicsPipeline> createGraphicsPipeline(const GraphicsPipelineDesc& desc) = 0;
    virtual std::unique_ptr<IDescriptorSet> createDescriptorSet(const DescriptorSetDesc& desc) = 0;
    virtual void updateBuffer(IBuffer* buffer, const void* data, size_t bytes, size_t dstOffset = 0) = 0;
    // Textures/samplers
    virtual std::unique_ptr<ITexture> createTexture(const TextureDesc& desc, const void* initialPixelsRGBA8) = 0;
    virtual std::unique_ptr<ISampler> createSampler(const SamplerDesc& desc) = 0;

    // Minimal draw API (immediate para compat; recomendável usar ICommandList)
    virtual void setGraphicsPipeline(IGraphicsPipeline* pipeline) = 0;
    virtual void setVertexBuffer(IBuffer* buffer) = 0;
    virtual void setIndexBuffer(IBuffer* buffer) = 0;
    virtual void bindDescriptorSet(IDescriptorSet* set) = 0;
    virtual void draw(uint32_t vertexCount, uint32_t firstVertex) = 0;
    virtual void drawIndexed(uint32_t indexCount, uint32_t firstIndex, IndexType indexType) = 0;

    // Command list
    virtual std::unique_ptr<ICommandList> createCommandList() = 0;
    virtual void submit(ICommandList* list) = 0;

    struct Capabilities {
        bool supportsGLSL420{false};
        bool hasShadingLanguage420Pack{false};
    };
    virtual Capabilities getCapabilities() const = 0;

    // Debug helpers
    virtual void setDebugWireframe(bool enable) = 0;
};

std::unique_ptr<IDevice> createDevice(BackendType type);

}


