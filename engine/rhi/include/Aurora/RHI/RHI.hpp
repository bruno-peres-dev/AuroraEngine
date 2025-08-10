#pragma once

#include <memory>
#include <string>
#include <cstdint>
#include <vector>

namespace Aurora::RHI {

enum class BackendType {
    Null,
    OpenGL,
    // Vulkan,
    // D3D12,
    // Metal
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
};

struct RenderPassDesc {
    float clearColor[4]{0.1f, 0.1f, 0.1f, 1.0f};
    bool clearColorEnabled{true};
    bool clearDepthEnabled{true};
    float clearDepth{1.0f};
};

class IRenderPass {
public:
    virtual ~IRenderPass() = default;
};

enum class ShaderStage : uint8_t { Vertex, Fragment };

struct ShaderModuleDesc {
    ShaderStage stage{ShaderStage::Vertex};
    const char* source{nullptr};
};

class IShaderModule {
public:
    virtual ~IShaderModule() = default;
    virtual ShaderStage getStage() const = 0;
};

enum class BufferUsage : uint8_t { Vertex, Index, Uniform };

class IBuffer {
public:
    virtual ~IBuffer() = default;
    virtual size_t getSize() const = 0;
    virtual BufferUsage getUsage() const = 0;
};

struct VertexAttribute {
    uint32_t location{0};
    uint32_t components{0};
    uint32_t offset{0};
};

struct VertexLayoutDesc {
    uint32_t stride{0};
    std::vector<VertexAttribute> attributes;
};

class IGraphicsPipeline {
public:
    virtual ~IGraphicsPipeline() = default;
};

enum class DepthFunc : uint8_t { Never, Less, Equal, LessEqual, Greater, NotEqual, GreaterEqual, Always };
enum class CullMode : uint8_t { None, Front, Back };

struct RasterState {
    bool depthTestEnable{false};
    bool depthWriteEnable{true};
    DepthFunc depthFunc{DepthFunc::Less};
    CullMode cullMode{CullMode::Back};
    bool frontFaceCCW{true};
    bool blendEnable{false};
};

struct PipelineStateDesc {
    RasterState raster{};
};

struct GraphicsPipelineDesc {
    IShaderModule* vertexShader{nullptr};
    IShaderModule* fragmentShader{nullptr};
    VertexLayoutDesc vertexLayout{};
    PipelineStateDesc state{};
};

struct UniformBinding {
    uint32_t binding{0};
    IBuffer* buffer{nullptr};
    size_t offset{0};
    size_t size{0};
    const char* blockName{nullptr};
};

struct DescriptorSetDesc {
    std::vector<UniformBinding> uniformBuffers;
};

class IDescriptorSet {
public:
    virtual ~IDescriptorSet() = default;
};

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
    virtual void drawIndexed(uint32_t indexCount, uint32_t firstIndex) = 0;
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

    // Minimal draw API (immediate for now)
    virtual void setGraphicsPipeline(IGraphicsPipeline* pipeline) = 0;
    virtual void setVertexBuffer(IBuffer* buffer) = 0;
    virtual void setIndexBuffer(IBuffer* buffer) = 0;
    virtual void bindDescriptorSet(IDescriptorSet* set) = 0;
    virtual void draw(uint32_t vertexCount, uint32_t firstVertex) = 0;
    virtual void drawIndexed(uint32_t indexCount, uint32_t firstIndex) = 0;

    // Command list
    virtual std::unique_ptr<ICommandList> createCommandList() = 0;
    virtual void submit(ICommandList* list) = 0;

    struct Capabilities {
        bool supportsGLSL420{false};
        bool hasShadingLanguage420Pack{false};
    };
    virtual Capabilities getCapabilities() const = 0;
};

std::unique_ptr<IDevice> createDevice(BackendType type);

}


