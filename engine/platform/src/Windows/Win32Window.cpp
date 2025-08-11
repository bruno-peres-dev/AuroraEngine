#include "Win32Window.hpp"
#include "Aurora/Core/Log.hpp"

#ifdef _WIN32
#include <windowsx.h>

namespace Aurora::Platform {

static const wchar_t* kWindowClassName = L"AuroraWin32Window";

static Key mapVkToKey(WPARAM vk, LPARAM /*lparam*/) {
    switch (vk) {
        // Letters
        case 'A': return Key::A; case 'B': return Key::B; case 'C': return Key::C; case 'D': return Key::D;
        case 'E': return Key::E; case 'F': return Key::F; case 'G': return Key::G; case 'H': return Key::H;
        case 'I': return Key::I; case 'J': return Key::J; case 'K': return Key::K; case 'L': return Key::L;
        case 'M': return Key::M; case 'N': return Key::N; case 'O': return Key::O; case 'P': return Key::P;
        case 'Q': return Key::Q; case 'R': return Key::R; case 'S': return Key::S; case 'T': return Key::T;
        case 'U': return Key::U; case 'V': return Key::V; case 'W': return Key::W; case 'X': return Key::X;
        case 'Y': return Key::Y; case 'Z': return Key::Z;
        // Numbers
        case '0': return Key::Num0; case '1': return Key::Num1; case '2': return Key::Num2; case '3': return Key::Num3;
        case '4': return Key::Num4; case '5': return Key::Num5; case '6': return Key::Num6; case '7': return Key::Num7;
        case '8': return Key::Num8; case '9': return Key::Num9;
        // Controls
        case VK_ESCAPE: return Key::Escape;
        case VK_SPACE: return Key::Space;
        case VK_RETURN: return Key::Enter;
        case VK_TAB: return Key::Tab;
        case VK_LSHIFT: return Key::ShiftLeft;
        case VK_RSHIFT: return Key::ShiftRight;
        case VK_LCONTROL: return Key::ControlLeft;
        case VK_RCONTROL: return Key::ControlRight;
        case VK_LMENU: return Key::AltLeft;
        case VK_RMENU: return Key::AltRight;
        // Arrows
        case VK_UP: return Key::ArrowUp;
        case VK_DOWN: return Key::ArrowDown;
        case VK_LEFT: return Key::ArrowLeft;
        case VK_RIGHT: return Key::ArrowRight;
        default: return Key::Unknown;
    }
}

static ATOM ensureWindowClass(HINSTANCE hinst) {
    static ATOM atom = 0;
    if (atom) return atom;
    WNDCLASSW wc{};
    wc.lpfnWndProc = Win32Window::WndProc;
    wc.hInstance = hinst;
    wc.lpszClassName = kWindowClassName;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    atom = RegisterClassW(&wc);
    return atom;
}

Win32Window::Win32Window(const WindowDesc& desc) {
    hinstance_ = GetModuleHandleW(nullptr);
    ensureWindowClass(hinstance_);

    DWORD style = WS_OVERLAPPEDWINDOW;
    RECT rect{0, 0, static_cast<LONG>(desc.width), static_cast<LONG>(desc.height)};
    AdjustWindowRect(&rect, style, FALSE);

    std::wstring title(desc.title.begin(), desc.title.end());
    hwnd_ = CreateWindowExW(
        0, kWindowClassName, title.c_str(), style,
        CW_USEDEFAULT, CW_USEDEFAULT,
        rect.right - rect.left, rect.bottom - rect.top,
        nullptr, nullptr, hinstance_, this);

    if (!hwnd_) {
        Core::log(Core::LogLevel::Error, "Falha ao criar janela Win32");
        running_ = false;
    }
}

Win32Window::~Win32Window() {
    if (hwnd_) DestroyWindow(hwnd_);
}

void Win32Window::show() {
    if (hwnd_) ShowWindow(hwnd_, SW_SHOW);
}

bool Win32Window::pumpEvents() {
    MSG msg;
    while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            running_ = false;
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return running_;
}

void Win32Window::getSize(uint32_t& outWidth, uint32_t& outHeight) const {
    RECT rc{0,0,0,0};
    if (hwnd_) {
        GetClientRect(hwnd_, &rc);
        outWidth = static_cast<uint32_t>(rc.right - rc.left);
        outHeight = static_cast<uint32_t>(rc.bottom - rc.top);
    } else {
        outWidth = outHeight = 0;
    }
}

LRESULT CALLBACK Win32Window::WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    if (msg == WM_NCCREATE) {
        auto cs = reinterpret_cast<CREATESTRUCTW*>(lparam);
        auto* that = static_cast<Win32Window*>(cs->lpCreateParams);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(that));
        that->hwnd_ = hwnd;
    }
    auto* that = reinterpret_cast<Win32Window*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    switch (msg) {
        case WM_CLOSE:
            if (that) {
                Event e{}; e.type = EventType::WindowClose; that->events_.push_back(e);
                that->running_ = false;
            }
            PostQuitMessage(0);
            return 0;
        case WM_SIZE: {
            if (that) {
                Event e{}; e.type = EventType::WindowResize; e.width = LOWORD(lparam); e.height = HIWORD(lparam);
                that->events_.push_back(e);
            }
            return 0;
        }
        case WM_SETFOCUS: {
            if (that) { that->input_.focused = true; Event e{}; e.type = EventType::WindowFocus; that->events_.push_back(e);} return 0;
        }
        case WM_KILLFOCUS: {
            if (that) { that->input_.focused = false; Event e{}; e.type = EventType::WindowLostFocus; that->events_.push_back(e);} return 0;
        }
        case WM_MOUSEMOVE: {
            if (that) {
                int x = GET_X_LPARAM(lparam), y = GET_Y_LPARAM(lparam);
                that->input_.mouseDeltaX += x - that->input_.mouseX;
                that->input_.mouseDeltaY += y - that->input_.mouseY;
                that->input_.mouseX = x; that->input_.mouseY = y;
                Event e{}; e.type = EventType::MouseMove; e.x = x; e.y = y; e.dx = that->input_.mouseDeltaX; e.dy = that->input_.mouseDeltaY;
                that->events_.push_back(e);
            }
            return 0;
        }
        case WM_MOUSEWHEEL: {
            if (that) {
                int delta = GET_WHEEL_DELTA_WPARAM(wparam);
                that->input_.mouseWheelDelta += delta;
                Event e{}; e.type = EventType::MouseWheel; e.wheelDelta = delta; that->events_.push_back(e);
            }
            return 0;
        }
        case WM_LBUTTONDOWN: case WM_RBUTTONDOWN: case WM_MBUTTONDOWN: case WM_XBUTTONDOWN: {
            if (that) {
                MouseButton btn = MouseButton::Unknown;
                if (msg == WM_LBUTTONDOWN) btn = MouseButton::Left; else if (msg == WM_RBUTTONDOWN) btn = MouseButton::Right; else if (msg == WM_MBUTTONDOWN) btn = MouseButton::Middle; else btn = (GET_XBUTTON_WPARAM(wparam) == XBUTTON1) ? MouseButton::X1 : MouseButton::X2;
                size_t idx = static_cast<size_t>(btn);
                that->input_.mouseDown.set(idx); that->input_.mousePressed.set(idx);
                Event e{}; e.type = EventType::MouseDown; e.mouseButton = btn; that->events_.push_back(e);
            }
            return 0;
        }
        case WM_LBUTTONUP: case WM_RBUTTONUP: case WM_MBUTTONUP: case WM_XBUTTONUP: {
            if (that) {
                MouseButton btn = MouseButton::Unknown;
                if (msg == WM_LBUTTONUP) btn = MouseButton::Left; else if (msg == WM_RBUTTONUP) btn = MouseButton::Right; else if (msg == WM_MBUTTONUP) btn = MouseButton::Middle; else btn = (GET_XBUTTON_WPARAM(wparam) == XBUTTON1) ? MouseButton::X1 : MouseButton::X2;
                size_t idx = static_cast<size_t>(btn);
                that->input_.mouseDown.reset(idx); that->input_.mouseReleased.set(idx);
                Event e{}; e.type = EventType::MouseUp; e.mouseButton = btn; that->events_.push_back(e);
            }
            return 0;
        }
        case WM_CHAR: {
            if (that) { Event e{}; e.type = EventType::TextInput; e.character = static_cast<uint32_t>(wparam); that->events_.push_back(e);} return 0;
        }
        case WM_KEYDOWN: case WM_SYSKEYDOWN: {
            if (that) {
                uint32_t vk = static_cast<uint32_t>(wparam);
                size_t idx = vk & 0xFFu;
                if (!that->input_.keyDown.test(idx)) that->input_.keyPressed.set(idx);
                that->input_.keyDown.set(idx);
                Event e{}; e.type = EventType::KeyDown; e.key = mapVkToKey(wparam, lparam); that->events_.push_back(e);
                if (vk == VK_ESCAPE) { that->running_ = false; }
            }
            return 0;
        }
        case WM_KEYUP: case WM_SYSKEYUP: {
            if (that) {
                uint32_t vk = static_cast<uint32_t>(wparam);
                size_t idx = vk & 0xFFu;
                that->input_.keyDown.reset(idx);
                that->input_.keyReleased.set(idx);
                Event e{}; e.type = EventType::KeyUp; e.key = mapVkToKey(wparam, lparam); that->events_.push_back(e);
            }
            return 0;
        }
        default:
            break;
    }
    return DefWindowProcW(hwnd, msg, wparam, lparam);
}

}

#endif


