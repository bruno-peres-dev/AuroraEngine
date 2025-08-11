#include "CameraSystem.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

namespace Aurora::EditorAppNS {

void CameraSystem::handleInput(const Platform::InputState& in, bool allowInput, double dt) {
    if (!allowInput) return;
    const bool altDown = in.keyDown.test(0x12); // VK_MENU
    const bool lmb = in.mouseDown.test((size_t)Platform::MouseButton::Left);
    const bool rmb = in.mouseDown.test((size_t)Platform::MouseButton::Right);
    const bool mmb = in.mouseDown.test((size_t)Platform::MouseButton::Middle);

    if ((altDown && lmb) || rmb) {
        camera_.yaw   -= static_cast<float>(in.mouseDeltaX) * params_.rotateSpeed;
        camera_.pitch -= static_cast<float>(in.mouseDeltaY) * params_.rotateSpeed;
        camera_.pitch = std::clamp(camera_.pitch, -1.5f, 1.5f);
    } else if (altDown && mmb) {
        camera_.target.x -= static_cast<float>(in.mouseDeltaX) * params_.panSpeed * camera_.distance;
        camera_.target.y += static_cast<float>(in.mouseDeltaY) * params_.panSpeed * camera_.distance;
    } else if ((altDown && rmb) || mmb) {
        camera_.distance += static_cast<float>(-in.mouseDeltaY) * params_.dollySpeed * static_cast<float>(dt) * 60.0f;
        if (camera_.distance < params_.minDistance) camera_.distance = params_.minDistance;
    }
    if (in.mouseWheelDelta != 0) {
        camera_.distance *= (in.mouseWheelDelta > 0) ? 0.9f : 1.1f;
        if (camera_.distance < params_.minDistance) camera_.distance = params_.minDistance;
    }
}

void CameraSystem::computeViewProj(uint32_t fbWidth, uint32_t fbHeight, float outView[16], float outProj[16]) const {
    const float cx = camera_.target.x;
    const float cy = camera_.target.y;
    const float cz = camera_.target.z;
    const float ex = cx + camera_.distance * std::cos(camera_.pitch) * std::cos(camera_.yaw);
    const float ey = cy + camera_.distance * std::sin(camera_.pitch);
    const float ez = cz + camera_.distance * std::cos(camera_.pitch) * std::sin(camera_.yaw);
    glm::vec3 eye(ex,ey,ez);
    glm::vec3 ctr(cx,cy,cz);
    glm::vec3 up(0.0f,1.0f,0.0f);
    glm::mat4 view = glm::lookAt(eye, ctr, up);
    float aspect = fbHeight ? (static_cast<float>(fbWidth)/static_cast<float>(fbHeight)) : 1.0f;
    glm::mat4 proj = glm::perspective(glm::radians(60.0f), aspect, 0.01f, 100.0f);
    memcpy(outView, &view[0][0], sizeof(float)*16);
    memcpy(outProj, &proj[0][0], sizeof(float)*16);
}

}


