#include "RenderSystem.hpp"

namespace Aurora::EditorAppNS {

void RenderSystem::ensureViewport(ViewportResources& vp, uint32_t width, uint32_t height) {
    if (vp.width == width && vp.height == height && vp.color && vp.depth && vp.renderPass) return;
    vp.width = width; vp.height = height;
    RHI::TextureDesc td{}; td.width = width; td.height = height; td.format = RHI::TextureFormat::RGBA8; td.usage = RHI::TextureUsage::RenderTarget; td.mipLevels = 1;
    vp.color = device_.createTexture(td, nullptr);
    RHI::TextureDesc dd{}; dd.width = width; dd.height = height; dd.format = RHI::TextureFormat::Depth24Stencil8; dd.usage = RHI::TextureUsage::DepthStencil;
    vp.depth = device_.createTexture(dd, nullptr);
    RHI::RenderPassDesc rp{}; rp.clearColor[0]=0.05f; rp.clearColor[1]=0.05f; rp.clearColor[2]=0.08f; rp.clearColor[3]=1.0f;
    rp.colorAttachments = { { vp.color.get(), 0 } };
    rp.depthAttachment = { vp.depth.get(), 0 };
    vp.renderPass = device_.createRenderPass(rp);
}

}


