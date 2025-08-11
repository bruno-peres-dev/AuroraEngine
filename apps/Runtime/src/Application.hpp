#pragma once

#include "Aurora/Core/Log.hpp"
#include "Aurora/Platform/Window.hpp"
#include "Aurora/Platform/Time.hpp"
#include "Aurora/RHI/RHI.hpp"
#include "Aurora/Assets/AssetManager.hpp"

#include <memory>
#include <string>

namespace Aurora::RuntimeApp {

class Application {
public:
    virtual ~Application() = default;

    // Executa a aplicação (cria janela, device, recursos e entra no loop principal)
    int run();

protected:
    // Callbacks do usuário
    virtual void onUpdate(double deltaSeconds) = 0;
    virtual void onRender() {}

    // Acesso a objetos principais
    RHI::IDevice* getDevice() { return device_.get(); }
    Platform::IWindow* getWindow() { return window_; }

    // Estruturas de dados compartilhadas com o render
    struct Globals { float color[4]; };
    Globals& getGlobals() { return globals_; }

private:
    bool initialize();
    void shutdown();

    // Recursos
    std::unique_ptr<RHI::IDevice> device_{};
    Platform::IWindow* window_{};
    std::unique_ptr<RHI::ISwapchain> swapchain_{};
    std::unique_ptr<RHI::IRenderPass> renderPass_{};
    // Shaders são de propriedade do AssetManager
    RHI::IShaderModule* vs_{};
    RHI::IShaderModule* fs_{};
    std::unique_ptr<RHI::IBuffer> vbo_{};
    std::unique_ptr<RHI::IBuffer> ibo_{};
    std::unique_ptr<RHI::IBuffer> ubo_{};
    std::unique_ptr<RHI::IGraphicsPipeline> pipeline_{};
    std::unique_ptr<RHI::IDescriptorSet> descriptorSet_{};
    std::unique_ptr<Assets::AssetManager> assets_{};

    // Estado
    Globals globals_{{0.2f, 0.9f, 0.3f, 1.0f}};
    bool quit_ = false;
    bool vsyncEnabled_ = true;
};

}


