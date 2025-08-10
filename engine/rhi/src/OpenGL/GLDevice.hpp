#pragma once

#include "Aurora/RHI/RHI.hpp"

#ifdef _WIN32
#  include "WGLContext.hpp"
#endif

namespace Aurora::RHI {

class GLRenderPass final : public IRenderPass {
public:
    explicit GLRenderPass(const RenderPassDesc& desc) : desc_(desc) {}
    RenderPassDesc desc_{};
};

class GLSwapchain final : public ISwapchain {
public:
    explicit GLSwapchain(uint32_t w, uint32_t h) : width_(w), height_(h) {}
    void present() override;
    void resize(uint32_t width, uint32_t height) override { width_ = width; height_ = height; }
    uint32_t getWidth() const override { return width_; }
    uint32_t getHeight() const override { return height_; }

#ifdef _WIN32
    WGLContext context_{};
#endif
private:
    uint32_t width_{0};
    uint32_t height_{0};
};

class GLDevice final : public IDevice {
public:
    const char* getName() const override { return "OpenGL"; }
    void beginFrame() override {}
    void endFrame() override {}
    std::unique_ptr<ISwapchain> createSwapchain(const SwapchainDesc& desc) override;
    std::unique_ptr<IRenderPass> createRenderPass(const RenderPassDesc& desc) override {
        return std::make_unique<GLRenderPass>(desc);
    }
    void beginRenderPass(IRenderPass* renderPass, ISwapchain* target) override;
    void endRenderPass() override;

private:
    // RHI resource implementations
    class GLShaderModule final : public IShaderModule {
    public:
        GLShaderModule(ShaderStage stage, unsigned int id) : stage_(stage), id_(id) {}
        ~GLShaderModule() override;
        ShaderStage getStage() const override { return stage_; }
        unsigned int id_{0};
    private:
        ShaderStage stage_;
    };

    class GLBuffer final : public IBuffer {
    public:
        GLBuffer(size_t size, BufferUsage usage, unsigned int id) : size_(size), usage_(usage), id_(id) {}
        ~GLBuffer() override;
        size_t getSize() const override { return size_; }
        BufferUsage getUsage() const override { return usage_; }
        unsigned int id_{0};
    private:
        size_t size_{};
        BufferUsage usage_{};
    };

    class GLGraphicsPipeline final : public IGraphicsPipeline {
    public:
        explicit GLGraphicsPipeline(unsigned int program, unsigned int vao) : program_(program), vao_(vao) {}
        ~GLGraphicsPipeline() override;
        unsigned int program_{0};
        unsigned int vao_{0};
    };

    class GLDescriptorSet final : public IDescriptorSet {
    public:
        explicit GLDescriptorSet(const DescriptorSetDesc& d) : desc(d) {}
        DescriptorSetDesc desc;
    };

public:
    std::unique_ptr<IShaderModule> createShaderModule(const ShaderModuleDesc& desc) override;
    std::unique_ptr<IBuffer> createBuffer(const void* data, size_t bytes, BufferUsage usage) override;
    std::unique_ptr<IGraphicsPipeline> createGraphicsPipeline(const GraphicsPipelineDesc& desc) override;
    std::unique_ptr<IDescriptorSet> createDescriptorSet(const DescriptorSetDesc& desc) override;
    void updateBuffer(IBuffer* buffer, const void* data, size_t bytes, size_t dstOffset = 0) override;
    void setGraphicsPipeline(IGraphicsPipeline* pipeline) override;
    void setVertexBuffer(IBuffer* buffer) override;
    void setIndexBuffer(IBuffer* buffer) override;
    void bindDescriptorSet(IDescriptorSet* set) override;
    void draw(uint32_t vertexCount, uint32_t firstVertex) override;
    void drawIndexed(uint32_t indexCount, uint32_t firstIndex) override;
    std::unique_ptr<ICommandList> createCommandList() override;
    void submit(ICommandList* list) override { (void)list; /* immediate path: already executed */ }
    Capabilities getCapabilities() const override { return caps_; }

private:
    GLGraphicsPipeline* currentPipeline_{nullptr};
    GLBuffer* currentVertexBuffer_{nullptr};
    GLBuffer* currentIndexBuffer_{nullptr};
    Capabilities caps_{};

    class GLCommandList final : public ICommandList {
    public:
        explicit GLCommandList(GLDevice& dev) : device_(dev) {}
        void begin() override {}
        void end() override {}
        void beginRenderPass(IRenderPass* renderPass, ISwapchain* target) override { device_.beginRenderPass(renderPass, target); }
        void endRenderPass() override { device_.endRenderPass(); }
        void setGraphicsPipeline(IGraphicsPipeline* pipeline) override { device_.setGraphicsPipeline(pipeline); }
        void setVertexBuffer(IBuffer* buffer) override { device_.setVertexBuffer(buffer); }
        void setIndexBuffer(IBuffer* buffer) override { device_.setIndexBuffer(buffer); }
        void bindDescriptorSet(IDescriptorSet* /*set*/) override { /* TODO: bind UBOs via glBindBufferBase */ }
        void draw(uint32_t vertexCount, uint32_t firstVertex) override { device_.draw(vertexCount, firstVertex); }
        void drawIndexed(uint32_t indexCount, uint32_t firstIndex) override { device_.drawIndexed(indexCount, firstIndex); }
    private:
        GLDevice& device_;
    };
};

}


