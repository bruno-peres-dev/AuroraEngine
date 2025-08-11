#pragma once

#include "Aurora/RHI/RHI.hpp"

namespace Aurora::RHI {

class GLTexture final : public ITexture {
public:
    GLTexture(const TextureDesc& d, unsigned int id) : desc_(d), id_(id) {}
    ~GLTexture() override;
    TextureDesc getDesc() const override { return desc_; }
    unsigned int id_{0};
private:
    TextureDesc desc_{};
};

}


