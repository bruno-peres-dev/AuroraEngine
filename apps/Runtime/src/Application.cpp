#include "Application.hpp"

#include <fstream>
#include <sstream>
#include <filesystem>

namespace Aurora::RuntimeApp {

static std::string loadTextFile(const std::string& path) {
    std::ifstream ifs(path, std::ios::in);
    if (!ifs) return {};
    std::ostringstream oss;
    oss << ifs.rdbuf();
    return oss.str();
}

static std::string loadFirstExisting(const std::vector<std::string>& candidates, std::string& usedPathOut) {
    for (const auto& p : candidates) {
        std::string data = loadTextFile(p);
        if (!data.empty()) { usedPathOut = p; return data; }
    }
    usedPathOut.clear();
    return {};
}

bool Application::initialize() {
    // Window
    Platform::WindowDesc wdesc; wdesc.title = "Aurora - Janela";
    window_ = Platform::createWindow(wdesc);
    if (!window_) { Core::log(Core::LogLevel::Critical, "Falha ao criar janela"); return false; }
    window_->show();

    // Device e swapchain
    device_ = RHI::createDevice(RHI::BackendType::OpenGL);
    uint32_t w = 0, h = 0; window_->getSize(w, h);
    RHI::SwapchainDesc sc{}; sc.windowHandle = window_->getNativeHandle(); sc.width = w; sc.height = h; sc.vsync = vsyncEnabled_;
    swapchain_ = device_->createSwapchain(sc);
    if (!swapchain_) { Core::log(Core::LogLevel::Critical, "Falha ao criar swapchain"); return false; }
    renderPass_ = device_->createRenderPass(RHI::RenderPassDesc{});
    assets_ = std::make_unique<Assets::AssetManager>(*device_);

    // Shaders via arquivos (tenta múltiplos caminhos) — agora usando shaders de textura
    std::vector<std::string> vsCandidates = {
        "apps/Runtime/shaders/texture.vert.glsl",
        "shaders/texture.vert.glsl"
    };
    std::vector<std::string> fsCandidates = {
        "apps/Runtime/shaders/texture.frag.glsl",
        "shaders/texture.frag.glsl"
    };
    std::string usedVsPath, usedFsPath;
    // Reutiliza função de leitura para localizar o primeiro caminho existente
    std::string vsProbe = loadFirstExisting(vsCandidates, usedVsPath);
    std::string fsProbe = loadFirstExisting(fsCandidates, usedFsPath);
    if (vsProbe.empty() || fsProbe.empty()) {
        Core::log(Core::LogLevel::Critical, std::string("Falha ao carregar shaders do disco. cwd=") + std::filesystem::current_path().string());
        for (const auto& p : vsCandidates) Core::log(Core::LogLevel::Error, std::string("Tentado VS: ") + p);
        for (const auto& p : fsCandidates) Core::log(Core::LogLevel::Error, std::string("Tentado FS: ") + p);
        return false;
    }
    Core::log(Core::LogLevel::Info, std::string("Shaders carregados: VS=") + usedVsPath + ", FS=" + usedFsPath);
    vs_ = assets_->getOrLoadShaderFromFile(RHI::ShaderStage::Vertex, usedVsPath);
    fs_ = assets_->getOrLoadShaderFromFile(RHI::ShaderStage::Fragment, usedFsPath);
    if (!vs_ || !fs_) {
        Core::log(Core::LogLevel::Critical, "Falha ao compilar shaders");
        return false;
    }

    // Buffers (pos,uv) e índices
    const float verts[] = {
        // pos        // uv
        -0.8f,-0.8f,   0.0f,0.0f,
         0.8f,-0.8f,   1.0f,0.0f,
         0.0f, 0.8f,   0.5f,1.0f,
    };
    const uint32_t indices[] = { 0, 1, 2 };
    vbo_ = device_->createBuffer(verts, sizeof(verts), RHI::BufferUsage::Vertex);
    ibo_ = device_->createBuffer(indices, sizeof(indices), RHI::BufferUsage::Index);
    ubo_ = device_->createBuffer(&globals_, sizeof(globals_), RHI::BufferUsage::Uniform);
    RHI::DescriptorSetDesc setDesc{};
    setDesc.uniformBuffers.push_back({0, ubo_.get(), 0, sizeof(globals_), "Globals"});
    // Textura e sampler
    // Tenta múltiplos caminhos para a textura
    std::vector<std::string> texCandidates = {
        "apps/Runtime/assets/checker.ppm",
        "assets/checker.ppm"
    };
    std::string usedTexPath, _;
    (void)loadFirstExisting(texCandidates, usedTexPath); // apenas para descobrir caminho
    auto* tex = assets_->getOrLoadTextureFromFile(!usedTexPath.empty() ? usedTexPath : texCandidates.back());
    RHI::SamplerDesc sdesc{}; // linear, repeat por default
    sdesc.mipmapMode = RHI::SamplerDesc::MipmapMode::Linear;
    auto* smp = assets_->getOrCreateSampler(sdesc);
    if (tex && smp) {
        setDesc.sampledTextures.push_back({1, tex, smp, "uTex"}); // binding 1
    }
    descriptorSet_ = device_->createDescriptorSet(setDesc);

    // Pipeline
    RHI::GraphicsPipelineDesc p{};
    p.vertexShader = vs_;
    p.fragmentShader = fs_;
    p.vertexLayout.stride = sizeof(float) * 4;
    p.vertexLayout.attributes.push_back({0, 2, 0});         // aPos
    p.vertexLayout.attributes.push_back({1, 2, sizeof(float)*2}); // aUV
    pipeline_ = device_->createGraphicsPipeline(p);

    return true;
}

void Application::shutdown() {
    // Destruir recursos GL antes da janela/contexto para evitar chamadas GL sem contexto
    pipeline_.reset();
    descriptorSet_.reset();
    ubo_.reset();
    ibo_.reset();
    vbo_.reset();
    if (assets_) assets_->clear();
    renderPass_.reset();
    swapchain_.reset();
    device_.reset();
    if (window_) { Platform::destroyWindow(window_); window_ = nullptr; }
}

int Application::run() {
    Core::initializeLogging();
    Core::log(Core::LogLevel::Info, "AuroraRuntime starting...");

    if (!initialize()) { shutdown(); Core::shutdownLogging(); return -1; }

    Platform::TimePoint start = Platform::getTimeNow();
    Platform::TimePoint last = start;

    while (!quit_ && window_->pumpEvents()) {
        // Timing
        Platform::TimePoint now = Platform::getTimeNow();
        double dt = static_cast<double>(now.ticks - last.ticks) / static_cast<double>(Platform::getTicksPerSecond());
        last = now;

        // Eventos
        for (const auto& e : window_->getEventQueue()) {
            if (e.type == Platform::EventType::WindowResize && swapchain_) {
                if (e.width && e.height) swapchain_->resize(e.width, e.height);
            }
            if (e.type == Platform::EventType::WindowClose) {
                quit_ = true;
            }
            if (e.type == Platform::EventType::KeyDown && e.key == Platform::Key::V) {
                vsyncEnabled_ = !vsyncEnabled_;
                if (swapchain_) swapchain_->setVsync(vsyncEnabled_);
                Core::log(Core::LogLevel::Info, std::string("VSync ") + (vsyncEnabled_ ? "ON" : "OFF"));
            }
            if (e.type == Platform::EventType::KeyDown && e.key == Platform::Key::W) {
                static bool wire = false; wire = !wire;
                if (device_) device_->setDebugWireframe(wire);
                Core::log(Core::LogLevel::Info, std::string("Wireframe ") + (wire ? "ON" : "OFF"));
            }
        }
        window_->clearEventQueue();

        // Update do usuário com dt
        onUpdate(dt);

        // Render
        device_->beginFrame();

        // Atualiza UBO com estado de aplicação (ex.: cor animada)
        device_->updateBuffer(ubo_.get(), &globals_, sizeof(globals_));

        if (swapchain_ && renderPass_) {
            auto cmd = device_->createCommandList();
            cmd->begin();
            cmd->beginRenderPass(renderPass_.get(), swapchain_.get());
            cmd->setGraphicsPipeline(pipeline_.get());
            cmd->bindDescriptorSet(descriptorSet_.get());
            cmd->setVertexBuffer(vbo_.get());
            cmd->setIndexBuffer(ibo_.get());
            onRender();
            cmd->drawIndexed(3, 0, RHI::IndexType::Uint32);
            cmd->endRenderPass();
            cmd->end();
            device_->submit(cmd.get());
            swapchain_->present();
        }
        device_->endFrame();
    }

    double elapsed = Platform::secondsSince(start);
    Core::log(Core::LogLevel::Info, "Exiting. Elapsed seconds: " + std::to_string(elapsed));
    shutdown();
    Core::shutdownLogging();
    return 0;
}

}


