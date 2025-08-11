#pragma once

#include "Aurora/RHI/RHI.hpp"
#include <memory>
#include <string>
#include <unordered_map>

namespace Aurora::Assets {

class AssetManager {
public:
    explicit AssetManager(RHI::IDevice& device) : device_(device) {}

    // Carrega um shader GLSL de um arquivo no disco com cache por caminho
    // Retorna um ponteiro não-proprietário. O AssetManager mantém a posse.
    RHI::IShaderModule* getOrLoadShaderFromFile(RHI::ShaderStage stage, const std::string& path);

    // Carrega textura RGBA8 do disco (pixels fornecidos externamente neste estágio)
    // Forneceremos uma sobrecarga utilitária com leitura de arquivo em implementação
    RHI::ITexture* getOrLoadTextureFromFile(const std::string& path);
    RHI::ISampler* getOrCreateSampler(const RHI::SamplerDesc& desc);

    // Placeholder para malhas/modelos (wire-up futuro)
    void* getOrLoadMeshFromFile(const std::string& path);

    // Opcional: limpar tudo (útil no shutdown controlado)
    void clear();

private:
    struct ShaderKey {
        RHI::ShaderStage stage;
        std::string path;
        bool operator==(const ShaderKey& other) const {
            return stage == other.stage && path == other.path;
        }
    };

    struct ShaderKeyHash {
        size_t operator()(const ShaderKey& k) const {
            std::hash<std::string> hs;
            std::hash<int> hi;
            return (hs(k.path) * 1315423911u) ^ hi(static_cast<int>(k.stage));
        }
    };

    RHI::IDevice& device_;
    std::unordered_map<ShaderKey, std::unique_ptr<RHI::IShaderModule>, ShaderKeyHash> shaderCache_;
    std::unordered_map<std::string, std::unique_ptr<RHI::ITexture>> textureCache_;
    struct SamplerKeyHash {
        size_t operator()(const RHI::SamplerDesc& d) const {
            return (static_cast<size_t>(d.minFilter) << 0) ^ (static_cast<size_t>(d.magFilter) << 4)
                   ^ (static_cast<size_t>(d.addressU) << 8) ^ (static_cast<size_t>(d.addressV) << 12);
        }
    };
    struct SamplerKeyEq {
        bool operator()(const RHI::SamplerDesc& a, const RHI::SamplerDesc& b) const {
            return a.minFilter==b.minFilter && a.magFilter==b.magFilter && a.addressU==b.addressU && a.addressV==b.addressV;
        }
    };
    std::unordered_map<RHI::SamplerDesc, std::unique_ptr<RHI::ISampler>, SamplerKeyHash, SamplerKeyEq> samplerCache_;
};

}


