#pragma once

#include "Aurora/Core/Log.hpp"
#include "Aurora/Platform/Window.hpp"
#include "Aurora/Platform/Time.hpp"
#include "Aurora/RHI/RHI.hpp"
#include "Aurora/Assets/AssetManager.hpp"

#include <memory>

// Sistemas
#include "CameraSystem.hpp"
#include "RenderSystem.hpp"

namespace Aurora::EditorAppNS {

class EditorApp {
public:
    EditorApp() = default;
    ~EditorApp();
    int run();

private:
    bool initialize();
    void shutdown();
    void update(double dt);
    void render();

    // Helpers de câmera
    void handleCameraInput(double dt);
    void computeViewProj(float outView[16], float outProj[16], uint32_t fbWidth, uint32_t fbHeight) const;

private:
    // Core
    std::unique_ptr<RHI::IDevice> device_{};
    Platform::IWindow* window_{};
    std::unique_ptr<RHI::ISwapchain> swapchain_{};
    std::unique_ptr<RHI::IRenderPass> rpBackbuffer_{};
    std::unique_ptr<Assets::AssetManager> assets_{};

    // Viewport offscreen (color + depth) e pass de render
    ViewportResources viewport_{};

    // Pipeline simples para desenhar uma cena dummy (triângulo) no viewport
    RHI::IShaderModule* vsScene_{};
    RHI::IShaderModule* fsScene_{};
    std::unique_ptr<RHI::IGraphicsPipeline> pipeScene_{};
    std::unique_ptr<RHI::IBuffer> vboScene_{};
    std::unique_ptr<RHI::IBuffer> iboScene_{};

    // UBO para VP matrix
    struct UBOScene { alignas(16) float view[16]; alignas(16) float proj[16]; };
    std::unique_ptr<RHI::IBuffer> uboScene_{};
    std::unique_ptr<RHI::IDescriptorSet> setScene_{};

    // Pipeline para blit do viewportColor ao backbuffer (usa shader de textura já existente)
    RHI::IShaderModule* vsBlit_{};
    RHI::IShaderModule* fsBlit_{};
    std::unique_ptr<RHI::IGraphicsPipeline> pipeBlit_{};
    std::unique_ptr<RHI::IBuffer> vboBlit_{};
    std::unique_ptr<RHI::IDescriptorSet> setBlit_{};

    // Estado
    bool quit_{false};
    bool vsync_{true};
    CameraSystem cameraSystem_{};
    std::unique_ptr<RenderSystem> renderSystem_{};
    uint32_t viewportWidth_{1280};
    uint32_t viewportHeight_{720};
    bool imguiInitialized_{false};
};

}


