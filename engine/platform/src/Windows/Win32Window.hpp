#pragma once

#include "Aurora/Platform/Window.hpp"

#ifdef _WIN32
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#  include <vector>
#  include "Aurora/Platform/Input.hpp"

namespace Aurora::Platform {

class Win32Window final : public IWindow {
public:
    explicit Win32Window(const WindowDesc& desc);
    ~Win32Window() override;

    void show() override;
    bool pumpEvents() override;
    void* getNativeHandle() const override { return hwnd_; }
    void getSize(uint32_t& outWidth, uint32_t& outHeight) const override;
    const std::vector<Event>& getEventQueue() const override { return events_; }
    void clearEventQueue() override { events_.clear(); input_.mouseDeltaX = input_.mouseDeltaY = input_.mouseWheelDelta = 0; input_.keyPressed.reset(); input_.keyReleased.reset(); input_.mousePressed.reset(); input_.mouseReleased.reset(); }
    const InputState& getInputState() const override { return input_; }

    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
    HWND hwnd_{};
    HINSTANCE hinstance_{};
    bool running_{true};
    std::vector<Event> events_{};
    InputState input_{};
};

}

#endif


