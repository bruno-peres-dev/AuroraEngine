#pragma once

#include "Aurora/RHI/RHI.hpp"

namespace Aurora::RHI {

class GLSampler final : public ISampler {
public:
    explicit GLSampler(unsigned int id) : id_(id) {}
    ~GLSampler() override;
    unsigned int id_{0};
};

}


