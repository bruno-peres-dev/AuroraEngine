#pragma once

#include <cstdint>
#include <string_view>
#include <functional>
#include <vector>
#include "Input.hpp"

namespace Aurora::Platform {

struct WindowDesc {
    uint32_t width{1280};
    uint32_t height{720};
    std::string_view title{"Aurora"};
};

class IWindow {
public:
    virtual ~IWindow() = default;
    virtual void show() = 0;
    virtual bool pumpEvents() = 0; // returns false when quitting
    virtual void* getNativeHandle() const = 0;
    virtual void getSize(uint32_t& outWidth, uint32_t& outHeight) const = 0;
    virtual const std::vector<Event>& getEventQueue() const = 0;
    virtual void clearEventQueue() = 0;
    virtual const InputState& getInputState() const = 0;
};

IWindow* createWindow(const WindowDesc& desc);
void destroyWindow(IWindow* window);

#ifdef _WIN32
// Hook opcional para encaminhar mensagens Win32 (útil para Dear ImGui)
using Win32MsgHook = bool(*)(void* hwnd, unsigned int msg, unsigned long long wparam, long long lparam);
void setWin32WndProcHook(Win32MsgHook hook);
#endif

}


