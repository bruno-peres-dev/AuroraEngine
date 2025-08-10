#pragma once

#include <cstdint>
#include <bitset>

namespace Aurora::Platform {

enum class Key : uint16_t {
    Unknown = 0,
    A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
    Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,
    Escape, Space, Enter, Tab,
    ShiftLeft, ShiftRight, ControlLeft, ControlRight, AltLeft, AltRight,
    ArrowUp, ArrowDown, ArrowLeft, ArrowRight
};

enum class MouseButton : uint8_t {
    Left = 0, Right = 1, Middle = 2, X1 = 3, X2 = 4, Unknown = 255
};

enum class EventType : uint8_t {
    KeyDown,
    KeyUp,
    TextInput,
    MouseMove,
    MouseDown,
    MouseUp,
    MouseWheel,
    WindowResize,
    WindowClose,
    WindowFocus,
    WindowLostFocus
};

struct Event {
    EventType type{};
    Key key{Key::Unknown};
    MouseButton mouseButton{MouseButton::Unknown};
    uint32_t character{0};
    int x{0}, y{0};
    int dx{0}, dy{0};
    int wheelDelta{0};
    uint32_t width{0}, height{0};
};

struct InputState {
    std::bitset<256> keyDown;
    std::bitset<256> keyPressed;
    std::bitset<256> keyReleased;

    std::bitset<8> mouseDown;
    std::bitset<8> mousePressed;
    std::bitset<8> mouseReleased;

    int mouseX{0}, mouseY{0};
    int mouseDeltaX{0}, mouseDeltaY{0};
    int mouseWheelDelta{0};
    bool focused{true};
};

}


