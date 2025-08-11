#pragma once

#include "Aurora/RHI/RHI.hpp"

namespace Aurora::RHI {

class GLShaderModule final : public IShaderModule {
public:
    GLShaderModule(ShaderStage stage, unsigned int id) : stage_(stage), id_(id) {}
    ~GLShaderModule() override;
    ShaderStage getStage() const override { return stage_; }
    unsigned int id_{0};
private:
    ShaderStage stage_;
};

}


