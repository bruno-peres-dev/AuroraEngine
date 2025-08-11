#pragma once

#include <cstdint>
#include <glm/glm.hpp>

#include "Aurora/Platform/Input.hpp"

namespace Aurora::EditorAppNS {

struct Camera {
    float yaw{0.0f};
    float pitch{0.3f};
    float distance{3.0f};
    glm::vec3 target{0.0f, 0.0f, 0.0f};
};

struct OrbitCameraParams {
    float rotateSpeed{0.005f};
    float panSpeed{0.002f};
    float dollySpeed{0.5f};
    float minDistance{0.2f};
};

class CameraSystem {
public:
    CameraSystem() = default;

    void handleInput(const Platform::InputState& in, bool allowInput, double dt);
    void computeViewProj(uint32_t fbWidth, uint32_t fbHeight, float outView[16], float outProj[16]) const;

    Camera& camera() { return camera_; }
    const Camera& camera() const { return camera_; }
    OrbitCameraParams& params() { return params_; }
    const OrbitCameraParams& params() const { return params_; }

private:
    Camera camera_{};
    OrbitCameraParams params_{};
};

}


