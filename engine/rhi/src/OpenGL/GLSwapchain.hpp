#pragma once

#include "Aurora/RHI/RHI.hpp"
#ifdef _WIN32
#  include "WGLContext.hpp"
#endif

namespace Aurora::RHI {

class GLSwapchain final : public ISwapchain {
public:
    explicit GLSwapchain(uint32_t w, uint32_t h) : width_(w), height_(h) {}
    void present() override;
    void resize(uint32_t width, uint32_t height) override { width_ = width; height_ = height; }
    uint32_t getWidth() const override { return width_; }
    uint32_t getHeight() const override { return height_; }
    void setVsync(bool enabled) override {
#ifdef _WIN32
        context_.setVsync(enabled);
#else
        (void)enabled;
#endif
    }

#ifdef _WIN32
    WGLContext context_{};
#endif
private:
    uint32_t width_{0};
    uint32_t height_{0};
};

}


