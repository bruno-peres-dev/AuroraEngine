#pragma once

// RHI agregado: reexporta headers fatiados
#include "Formats.hpp"
#include "Resources.hpp"
#include "Pipeline.hpp"
#include "Descriptors.hpp"
#include "Commands.hpp"
#include "Device.hpp"

// Desabilita o conteúdo monolítico legado abaixo
#if 0

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
    virtual void setVsync(bool enabled) = 0;
};

// Contrato: RenderPass apenas carrega clear e viewport no backbuffer do swapchain (sem attachments custom)
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
enum class IndexType : uint8_t { Uint16, Uint32 };

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

// Contrato: layout descreve stride e atributos por location; offset em bytes no VBO
struct VertexLayoutDesc {
    uint32_t stride{0};
    std::vector<VertexAttribute> attributes;
};

class IGraphicsPipeline {
public:
    virtual ~IGraphicsPipeline() = default;
};

// Textures e Samplers
enum class TextureFormat : uint8_t { RGBA8, RGB8, R8 };
enum class TextureUsage : uint8_t { Sampled };

// Contrato: formatos hoje só suportam 8-bit por canal; mipmaps gerados no upload se mipLevels > 1
struct TextureDesc {
    uint32_t width{0};
    uint32_t height{0};
    TextureFormat format{TextureFormat::RGBA8};
    TextureUsage usage{TextureUsage::Sampled};
    uint32_t mipLevels{1};
};

class ITexture {
public:
    virtual ~ITexture() = default;
    virtual TextureDesc getDesc() const = 0;
};

enum class FilterMode : uint8_t { Nearest, Linear };
enum class AddressMode : uint8_t { Repeat, ClampToEdge };

// Contrato: sampler controla filtros e endereçamento; mipmapMode ajusta o minFilter quando mipLevels > 1
struct SamplerDesc {
    FilterMode minFilter{FilterMode::Linear};
    FilterMode magFilter{FilterMode::Linear};
    AddressMode addressU{AddressMode::Repeat};
    AddressMode addressV{AddressMode::Repeat};
    // Modo de mipmap para compor com o minFilter: None usa minFilter puro
    enum class MipmapMode : uint8_t { None, Nearest, Linear };
    MipmapMode mipmapMode{MipmapMode::None};
};

class ISampler {
public:
    virtual ~ISampler() = default;
};

enum class DepthFunc : uint8_t { Never, Less, Equal, LessEqual, Greater, NotEqual, GreaterEqual, Always };
enum class CullMode : uint8_t { None, Front, Back };

// Blending e máscaras
enum class BlendFactor : uint8_t {
    Zero, One,
    SrcColor, OneMinusSrcColor,
    DstColor, OneMinusDstColor,
    SrcAlpha, OneMinusSrcAlpha,
    DstAlpha, OneMinusDstAlpha
};

enum class BlendOp : uint8_t { Add, Subtract, ReverseSubtract, Min, Max };

enum ColorWriteMask : uint8_t {
    ColorWrite_R = 1 << 0,
    ColorWrite_G = 1 << 1,
    ColorWrite_B = 1 << 2,
    ColorWrite_A = 1 << 3,
    ColorWrite_All = ColorWrite_R | ColorWrite_G | ColorWrite_B | ColorWrite_A
};

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
    // Stencil (placeholder)
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
    // Futuro: parâmetros para blending e depth/stencil por attachment
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
    struct SampledTextureBinding {
        uint32_t binding{0};
        ITexture* texture{nullptr};
        ISampler* sampler{nullptr};
        const char* uniformName{nullptr}; // se fornecido, resolve pelo nome
    };
    std::vector<SampledTextureBinding> sampledTextures;
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
    virtual void drawIndexed(uint32_t indexCount, uint32_t firstIndex, IndexType indexType) = 0;
    // Debug helpers
    virtual void setDebugWireframe(bool enable) = 0;
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

    // Minimal draw API (immediate for now)
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

#endif // end legacy block