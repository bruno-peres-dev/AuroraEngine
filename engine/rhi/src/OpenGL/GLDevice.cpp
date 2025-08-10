#include "GLDevice.hpp"
#include "Aurora/Core/Log.hpp"

#include <glad/glad.h>

namespace Aurora::RHI {

GLDevice::GLShaderModule::~GLShaderModule() {
    if (id_) glDeleteShader(id_);
}

GLDevice::GLBuffer::~GLBuffer() {
    if (id_) glDeleteBuffers(1, &id_);
}

GLDevice::GLGraphicsPipeline::~GLGraphicsPipeline() {
    if (vao_) glDeleteVertexArrays(1, &vao_);
    if (program_) glDeleteProgram(program_);
}

std::unique_ptr<ISwapchain> GLDevice::createSwapchain(const SwapchainDesc& desc) {
    auto sc = std::make_unique<GLSwapchain>(desc.width, desc.height);
#ifdef _WIN32
    HWND hwnd = static_cast<HWND>(desc.windowHandle);
    if (!sc->context_.initialize(hwnd, desc.vsync)) {
        Core::log(Core::LogLevel::Error, "Falha ao inicializar contexto WGL");
        return nullptr;
    }
    // Detect capabilities after context creation and glad load
    caps_.supportsGLSL420 = GLAD_GL_VERSION_4_2 != 0;
    caps_.hasShadingLanguage420Pack = false; // extensão não checada; versão já cobre o caminho 420
#endif
    return sc;
}

void GLSwapchain::present() {
#ifdef _WIN32
    context_.swapBuffers();
#endif
}

// Define missing tokens for modern GL when using legacy headers
#ifndef GL_LINK_STATUS
#define GL_LINK_STATUS 0x8B82
#endif
#ifndef GL_COMPILE_STATUS
#define GL_COMPILE_STATUS 0x8B81
#endif
#ifndef GL_ARRAY_BUFFER
#define GL_ARRAY_BUFFER 0x8892
#endif
#ifndef GL_STATIC_DRAW
#define GL_STATIC_DRAW 0x88E4
#endif
#ifndef GL_VERTEX_SHADER
#define GL_VERTEX_SHADER 0x8B31
#endif
#ifndef GL_FRAGMENT_SHADER
#define GL_FRAGMENT_SHADER 0x8B30
#endif
#ifndef GL_FLOAT
#define GL_FLOAT 0x1406
#endif

void GLDevice::beginRenderPass(IRenderPass* renderPass, ISwapchain* target) {
    (void)target;
    auto* rp = static_cast<GLRenderPass*>(renderPass);
    glViewport(0, 0, static_cast<GLsizei>(target->getWidth()), static_cast<GLsizei>(target->getHeight()));
    if (rp->desc_.clearColorEnabled || rp->desc_.clearDepthEnabled) {
        GLbitfield mask = 0;
        if (rp->desc_.clearColorEnabled) {
            glClearColor(rp->desc_.clearColor[0], rp->desc_.clearColor[1], rp->desc_.clearColor[2], rp->desc_.clearColor[3]);
            mask |= GL_COLOR_BUFFER_BIT;
        }
        if (rp->desc_.clearDepthEnabled) {
            glClearDepth(rp->desc_.clearDepth);
            mask |= GL_DEPTH_BUFFER_BIT;
        }
        glClear(mask);
    }

    // render pass não emite draw por conta própria (feito via draw())
}

void GLDevice::endRenderPass() {
    // Nothing for now
}

// Simple triangle validation on first frame could be added later
static GLuint compile(GLenum type, const char* src) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);
    GLint ok = 0;
    glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[1024]; GLsizei len = 0; glGetShaderInfoLog(s, 1024, &len, log);
        Core::log(Core::LogLevel::Error, std::string("GL shader error: ") + log);
    }
    return s;
}

std::unique_ptr<IShaderModule> GLDevice::createShaderModule(const ShaderModuleDesc& desc) {
    GLenum type = (desc.stage == ShaderStage::Vertex) ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER;
    GLuint id = compile(type, desc.source);
    return std::make_unique<GLShaderModule>(desc.stage, id);
}

std::unique_ptr<IBuffer> GLDevice::createBuffer(const void* data, size_t bytes, BufferUsage usage) {
    unsigned int id = 0;
    glGenBuffers(1, &id);
    GLenum target = GL_ARRAY_BUFFER;
    if (usage == BufferUsage::Index) target = 0x8893 /*GL_ELEMENT_ARRAY_BUFFER*/;
    glBindBuffer(target, id);
    glBufferData(target, static_cast<ptrdiff_t>(bytes), data, GL_STATIC_DRAW);
    return std::make_unique<GLBuffer>(bytes, usage, id);
}

std::unique_ptr<IGraphicsPipeline> GLDevice::createGraphicsPipeline(const GraphicsPipelineDesc& desc) {
    GLuint program = glCreateProgram();
    auto* vs = static_cast<GLShaderModule*>(desc.vertexShader);
    auto* fs = static_cast<GLShaderModule*>(desc.fragmentShader);
    glAttachShader(program, vs->id_);
    glAttachShader(program, fs->id_);
    glLinkProgram(program);
    GLint linked = 0; glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked) {
        char logBuf[1024]; GLsizei len = 0; glGetProgramInfoLog(program, 1024, &len, logBuf);
        Core::log(Core::LogLevel::Error, std::string("GL link error: ") + logBuf);
    }
    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    for (const auto& a : desc.vertexLayout.attributes) {
        glVertexAttribPointer(a.location, a.components, GL_FLOAT, GL_FALSE, static_cast<GLsizei>(desc.vertexLayout.stride), reinterpret_cast<const void*>(static_cast<uintptr_t>(a.offset)));
        glEnableVertexAttribArray(a.location);
    }
    // Raster/state
    if (desc.state.raster.depthTestEnable) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
    glDepthMask(desc.state.raster.depthWriteEnable ? GL_TRUE : GL_FALSE);
    switch (desc.state.raster.depthFunc) {
        case DepthFunc::Less: glDepthFunc(GL_LESS); break;
        case DepthFunc::LessEqual: glDepthFunc(GL_LEQUAL); break;
        case DepthFunc::Greater: glDepthFunc(GL_GREATER); break;
        case DepthFunc::GreaterEqual: glDepthFunc(GL_GEQUAL); break;
        case DepthFunc::Equal: glDepthFunc(GL_EQUAL); break;
        case DepthFunc::NotEqual: glDepthFunc(GL_NOTEQUAL); break;
        case DepthFunc::Always: glDepthFunc(GL_ALWAYS); break;
        case DepthFunc::Never: default: glDepthFunc(GL_NEVER); break;
    }
    if (desc.state.raster.cullMode == CullMode::None) glDisable(GL_CULL_FACE); else glEnable(GL_CULL_FACE);
    if (desc.state.raster.cullMode == CullMode::Back) glCullFace(GL_BACK); else if (desc.state.raster.cullMode == CullMode::Front) glCullFace(GL_FRONT);
    glFrontFace(desc.state.raster.frontFaceCCW ? GL_CCW : GL_CW);
    if (desc.state.raster.blendEnable) { glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); } else { glDisable(GL_BLEND); }
    return std::make_unique<GLGraphicsPipeline>(program, vao);
}

void GLDevice::setGraphicsPipeline(IGraphicsPipeline* pipeline) {
    currentPipeline_ = static_cast<GLGraphicsPipeline*>(pipeline);
    glUseProgram(currentPipeline_->program_);
    glBindVertexArray(currentPipeline_->vao_);
}

void GLDevice::setVertexBuffer(IBuffer* buffer) {
    auto* glb = static_cast<GLBuffer*>(buffer);
    glBindBuffer(GL_ARRAY_BUFFER, glb->id_);
    currentVertexBuffer_ = glb;
}

void GLDevice::draw(uint32_t vertexCount, uint32_t firstVertex) {
    glDrawArrays(GL_TRIANGLES, static_cast<GLint>(firstVertex), static_cast<GLsizei>(vertexCount));
}

std::unique_ptr<IDescriptorSet> GLDevice::createDescriptorSet(const DescriptorSetDesc& desc) {
    return std::make_unique<GLDescriptorSet>(desc);
}

void GLDevice::setIndexBuffer(IBuffer* buffer) {
    auto* glb = static_cast<GLBuffer*>(buffer);
    currentIndexBuffer_ = glb;
    // Bind to element array
    glBindBuffer(0x8893 /*GL_ELEMENT_ARRAY_BUFFER*/, glb->id_);
}

void GLDevice::drawIndexed(uint32_t indexCount, uint32_t firstIndex) {
    (void)firstIndex; // not supporting offset for now
    // glDrawElements with unsigned int indices is sufficient for sample
    typedef void (APIENTRYP PFNGLDRAWELEMENTSPROC)(GLenum, GLsizei, GLenum, const void*);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indexCount), 0x1405 /*GL_UNSIGNED_INT*/, nullptr);
}

void GLDevice::bindDescriptorSet(IDescriptorSet* set) {
    auto* glset = static_cast<GLDescriptorSet*>(set);
    for (const auto& ub : glset->desc.uniformBuffers) {
        auto* buf = static_cast<GLBuffer*>(ub.buffer);
        // Se o shader não especifica layout(binding), associamos bloco ao binding com glUniformBlockBinding
        GLuint program = currentPipeline_ ? currentPipeline_->program_ : 0;
        if (program) {
            const char* blockName = ub.blockName ? ub.blockName : "Globals";
            GLint blockIndex = glGetUniformBlockIndex(program, blockName);
            if (blockIndex >= 0) {
                glUniformBlockBinding(program, static_cast<GLuint>(blockIndex), ub.binding);
            }
        }
        glBindBufferBase(0x8A11 /*GL_UNIFORM_BUFFER*/, ub.binding, buf->id_);
    }
}

void GLDevice::updateBuffer(IBuffer* buffer, const void* data, size_t bytes, size_t dstOffset) {
    auto* glb = static_cast<GLBuffer*>(buffer);
    GLenum target = (glb->getUsage() == BufferUsage::Index) ? 0x8893 /*GL_ELEMENT_ARRAY_BUFFER*/ : (glb->getUsage() == BufferUsage::Uniform ? 0x8A11 /*GL_UNIFORM_BUFFER*/ : GL_ARRAY_BUFFER);
    glBindBuffer(target, glb->id_);
    glBufferSubData(target, static_cast<GLintptr>(dstOffset), static_cast<GLsizeiptr>(bytes), data);
}

// Future: bindDescriptorSet implementation for UBOs (glBindBufferBase)

std::unique_ptr<ICommandList> GLDevice::createCommandList() {
    return std::make_unique<GLCommandList>(*this);
}


}


