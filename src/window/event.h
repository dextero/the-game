#pragma once

#ifdef PLATFORM_LINUX
# include <X11/X.h>
# include <X11/keysym.h>

namespace sb {

namespace Mouse {
    enum class Button
    {
        Left = Button1,
        Right = Button3,
        Middle = Button2,
        // uh, these two are actually wheel motions
        X1 = Button4,
        X2 = Button5
    };
} // namespace Mouse

enum class Key
{
    Backspace = XK_BackSpace,
    Esc = XK_Escape,
    Enter = XK_Return,
    CapsLock = XK_Caps_Lock,
    ScrollLock = XK_Scroll_Lock,
    NumLock = XK_Num_Lock,
    Delete = XK_Delete,
    Insert = XK_Insert,
    Pause = XK_Pause,
    PrintScreen = XK_Print,  // TODO: check
    //Sleep = XK_Sleep,      // TODO: find
    Space = XK_space,        // TODO: check
    PageUp = XK_Prior,
    PageDown = XK_Next,
    Home = XK_Home,
    End = XK_End,

    Comma = XK_comma,            // ,<
    Period = XK_period,          // .>
    Equal = XK_equal,            // +=
    Minus = XK_minus,            // -_
    Colon = XK_semicolon,        // ;:
    Slash = XK_slash,            // /?
    Tilde = XK_dead_grave,       // `~
    LBracket = XK_bracketleft,   // [{
    Backslash = XK_backslash,    // \|
    RBracket = XK_bracketright,  // ]}
    Apostrophe = XK_dead_acute,  // '"

    LCtrl = XK_Control_L,
    LAlt = XK_Alt_L,
    LShift = XK_Shift_L,
    LWin = XK_Super_L,    // TODO: check
    RCtrl = XK_Control_R,
    RAlt = XK_Alt_R,
    RShift = XK_Shift_R,
    RWin = XK_Super_R,    // TODO: check

    ArrowDown = XK_Down,
    ArrowRight = XK_Right,
    ArrowLeft = XK_Left,
    ArrowUp = XK_Up,

    F1 = XK_F1,
    F2 = XK_F2,
    F3 = XK_F3,
    F4 = XK_F4,
    F5 = XK_F5,
    F6 = XK_F6,
    F7 = XK_F7,
    F8 = XK_F8,
    F9 = XK_F9,
    F10 = XK_F10,
    F11 = XK_F11,
    F12 = XK_F12,

    Numpad0 = XK_KP_0,
    Numpad1 = XK_KP_1,
    Numpad2 = XK_KP_2,
    Numpad3 = XK_KP_3,
    Numpad4 = XK_KP_4,
    Numpad5 = XK_KP_5,
    Numpad6 = XK_KP_6,
    Numpad7 = XK_KP_7,
    Numpad8 = XK_KP_8,
    Numpad9 = XK_KP_9,
    NumpadDecimal = XK_KP_Separator,
    NumpadMultiply = XK_KP_Multiply,
    NumpadDivide = XK_KP_Divide,
    NumpadSubtract = XK_KP_Subtract,
    NumpadAdd = XK_KP_Add,

    Num0 = XK_0,
    Num1 = XK_1,
    Num2 = XK_2,
    Num3 = XK_3,
    Num4 = XK_4,
    Num5 = XK_5,
    Num6 = XK_6,
    Num7 = XK_7,
    Num8 = XK_8,
    Num9 = XK_9,
    A = XK_a,
    B = XK_b,
    C = XK_c,
    D = XK_d,
    E = XK_e,
    F = XK_f,
    G = XK_g,
    H = XK_h,
    I = XK_i,
    J = XK_j,
    K = XK_k,
    L = XK_l,
    M = XK_m,
    N = XK_n,
    O = XK_o,
    P = XK_p,
    Q = XK_q,
    R = XK_r,
    S = XK_s,
    T = XK_t,
    U = XK_u,
    V = XK_v,
    W = XK_w,
    X = XK_x,
    Y = XK_y,
    Z = XK_z
};

#elif PLATFORM_WINDOWS
# include <WinUser.h>

namespace sb
{
namespace Mouse
{
    enum class Button {
        ButtonLeft = VK_LBUTTON,
        ButtonRight = VK_RBUTTON,
        ButtonMiddle = VK_MBUTTON,
        ButtonX1 = VK_XBUTTON1,
        ButtonX2 = VK_XBUTTON2
    };
}

enum class Key
{
    Backspace = VK_BACK,
    Esc = VK_ESCAPE,
    Enter = VK_RETURN,
    CapsLock = VK_CAPITAL,
    ScrollLock = VK_SCROLL,
    NumLock = VK_NUMLOCK,
    Delete = VK_DELETE,
    Insert = VK_INSERT,
    Pause = VK_PAUSE,
    PrintScreen = VK_SNAPSHOT,
    Sleep = VK_SLEEP,
    Space = VK_SPACE,
    PageUp = VK_PRIOR,
    PageDown = VK_NEXT,
    Home = VK_HOME,
    End = VK_END,

    Comma = VK_OEM_COMMA,   // ,<
    Period = VK_OEM_PERIOD, // .>
    Equal = VK_OEM_PLUS,    // +=
    Minus = VK_OEM_MINUS,   // -_
    Colon = VK_OEM_1,       // ;:
    Slash = VK_OEM_2,       // /?
    Tilde = VK_OEM_3,       // `~
    LBracket = VK_OEM_4,    // [{
    Backslash = VK_OEM_5,   // \|
    RBracket = VK_OEM_6,    // ]}
    Apostrophe = VK_OEM_7,  // '"

    LCtrl = VK_LCONTROL,
    LAlt = VK_LMENU,
    LShift = VK_LSHIFT,
    LWin = VK_LWIN,
    RCtrl = VK_RCONTROL,
    RAlt = VK_RMENU,
    RShift = VK_RSHIFT,
    RWin = VK_RWIN,

    ArrowDown = VK_DOWN,
    ArrowRight = VK_RIGHT,
    ArrowLeft = VK_LEFT,
    ArrowUp = VK_UP,

    F1 = VK_F1,
    F2 = VK_F2,
    F3 = VK_F3,
    F4 = VK_F4,
    F5 = VK_F5,
    F6 = VK_F6,
    F7 = VK_F7,
    F8 = VK_F8,
    F9 = VK_F9,
    F10 = VK_F10,
    F11 = VK_F11,
    F12 = VK_F12,

    Numpad0 = VK_NUMPAD0,
    Numpad1 = VK_NUMPAD1,
    Numpad2 = VK_NUMPAD2,
    Numpad3 = VK_NUMPAD3,
    Numpad4 = VK_NUMPAD4,
    Numpad5 = VK_NUMPAD5,
    Numpad6 = VK_NUMPAD6,
    Numpad7 = VK_NUMPAD7,
    Numpad8 = VK_NUMPAD8,
    Numpad9 = VK_NUMPAD9,
    NumpadDecimal = VK_DECIMAL,
    NumpadMultiply = VK_MULTIPLY,
    NumpadDivide = VK_DIVIDE,
    NumpadSubtract = VK_SUBTRACT,
    NumpadAdd = VK_ADD,

    Num0 = '0',
    Num1 = '1',
    Num2 = '2',
    Num3 = '3',
    Num4 = '4',
    Num5 = '5',
    Num6 = '6',
    Num7 = '7',
    Num8 = '8',
    Num9 = '9',
    A = 'A',
    B = 'B',
    C = 'C',
    D = 'D',
    E = 'E',
    F = 'F',
    G = 'G',
    H = 'H',
    I = 'I',
    J = 'J',
    K = 'K',
    L = 'L',
    M = 'M',
    N = 'N',
    O = 'O',
    P = 'P',
    Q = 'Q',
    R = 'R',
    S = 'S',
    T = 'T',
    U = 'U',
    V = 'V',
    W = 'W',
    X = 'X',
    Y = 'Y',
    Z = 'Z'
};

#endif // PLATFORM_*

class Event
{
public:
    enum Type {
        Invalid,

        MouseMoved,
        MousePressed,
        MouseReleased,
        MouseWheel,

        KeyPressed,
        KeyReleased,

        WindowResized,
        WindowFocus,
        WindowClosed
    } type;

    union {
        struct MouseButtonEvent {
            unsigned x;
            unsigned y;
            Mouse::Button button;
        } mouse;

        int mouseWheelDelta;
        Key key;

        struct WindowResize {
            unsigned width;
            unsigned height;
        } wndResize;

        bool focus;
    } data;

    Event():
        type(Invalid),
        data()
    {}

    static Event mouseMovedEvent(unsigned x,
                                 unsigned y)
    {
        return Event(MouseMoved, x, y);
    }
    static Event mousePressedEvent(unsigned x,
                                   unsigned y,
                                   Mouse::Button btn)
    {
        return Event(MousePressed, btn, x, y);
    }
    static Event mouseReleasedEvent(unsigned x,
                                    unsigned y,
                                    Mouse::Button btn)
    {
        return Event(MouseReleased, btn, x, y);
    }
    static Event mouseWheelEvent(int delta)
    {
        return Event(delta);
    }
    static Event keyPressedEvent(Key code)
    {
        return Event(code, true);
    }
    static Event keyReleasedEvent(Key code)
    {
        return Event(code, false);
    }
    static Event windowResizedEvent(unsigned x,
                                    unsigned y)
    {
        return Event(WindowResized, x, y);
    }
    static Event windowFocusEvent(bool focus)
    {
        return Event(focus);
    }
    static Event windowClosedEvent()
    {
        return Event(WindowClosed);
    }

private:
    Event(Type type,
          Mouse::Button btn,
          unsigned x,
          unsigned y):
        type(type),
        data()
    {
        assert(type == MousePressed
                || type == MouseReleased);

        data.mouse.button = btn;
        data.mouse.x = x;
        data.mouse.y = y;
    }

    Event(Type type,
          unsigned x,
          unsigned y):
        type(type),
        data()
    {
        switch (type) {
        case MouseMoved:
            data.mouse.x = x;
            data.mouse.y = y;
            break;
        case WindowResized:
            data.wndResize.width = x;
            data.wndResize.height = y;
        default:
            assert(!"invalid event type");
            break;
        }
    }

    Event(int mouseWheelDelta):
        type(MouseWheel),
        data()
    {
        data.mouseWheelDelta = mouseWheelDelta;
    }

    Event(Key code,
          bool pressed):
        type(pressed ? KeyPressed : KeyReleased),
        data()
    {
        data.key = code;
    }

    Event(bool focus):
        type(WindowFocus),
        data()
    {
        data.focus = focus;
    }
};

} // namespace sb

