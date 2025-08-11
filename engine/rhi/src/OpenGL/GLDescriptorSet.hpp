#pragma once

#include "Aurora/RHI/RHI.hpp"

namespace Aurora::RHI {

class GLDescriptorSet final : public IDescriptorSet {
public:
    explicit GLDescriptorSet(const DescriptorSetDesc& d) : desc(d) {}
    DescriptorSetDesc desc;
};

}


