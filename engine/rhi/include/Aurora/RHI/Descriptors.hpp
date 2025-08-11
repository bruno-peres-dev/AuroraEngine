#pragma once

#include <vector>
#include <cstdint>
#include "Resources.hpp"

namespace Aurora::RHI {

// Contrato endurecido: ou aponta por binding (recomendado), ou por nome, não ambos.
struct UniformBinding {
    uint32_t binding{0};
    IBuffer* buffer{nullptr};
    size_t offset{0};
    size_t size{0};
    const char* blockName{nullptr}; // use OU binding OU nome. Se blockName != nullptr, binding é ignorado.
};

struct DescriptorSetDesc {
    std::vector<UniformBinding> uniformBuffers;
    struct SampledTextureBinding {
        uint32_t binding{0};
        ITexture* texture{nullptr};
        ISampler* sampler{nullptr};
        const char* uniformName{nullptr}; // se fornecido, binding é ignorado
    };
    std::vector<SampledTextureBinding> sampledTextures;
};

class IDescriptorSet {
public:
    virtual ~IDescriptorSet() = default;
};

}


