#pragma once

#include "Aurora/RHI/RHI.hpp"

namespace Aurora::RHI {

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

}


