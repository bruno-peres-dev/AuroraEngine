#include "GLDevice.hpp"
#include "Aurora/Core/Log.hpp"
#include "GLState.hpp"
#include "GLRenderPass.hpp"
#include "GLSwapchain.hpp"
#include "GLShaderModule.hpp"
#include "GLBuffer.hpp"
#include "GLGraphicsPipeline.hpp"
#include "GLDescriptorSet.hpp"
#include "GLTexture.hpp"
#include "GLSampler.hpp"
#include "GLCapabilities.hpp"

#include <glad/glad.h>

namespace Aurora::RHI {

#ifdef AURORA_DEBUG
static void glCheckError(const char* where) {
    GLenum err = glGetError();
    if (err != 0) {
        Core::log(Core::LogLevel::Error, std::string("GL error [") + where + "]: 0x" + std::to_string(static_cast<unsigned int>(err)));
    }
}
#define GL_CHECK(where) glCheckError(where)
#else
#define GL_CHECK(where) ((void)0)
#endif

using namespace Aurora::RHI::GLState;

// Destruidores das classes específicas agora residem em seus próprios arquivos

std::unique_ptr<ISwapchain> GLDevice::createSwapchain(const SwapchainDesc& desc) {
    auto sc = std::make_unique<GLSwapchain>(desc.width, desc.height);
#ifdef _WIN32
    HWND hwnd = static_cast<HWND>(desc.windowHandle);
    if (!sc->context_.initialize(hwnd, desc.vsync)) {
        Core::log(Core::LogLevel::Error, "Falha ao inicializar contexto WGL");
        return nullptr;
    }
    // Detect capabilities após criação de contexto e carregamento do glad
    auto glcaps = GLCapabilities::query();
    caps_.supportsGLSL420 = glcaps.supportsGLSL420;
    caps_.hasShadingLanguage420Pack = glcaps.hasShadingLanguage420Pack;
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
// FBO tokens (fallback para headers legados)
#ifndef GL_FRAMEBUFFER
#define GL_FRAMEBUFFER 0x8D40
#endif
#ifndef GL_COLOR_ATTACHMENT0
#define GL_COLOR_ATTACHMENT0 0x8CE0
#endif
#ifndef GL_DEPTH_ATTACHMENT
#define GL_DEPTH_ATTACHMENT 0x8D00
#endif
#ifndef GL_DEPTH_STENCIL_ATTACHMENT
#define GL_DEPTH_STENCIL_ATTACHMENT 0x821A
#endif
#ifndef GL_FRAMEBUFFER_COMPLETE
#define GL_FRAMEBUFFER_COMPLETE 0x8CD5
#endif

void GLDevice::beginRenderPass(IRenderPass* renderPass, ISwapchain* target) {
    auto* rp = static_cast<GLRenderPass*>(renderPass);

    // Se attachments foram especificados, configuramos um FBO temporário (MVP)
    bool useFBO = false;
    unsigned int viewportW = target ? target->getWidth() : 0;
    unsigned int viewportH = target ? target->getHeight() : 0;

    if (!rp->desc_.colorAttachments.empty() || rp->desc_.depthAttachment.texture) {
        glGenFramebuffers(1, &currentFBO_);
        glBindFramebuffer(GL_FRAMEBUFFER, currentFBO_);
        tempFBOCreated_ = true;
        useFBO = true;

        // Attach colors
        std::vector<unsigned int> drawBuffers;
        drawBuffers.reserve(rp->desc_.colorAttachments.size());
        for (size_t i = 0; i < rp->desc_.colorAttachments.size(); ++i) {
            const auto& a = rp->desc_.colorAttachments[i];
            if (!a.texture) continue;
            auto* gltex = static_cast<GLTexture*>(a.texture);
            glFramebufferTexture2D(GL_FRAMEBUFFER, static_cast<unsigned int>(GL_COLOR_ATTACHMENT0 + i), 0x0DE1 /*GL_TEXTURE_2D*/, gltex->id_, static_cast<int>(a.mipLevel));
            drawBuffers.push_back(static_cast<unsigned int>(GL_COLOR_ATTACHMENT0 + i));
            if (viewportW == 0 || viewportH == 0) {
                auto td = gltex->getDesc();
                viewportW = td.width >> a.mipLevel; if (viewportW == 0) viewportW = 1;
                viewportH = td.height >> a.mipLevel; if (viewportH == 0) viewportH = 1;
            }
        }
        if (!drawBuffers.empty()) {
            glDrawBuffers(static_cast<int>(drawBuffers.size()), reinterpret_cast<const unsigned int*>(drawBuffers.data()));
        } else {
            // depth-only: desabilita draw buffers
            glDrawBuffer(0);
        }

        // Attach depth
        if (rp->desc_.depthAttachment.texture) {
            auto* gltex = static_cast<GLTexture*>(rp->desc_.depthAttachment.texture);
            auto td = gltex->getDesc();
            unsigned int attachment = (td.format == TextureFormat::Depth24Stencil8) ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT;
            glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, 0x0DE1 /*GL_TEXTURE_2D*/, gltex->id_, static_cast<int>(rp->desc_.depthAttachment.mipLevel));
            if (viewportW == 0 || viewportH == 0) {
                viewportW = td.width >> rp->desc_.depthAttachment.mipLevel; if (viewportW == 0) viewportW = 1;
                viewportH = td.height >> rp->desc_.depthAttachment.mipLevel; if (viewportH == 0) viewportH = 1;
            }
        }

        unsigned int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            Core::log(Core::LogLevel::Error, "FBO incompleto");
        }
    } else {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    // Viewport
    if (viewportW == 0 || viewportH == 0) {
        viewportW = target ? target->getWidth() : 0;
        viewportH = target ? target->getHeight() : 0;
    }
    glViewport(0, 0, static_cast<GLsizei>(viewportW), static_cast<GLsizei>(viewportH));
    // Sempre garantir estado de limpeza consistente: habilita teste de depth e mascara
    glEnable(0x0B71 /*GL_DEPTH_TEST*/);
    glDepthMask(1);
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
    if (tempFBOCreated_) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDeleteFramebuffers(1, &currentFBO_);
        currentFBO_ = 0;
        tempFBOCreated_ = false;
    }
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
    return std::make_unique<GLGraphicsPipeline>(program, vao, desc.vertexLayout, desc.state);
}

void GLDevice::setGraphicsPipeline(IGraphicsPipeline* pipeline) {
    currentPipeline_ = static_cast<GLGraphicsPipeline*>(pipeline);
    glUseProgram(currentPipeline_->program_);
    glBindVertexArray(currentPipeline_->vao_);
    // Aplicar estado de raster/blend/depth do pipeline atual
    applyPipelineState(currentPipeline_->state_);
}

void GLDevice::setVertexBuffer(IBuffer* buffer) {
    auto* glb = static_cast<GLBuffer*>(buffer);
    glBindBuffer(GL_ARRAY_BUFFER, glb->id_);
    currentVertexBuffer_ = glb;
    // Re-aplicar ponteiros de atributo para garantir estado correto com este VBO
    if (currentPipeline_) {
        for (const auto& a : currentPipeline_->layout_.attributes) {
            glVertexAttribPointer(a.location, a.components, GL_FLOAT, GL_FALSE, static_cast<GLsizei>(currentPipeline_->layout_.stride), reinterpret_cast<const void*>(static_cast<uintptr_t>(a.offset)));
            glEnableVertexAttribArray(a.location);
        }
    }
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

void GLDevice::drawIndexed(uint32_t indexCount, uint32_t firstIndex, IndexType indexType) {
    (void)firstIndex; // not supporting offset for now
    GLenum glType = (indexType == IndexType::Uint16) ? 0x1403 /*GL_UNSIGNED_SHORT*/ : 0x1405 /*GL_UNSIGNED_INT*/;
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indexCount), glType, nullptr);
}

void GLDevice::bindDescriptorSet(IDescriptorSet* set) {
    auto* glset = static_cast<GLDescriptorSet*>(set);
    for (const auto& ub : glset->desc.uniformBuffers) {
        auto* buf = static_cast<GLBuffer*>(ub.buffer);
        // Se o shader não especifica layout(binding), associamos bloco ao binding com glUniformBlockBinding
        GLuint program = currentPipeline_ ? currentPipeline_->program_ : 0;
        if (program) {
            const char* blockName = ub.blockName ? ub.blockName : "Globals";
            // Cache lookup para blockIndex
            GLint blockIndex = -1;
            auto& blockMap = programToUniformBlockIndexCache_[program];
            auto itBlock = blockMap.find(blockName);
            if (itBlock != blockMap.end()) {
                blockIndex = itBlock->second;
            } else {
                blockIndex = glGetUniformBlockIndex(program, blockName);
                blockMap[blockName] = blockIndex;
            }
            if (blockIndex >= 0) {
                unsigned long long key = (static_cast<unsigned long long>(program) << 32) | static_cast<unsigned long long>(blockIndex);
                auto itApplied = uniformBlockBindingApplied_.find(key);
                if (itApplied == uniformBlockBindingApplied_.end() || itApplied->second != ub.binding) {
                    glUniformBlockBinding(program, static_cast<GLuint>(blockIndex), ub.binding);
                    uniformBlockBindingApplied_[key] = ub.binding;
                }
            }
        }
        glBindBufferBase(0x8A11 /*GL_UNIFORM_BUFFER*/, ub.binding, buf->id_);
    }

    // Bind sampled textures
    for (const auto& st : glset->desc.sampledTextures) {
        auto* tex = static_cast<GLTexture*>(st.texture);
        auto* smp = static_cast<GLSampler*>(st.sampler);
        if (!tex || !smp) continue;
        // Resolve uniform sampler location if a name is provided
        if (currentPipeline_ && st.uniformName) {
            unsigned int program = currentPipeline_->program_;
            auto& locMap = programToUniformSamplerLocationCache_[program];
            int loc = -1;
            auto itLoc = locMap.find(st.uniformName);
            if (itLoc != locMap.end()) loc = itLoc->second; else { loc = glGetUniformLocation(program, st.uniformName); locMap[st.uniformName] = loc; }
            if (loc >= 0) {
                unsigned long long key = (static_cast<unsigned long long>(program) << 32) | static_cast<unsigned long long>(loc);
                auto itSet = samplerUniformApplied_.find(key);
                if (itSet == samplerUniformApplied_.end() || itSet->second != static_cast<int>(st.binding)) {
                    glUniform1i(loc, static_cast<int>(st.binding));
                    samplerUniformApplied_[key] = static_cast<int>(st.binding);
                }
            }
        }
        // Activate texture unit == binding, bind texture and sampler
        glActiveTexture(0x84C0 /*GL_TEXTURE0*/ + st.binding);
        glBindTexture(0x0DE1 /*GL_TEXTURE_2D*/, tex->id_);
        glBindSampler(st.binding, smp->id_);
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

void GLDevice::submit(ICommandList* list) {
    auto* gl = static_cast<GLCommandList*>(list);
    for (auto& op : gl->operations_) op();
}

std::unique_ptr<ITexture> GLDevice::createTexture(const TextureDesc& desc, const void* initialPixelsRGBA8) {
    // Define tokens ausentes caso necessário
    #ifndef GL_TEXTURE_2D
    #define GL_TEXTURE_2D 0x0DE1
    #endif
    #ifndef GL_UNPACK_ALIGNMENT
    #define GL_UNPACK_ALIGNMENT 0x0CF5
    #endif
    #ifndef GL_RGBA8
    #define GL_RGBA8 0x8058
    #endif
    #ifndef GL_RGB8
    #define GL_RGB8 0x8051
    #endif
    #ifndef GL_R8
    #define GL_R8 0x8229
    #endif
    #ifndef GL_RGBA16F
    #define GL_RGBA16F 0x881A
    #endif
    #ifndef GL_R16F
    #define GL_R16F 0x822D
    #endif
    #ifndef GL_DEPTH24_STENCIL8
    #define GL_DEPTH24_STENCIL8 0x88F0
    #endif
    #ifndef GL_DEPTH_COMPONENT32F
    #define GL_DEPTH_COMPONENT32F 0x8CAC
    #endif
    #ifndef GL_DEPTH_STENCIL
    #define GL_DEPTH_STENCIL 0x84F9
    #endif
    #ifndef GL_UNSIGNED_INT_24_8
    #define GL_UNSIGNED_INT_24_8 0x84FA
    #endif
    #ifndef GL_DEPTH_COMPONENT
    #define GL_DEPTH_COMPONENT 0x1902
    #endif
    #ifndef GL_HALF_FLOAT
    #define GL_HALF_FLOAT 0x140B
    #endif
    #ifndef GL_LINEAR
    #define GL_LINEAR 0x2601
    #endif
    #ifndef GL_NEAREST
    #define GL_NEAREST 0x2600
    #endif
    #ifndef GL_CLAMP_TO_EDGE
    #define GL_CLAMP_TO_EDGE 0x812F
    #endif

    unsigned int id = 0;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    // Parametrização padrão
    glTexParameteri(GL_TEXTURE_2D, 0x2801 /*GL_TEXTURE_MIN_FILTER*/, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, 0x2800 /*GL_TEXTURE_MAG_FILTER*/, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, 0x2802 /*GL_TEXTURE_WRAP_S*/, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, 0x2803 /*GL_TEXTURE_WRAP_T*/, GL_CLAMP_TO_EDGE);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    int internal = GL_RGBA8;
    int format = 0x1908 /*GL_RGBA*/;
    int type = 0x1401 /*GL_UNSIGNED_BYTE*/;

    switch (desc.format) {
        case TextureFormat::RGBA8: internal = GL_RGBA8; format = 0x1908 /*GL_RGBA*/; type = 0x1401 /*GL_UNSIGNED_BYTE*/; break;
        case TextureFormat::RGB8: internal = GL_RGB8; format = 0x1907 /*GL_RGB*/; type = 0x1401 /*GL_UNSIGNED_BYTE*/; break;
        case TextureFormat::R8: internal = GL_R8; format = 0x1903 /*GL_RED*/; type = 0x1401 /*GL_UNSIGNED_BYTE*/; break;
        case TextureFormat::RGBA16F: internal = GL_RGBA16F; format = 0x1908 /*GL_RGBA*/; type = GL_HALF_FLOAT; break;
        case TextureFormat::R16F: internal = GL_R16F; format = 0x1903 /*GL_RED*/; type = GL_HALF_FLOAT; break;
        case TextureFormat::Depth24Stencil8: internal = GL_DEPTH24_STENCIL8; format = GL_DEPTH_STENCIL; type = GL_UNSIGNED_INT_24_8; break;
        case TextureFormat::Depth32F: internal = GL_DEPTH_COMPONENT32F; format = GL_DEPTH_COMPONENT; type = 0x1406 /*GL_FLOAT*/; break;
        default: break;
    }

    const void* data = initialPixelsRGBA8;
    if (desc.format == TextureFormat::Depth24Stencil8 || desc.format == TextureFormat::Depth32F) {
        data = nullptr; // sem upload inicial para depth
    }

    glTexImage2D(GL_TEXTURE_2D, 0, internal, static_cast<int>(desc.width), static_cast<int>(desc.height), 0, format, type, data);
    if (desc.mipLevels > 1 && (desc.format == TextureFormat::RGBA8 || desc.format == TextureFormat::RGB8 || desc.format == TextureFormat::R8 || desc.format == TextureFormat::RGBA16F || desc.format == TextureFormat::R16F)) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    return std::make_unique<GLTexture>(desc, id);
}

std::unique_ptr<ISampler> GLDevice::createSampler(const SamplerDesc& desc) {
    unsigned int id = 0; glGenSamplers(1, &id);
    int minf = (desc.minFilter == FilterMode::Linear) ? 0x2601 /*GL_LINEAR*/ : 0x2600 /*GL_NEAREST*/;
    int magf = (desc.magFilter == FilterMode::Linear) ? 0x2601 /*GL_LINEAR*/ : 0x2600 /*GL_NEAREST*/;
    int wrapU = (desc.addressU == AddressMode::Repeat) ? 0x2901 /*GL_REPEAT*/ : 0x812F /*GL_CLAMP_TO_EDGE*/;
    int wrapV = (desc.addressV == AddressMode::Repeat) ? 0x2901 /*GL_REPEAT*/ : 0x812F /*GL_CLAMP_TO_EDGE*/;
    // Mipmap mode
    int minFilterWithMips = minf;
    if (desc.mipmapMode == SamplerDesc::MipmapMode::Nearest) {
        minFilterWithMips = (minf == 0x2601 /*GL_LINEAR*/) ? 0x2701 /*GL_LINEAR_MIPMAP_NEAREST*/ : 0x2700 /*GL_NEAREST_MIPMAP_NEAREST*/;
    } else if (desc.mipmapMode == SamplerDesc::MipmapMode::Linear) {
        minFilterWithMips = (minf == 0x2601 /*GL_LINEAR*/) ? 0x2703 /*GL_LINEAR_MIPMAP_LINEAR*/ : 0x2702 /*GL_NEAREST_MIPMAP_LINEAR*/;
    }
    glSamplerParameteri(id, 0x2801 /*GL_TEXTURE_MIN_FILTER*/, minFilterWithMips);
    glSamplerParameteri(id, 0x2800 /*GL_TEXTURE_MAG_FILTER*/, magf);
    glSamplerParameteri(id, 0x2802 /*GL_TEXTURE_WRAP_S*/, wrapU);
    glSamplerParameteri(id, 0x2803 /*GL_TEXTURE_WRAP_T*/, wrapV);
    return std::make_unique<GLSampler>(id);
}

void GLDevice::setDebugWireframe(bool enable) {
    // Algumas implementações core profile desabilitam polygon mode por face
    // Consulta o profile mask uma única vez
    #ifndef GL_CONTEXT_PROFILE_MASK
    #define GL_CONTEXT_PROFILE_MASK 0x9126
    #endif
    #ifndef GL_CONTEXT_COMPATIBILITY_PROFILE_BIT
    #define GL_CONTEXT_COMPATIBILITY_PROFILE_BIT 0x00000002
    #endif
    static int s_profileMask = -1;
    if (s_profileMask == -1) {
        glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &s_profileMask);
    }
    if (s_profileMask & GL_CONTEXT_COMPATIBILITY_PROFILE_BIT) {
        glPolygonMode(0x0404 /*GL_FRONT_AND_BACK*/, enable ? 0x1B01 /*GL_LINE*/ : 0x1B02 /*GL_FILL*/);
    } else {
        Core::log(Core::LogLevel::Warn, "Wireframe indisponível no Core Profile; ignorando toggle");
    }
}


}


