#include "Aurora/Core/Log.hpp"
#include "Aurora/Platform/Time.hpp"
#include "Aurora/Platform/Window.hpp"
#include "Aurora/RHI/RHI.hpp"

#include <thread>

using namespace Aurora;

int main() {
    Core::initializeLogging();
    Core::log(Core::LogLevel::Info, "AuroraRuntime starting...");

    auto device = RHI::createDevice(RHI::BackendType::OpenGL);
    Core::log(Core::LogLevel::Info, std::string("RHI Device: ") + device->getName());

    Platform::WindowDesc wdesc;
    wdesc.title = "Aurora - Janela";
    auto* window = Platform::createWindow(wdesc);
    if (!window) {
        Core::log(Core::LogLevel::Critical, "Falha ao criar janela");
        return -1;
    }
    window->show();

    Platform::TimePoint start = Platform::getTimeNow();

    uint32_t winW = 0, winH = 0;
    window->getSize(winW, winH);

    RHI::SwapchainDesc scDesc{};
    scDesc.windowHandle = window->getNativeHandle();
    scDesc.width = winW;
    scDesc.height = winH;
    scDesc.vsync = true;

    auto swapchain = device->createSwapchain(scDesc);
    auto rp = device->createRenderPass(RHI::RenderPassDesc{});

    // Pipeline profissional mínima: shaders + vertex buffer + layout
    const bool use420 = device->getCapabilities().supportsGLSL420 || device->getCapabilities().hasShadingLanguage420Pack;
    const char* vsSrc = use420 ?
        "#version 420 core\nlayout(location=0) in vec2 aPos; void main(){ gl_Position = vec4(aPos,0,1); }" :
        "#version 330 core\nlayout(location=0) in vec2 aPos; void main(){ gl_Position = vec4(aPos,0,1); }";
    const char* fsSrc = use420 ?
        "#version 420 core\nlayout(std140, binding=0) uniform Globals { vec4 color; };\nout vec4 FragColor; void main(){ FragColor = color; }" :
        "#version 330 core\nlayout(std140) uniform Globals { vec4 color; };\nout vec4 FragColor; void main(){ FragColor = color; }";
    auto vs = device->createShaderModule({RHI::ShaderStage::Vertex, vsSrc});
    auto fs = device->createShaderModule({RHI::ShaderStage::Fragment, fsSrc});
    const float triVerts[] = { -0.6f,-0.5f, 0.6f,-0.5f, 0.0f,0.6f };
    const uint32_t indices[] = { 0, 1, 2 };
    auto vbo = device->createBuffer(triVerts, sizeof(triVerts), RHI::BufferUsage::Vertex);
    auto ibo = device->createBuffer(indices, sizeof(indices), RHI::BufferUsage::Index);
    alignas(16) struct Globals { float color[4]; } globals{ {0.2f, 0.9f, 0.3f, 1.0f} };
    auto ubo = device->createBuffer(&globals, sizeof(globals), RHI::BufferUsage::Uniform);
    RHI::DescriptorSetDesc setDesc{};
    setDesc.uniformBuffers.push_back({0, ubo.get(), 0, sizeof(globals), "Globals"});
    auto set = device->createDescriptorSet(setDesc);

    RHI::GraphicsPipelineDesc pipeDesc{};
    pipeDesc.vertexShader = vs.get();
    pipeDesc.fragmentShader = fs.get();
    pipeDesc.vertexLayout.stride = sizeof(float) * 2;
    pipeDesc.vertexLayout.attributes.push_back({0, 2, 0});
    auto pipeline = device->createGraphicsPipeline(pipeDesc);

    bool quit = false;
    while (!quit && window->pumpEvents()) {
        for (const auto& e : window->getEventQueue()) {
            if (e.type == Platform::EventType::WindowResize && swapchain) {
                if (e.width && e.height) swapchain->resize(e.width, e.height);
            }
        }
        // Fechar com ESC
        const auto& input = window->getInputState();
        if (input.keyPressed.test(27)) { // VK_ESCAPE
            quit = true;
        }
        window->clearEventQueue();
        uint32_t curW = 0, curH = 0;
        window->getSize(curW, curH);
        if ((curW != 0 && curH != 0) && (curW != swapchain->getWidth() || curH != swapchain->getHeight())) {
            swapchain->resize(curW, curH);
        }

        device->beginFrame();
        // Animar cor do UBO
        double t = Platform::secondsSince(start);
        globals.color[0] = 0.5f + 0.5f * static_cast<float>(std::sin(t));
        globals.color[1] = 0.5f + 0.5f * static_cast<float>(std::sin(t * 1.3));
        globals.color[2] = 0.5f + 0.5f * static_cast<float>(std::sin(t * 0.7));
        device->updateBuffer(ubo.get(), &globals, sizeof(globals));
        if (swapchain && rp) {
            device->beginRenderPass(rp.get(), swapchain.get());
            device->setGraphicsPipeline(pipeline.get());
            device->bindDescriptorSet(set.get());
            device->setVertexBuffer(vbo.get());
            device->setIndexBuffer(ibo.get());
            device->drawIndexed(3, 0);
            device->endRenderPass();
            swapchain->present();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        device->endFrame();
    }
    Platform::destroyWindow(window);

    double elapsed = Platform::secondsSince(start);
    Core::log(Core::LogLevel::Info, "Exiting. Elapsed seconds: " + std::to_string(elapsed));

    Core::shutdownLogging();
    return 0;
}


