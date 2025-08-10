#include "Aurora/RHI/RHI.hpp"

#include "Null/NullDevice.hpp"
#include "OpenGL/GLDevice.hpp"

namespace Aurora::RHI {

std::unique_ptr<IDevice> createDevice(BackendType type) {
    switch (type) {
        case BackendType::Null:
            return std::make_unique<NullDevice>();
        case BackendType::OpenGL:
            return std::make_unique<GLDevice>();
        default:
            return std::make_unique<NullDevice>();
    }
}

}


