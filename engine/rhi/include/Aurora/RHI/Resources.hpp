#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include "Formats.hpp"

namespace Aurora::RHI {

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

struct VertexLayoutDesc {
    uint32_t stride{0};
    std::vector<VertexAttribute> attributes;
};

// Textures e Samplers
enum class TextureUsage : uint8_t { Sampled, RenderTarget, DepthStencil, Storage };

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

struct SamplerDesc {
    FilterMode minFilter{FilterMode::Linear};
    FilterMode magFilter{FilterMode::Linear};
    AddressMode addressU{AddressMode::Repeat};
    AddressMode addressV{AddressMode::Repeat};
    enum class MipmapMode : uint8_t { None, Nearest, Linear };
    MipmapMode mipmapMode{MipmapMode::None};
};

class ISampler {
public:
    virtual ~ISampler() = default;
};

}


