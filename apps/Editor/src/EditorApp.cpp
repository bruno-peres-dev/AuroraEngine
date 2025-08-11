#include "EditorApp.hpp"

#include <cmath>
#include <vector>
#include <filesystem>
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "imgui.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_opengl3.h"
#include "GLTexture.hpp"

// Forward decl do handler do backend (evita macro guards que possam ocultar a decl)
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

using namespace Aurora;
using namespace Aurora::EditorAppNS;

namespace { }

EditorApp::~EditorApp() { shutdown(); }

int EditorApp::run() {
    Core::initializeLogging();
    Core::log(Core::LogLevel::Info, "AuroraEditor starting...");
    if (!initialize()) { shutdown(); Core::shutdownLogging(); return -1; }

    Platform::TimePoint last = Platform::getTimeNow();
    while (!quit_ && window_->pumpEvents()) {
        Platform::TimePoint now = Platform::getTimeNow();
        double dt = static_cast<double>(now.ticks - last.ticks) / static_cast<double>(Platform::getTicksPerSecond());
        last = now;

        for (const auto& e : window_->getEventQueue()) {
            if (e.type == Platform::EventType::WindowClose) quit_ = true;
            if (e.type == Platform::EventType::WindowResize && swapchain_) {
                if (e.width && e.height) { swapchain_->resize(e.width, e.height); }
            }
        }
        window_->clearEventQueue();

        update(dt);

        device_->beginFrame();

        // Begin frame ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // Dockspace no viewport principal
        ImGui::DockSpaceOverViewport();

        // Painel Viewport
        bool viewportHovered = false;
        ImGui::Begin("Viewport");
        ImVec2 avail = ImGui::GetContentRegionAvail();
        uint32_t newW = (uint32_t)std::max(1.0f, avail.x);
        uint32_t newH = (uint32_t)std::max(1.0f, avail.y);
        if (newW != viewportWidth_ || newH != viewportHeight_) {
            viewportWidth_ = newW; viewportHeight_ = newH;
            renderSystem_->ensureViewport(viewport_, viewportWidth_, viewportHeight_);
            // Atualiza set do blit para apontar para nova textura
            RHI::SamplerDesc sdesc{}; auto* smp = assets_->getOrCreateSampler(sdesc);
            RHI::DescriptorSetDesc setB{};
            RHI::DescriptorSetDesc::SampledTextureBinding stb{}; stb.binding = 0; stb.texture = viewport_.color.get(); stb.sampler = smp; stb.uniformName = "uTex";
            setB.sampledTextures.push_back(stb);
            setBlit_ = device_->createDescriptorSet(setB);
        }
        // Render viewport offscreen (cena)
        render();

        // Mostrar a textura do viewport
        auto* gltex = static_cast<RHI::GLTexture*>(viewport_.color.get());
        ImGui::Image((ImTextureID)(intptr_t)gltex->id_, ImVec2((float)viewportWidth_, (float)viewportHeight_), ImVec2(0,1), ImVec2(1,0));
        viewportHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
        ImGui::End();

        // Limpa o backbuffer antes de desenhar a UI
        if (rpBackbuffer_) {
            auto cmd = device_->createCommandList();
            cmd->begin();
            cmd->beginRenderPass(rpBackbuffer_.get(), swapchain_.get());
            cmd->endRenderPass();
            cmd->end();
            device_->submit(cmd.get());
        }

        // Render UI e present
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        swapchain_->present();
        device_->endFrame();
    }

    shutdown();
    Core::shutdownLogging();
    return 0;
}

bool EditorApp::initialize() {
    // Window
    Platform::WindowDesc wdesc; wdesc.title = "Aurora Editor"; wdesc.width = 1600; wdesc.height = 900;
    window_ = Platform::createWindow(wdesc);
    if (!window_) { Core::log(Core::LogLevel::Critical, "Falha ao criar janela do Editor"); return false; }
    window_->show();

    // Device, swapchain e render pass do backbuffer
    device_ = RHI::createDevice(RHI::BackendType::OpenGL);
    uint32_t w=0,h=0; window_->getSize(w,h);
    RHI::SwapchainDesc sc{}; sc.windowHandle = window_->getNativeHandle(); sc.width=w; sc.height=h; sc.vsync = vsync_;
    swapchain_ = device_->createSwapchain(sc);
    if (!swapchain_) { Core::log(Core::LogLevel::Critical, "Falha ao criar swapchain"); return false; }
    rpBackbuffer_ = device_->createRenderPass(RHI::RenderPassDesc{});
    assets_ = std::make_unique<Assets::AssetManager>(*device_);

    // Dear ImGui init
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init((HWND)window_->getNativeHandle());
    ImGui_ImplOpenGL3_Init("#version 330");
    imguiInitialized_ = true;
    // Encaminhar mensagens Win32 para ImGui
    Platform::setWin32WndProcHook([](void* hwnd, unsigned int msg, unsigned long long wparam, long long lparam)->bool{
        return ImGui_ImplWin32_WndProcHandler((HWND)hwnd, msg, (WPARAM)wparam, (LPARAM)lparam) != 0;
    });

    // Viewport offscreen inicial
    viewportWidth_ = 1280; viewportHeight_ = 720;
    renderSystem_->ensureViewport(viewport_, viewportWidth_, viewportHeight_);

    // Shaders e pipelines
    vsScene_ = assets_->getOrLoadShaderFromFile(RHI::ShaderStage::Vertex, "apps/Editor/shaders/viewport.vert.glsl");
    fsScene_ = assets_->getOrLoadShaderFromFile(RHI::ShaderStage::Fragment, "apps/Editor/shaders/viewport.frag.glsl");
    if (!vsScene_ || !fsScene_) return false;

    // Cena: um triângulo em world space com UV dummy
    struct V { float pos[3]; float uv[2]; };
    const V verts[] = {
        {{-0.5f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        {{ 0.5f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{ 0.0f, 1.0f, 0.0f}, {0.5f, 1.0f}},
    };
    const uint32_t idx[] = {0,1,2};
    vboScene_ = device_->createBuffer(verts, sizeof(verts), RHI::BufferUsage::Vertex);
    iboScene_ = device_->createBuffer(idx, sizeof(idx), RHI::BufferUsage::Index);

    uboScene_ = device_->createBuffer(nullptr, sizeof(UBOScene), RHI::BufferUsage::Uniform);
    RHI::DescriptorSetDesc setS{}; setS.uniformBuffers.push_back({0, uboScene_.get(), 0, sizeof(UBOScene), "Globals"});
    setScene_ = device_->createDescriptorSet(setS);

    RHI::GraphicsPipelineDesc pScene{};
    pScene.vertexShader = vsScene_;
    pScene.fragmentShader = fsScene_;
    pScene.vertexLayout.stride = sizeof(V);
    pScene.vertexLayout.attributes.push_back({0, 3, 0});
    pScene.vertexLayout.attributes.push_back({1, 2, (uint32_t)offsetof(V, uv)});
    pScene.state.depthStencil.depthTestEnable = true;
    pScene.state.depthStencil.depthWriteEnable = true;
    pScene.state.raster.cullMode = RHI::CullMode::None; // evita culling indevido
    pipeScene_ = device_->createGraphicsPipeline(pScene);

    // Blit pipeline: reutiliza os shaders de textura da Runtime
    vsBlit_ = assets_->getOrLoadShaderFromFile(RHI::ShaderStage::Vertex, "apps/Runtime/shaders/texture.vert.glsl");
    fsBlit_ = assets_->getOrLoadShaderFromFile(RHI::ShaderStage::Fragment, "apps/Runtime/shaders/texture.frag.glsl");
    if (!vsBlit_ || !fsBlit_) return false;

    struct VB { float pos[2]; float uv[2]; };
    const VB quad[] = {
        {{-1.f,-1.f},{0.f,0.f}}, {{ 1.f,-1.f},{1.f,0.f}}, {{ 1.f, 1.f},{1.f,1.f}},
        {{-1.f,-1.f},{0.f,0.f}}, {{ 1.f, 1.f},{1.f,1.f}}, {{-1.f, 1.f},{0.f,1.f}},
    };
    vboBlit_ = device_->createBuffer(quad, sizeof(quad), RHI::BufferUsage::Vertex);
    RHI::GraphicsPipelineDesc pBlit{};
    pBlit.vertexShader = vsBlit_;
    pBlit.fragmentShader = fsBlit_;
    pBlit.vertexLayout.stride = sizeof(VB);
    pBlit.vertexLayout.attributes.push_back({0,2,0});
    pBlit.vertexLayout.attributes.push_back({1,2,(uint32_t)offsetof(VB, uv)});
    pipeBlit_ = device_->createGraphicsPipeline(pBlit);

    RHI::SamplerDesc sdesc{}; sdesc.mipmapMode = RHI::SamplerDesc::MipmapMode::None;
    auto* smp = assets_->getOrCreateSampler(sdesc);
    RHI::DescriptorSetDesc setB{};
    RHI::DescriptorSetDesc::SampledTextureBinding stb{}; stb.binding = 0; stb.texture = viewport_.color.get(); stb.sampler = smp; stb.uniformName = "uTex";
    setB.sampledTextures.push_back(stb);
    setBlit_ = device_->createDescriptorSet(setB);

    return true;
}

void EditorApp::shutdown() {
    // ImGui shutdown
    if (imguiInitialized_) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
        imguiInitialized_ = false;
    }
    setBlit_.reset();
    pipeBlit_.reset();
    vboBlit_.reset();

    setScene_.reset();
    uboScene_.reset();
    iboScene_.reset();
    vboScene_.reset();
    pipeScene_.reset();

    viewport_.renderPass.reset();
    viewport_.depth.reset();
    viewport_.color.reset();

    assets_.reset();
    rpBackbuffer_.reset();
    swapchain_.reset();
    device_.reset();
    if (window_) { Platform::destroyWindow(window_); window_ = nullptr; }
}

void EditorApp::handleCameraInput(double dt) {
    ImGuiIO& io = ImGui::GetIO();
    bool viewportAllowInput = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) && !io.WantCaptureMouse;
    cameraSystem_.handleInput(window_->getInputState(), viewportAllowInput, dt);
}

void EditorApp::computeViewProj(float outView[16], float outProj[16], uint32_t fbWidth, uint32_t fbHeight) const {
    cameraSystem_.computeViewProj(fbWidth, fbHeight, outView, outProj);
}

void EditorApp::update(double dt) {
    handleCameraInput(dt);
}

void EditorApp::render() {
    // Atualiza UBO com VP
    UBOScene u{}; computeViewProj(u.view, u.proj, viewportWidth_, viewportHeight_);
    device_->updateBuffer(uboScene_.get(), &u, sizeof(u));

    // Render da cena no viewport offscreen
    if (viewport_.renderPass) {
        auto cmd = device_->createCommandList();
        cmd->begin();
        cmd->beginRenderPass(viewport_.renderPass.get(), nullptr);
        cmd->setGraphicsPipeline(pipeScene_.get());
        cmd->bindDescriptorSet(setScene_.get());
        cmd->setVertexBuffer(vboScene_.get());
        cmd->setIndexBuffer(iboScene_.get());
        cmd->drawIndexed(3, 0, RHI::IndexType::Uint32);
        cmd->endRenderPass();
        cmd->end();
        device_->submit(cmd.get());
    }

    // (não blitar para o backbuffer; mostramos a textura no painel do ImGui)
}


