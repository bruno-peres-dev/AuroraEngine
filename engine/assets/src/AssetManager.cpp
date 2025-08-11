#include "Aurora/Assets/AssetManager.hpp"
#include "Aurora/Core/Log.hpp"

#include <fstream>
#include <sstream>
#include <vector>

namespace Aurora::Assets {

static std::string loadTextFile(const std::string& path) {
    std::ifstream ifs(path, std::ios::in);
    if (!ifs) return {};
    std::ostringstream oss;
    oss << ifs.rdbuf();
    return oss.str();
}

RHI::IShaderModule* AssetManager::getOrLoadShaderFromFile(RHI::ShaderStage stage, const std::string& path) {
    ShaderKey key{stage, path};
    auto it = shaderCache_.find(key);
    if (it != shaderCache_.end()) return it->second.get();

    std::string src = loadTextFile(path);
    if (src.empty()) {
        Core::log(Core::LogLevel::Error, std::string("AssetManager: falha ao ler shader ") + path);
        return nullptr;
    }
    RHI::ShaderModuleDesc desc{stage, src.c_str()};
    auto module = device_.createShaderModule(desc);
    if (!module) {
        Core::log(Core::LogLevel::Error, std::string("AssetManager: falha ao compilar shader ") + path);
        return nullptr;
    }
    RHI::IShaderModule* raw = module.get();
    shaderCache_.emplace(std::move(key), std::move(module));
    return raw;
}

void AssetManager::clear() {
    shaderCache_.clear();
    textureCache_.clear();
    samplerCache_.clear();
}

// Loader de imagem super simples (PPM binário P6) para exemplo rápido
static bool loadPPM(const std::string& path, uint32_t& outW, uint32_t& outH, std::vector<unsigned char>& outRGBA) {
    std::ifstream f(path, std::ios::binary);
    if (!f) return false;
    std::string magic; f >> magic; if (magic != "P6" && magic != "P3") return false;
    int w=0,h=0,maxv=0; f >> w >> h >> maxv;
    if (w<=0 || h<=0 || maxv<=0) return false;
    outW = static_cast<uint32_t>(w); outH = static_cast<uint32_t>(h);
    outRGBA.resize(static_cast<size_t>(w*h*4));
    if (magic == "P6") {
        f.get(); // consumir um byte de newline
        std::vector<unsigned char> rgb(static_cast<size_t>(w*h*3));
        f.read(reinterpret_cast<char*>(rgb.data()), rgb.size());
        if (f.gcount() != static_cast<std::streamsize>(rgb.size())) return false;
        for (size_t i=0, j=0; i<rgb.size(); i+=3, j+=4) {
            outRGBA[j+0] = rgb[i+0];
            outRGBA[j+1] = rgb[i+1];
            outRGBA[j+2] = rgb[i+2];
            outRGBA[j+3] = 255;
        }
    } else {
        // P3 ASCII
        for (int i=0; i<w*h; ++i) {
            int r=0,g=0,b=0; f >> r >> g >> b;
            size_t j = static_cast<size_t>(i*4);
            outRGBA[j+0] = static_cast<unsigned char>(r);
            outRGBA[j+1] = static_cast<unsigned char>(g);
            outRGBA[j+2] = static_cast<unsigned char>(b);
            outRGBA[j+3] = 255;
        }
    }
    return true;
}

RHI::ITexture* AssetManager::getOrLoadTextureFromFile(const std::string& path) {
    auto it = textureCache_.find(path);
    if (it != textureCache_.end()) return it->second.get();

    uint32_t w=0,h=0; std::vector<unsigned char> pixels;
    if (!loadPPM(path, w, h, pixels)) {
        Core::log(Core::LogLevel::Error, std::string("AssetManager: falha ao ler textura (PPM esperado) ") + path);
        return nullptr;
    }
    RHI::TextureDesc td{}; td.width = w; td.height = h; td.format = RHI::TextureFormat::RGBA8; td.usage = RHI::TextureUsage::Sampled; td.mipLevels = 2;
    auto tex = device_.createTexture(td, pixels.data());
    if (!tex) {
        Core::log(Core::LogLevel::Error, std::string("AssetManager: falha ao criar textura ") + path);
        return nullptr;
    }
    RHI::ITexture* raw = tex.get();
    textureCache_.emplace(path, std::move(tex));
    return raw;
}

RHI::ISampler* AssetManager::getOrCreateSampler(const RHI::SamplerDesc& desc) {
    auto it = samplerCache_.find(desc);
    if (it != samplerCache_.end()) return it->second.get();
    auto smp = device_.createSampler(desc);
    RHI::ISampler* raw = smp.get();
    samplerCache_.emplace(desc, std::move(smp));
    return raw;
}

void* AssetManager::getOrLoadMeshFromFile(const std::string& path) {
    (void)path; // placeholder
    return nullptr;
}

}


