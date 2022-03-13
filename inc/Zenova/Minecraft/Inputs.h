#pragma once

#include <algorithm>
#include <functional>
#include <iterator>
#include <string>
#include <utility>
#include <vector>
#include <initializer_list>

#include "Zenova/Minecraft.h"

//https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
//add most of the standard keyboard to this to allow for lists like { KeyboardBinding::C, KeyboardBinding::Mouse4 }
enum class KeyboardBinding : int {
    None,
    Back = 0x8,
    Tab,
    Return = 0xD,
    Shift = 0x10,
    Control,
    Pause = 0x13,
    CapsLock,
    Escape = 0x1B,
    Space = 0x20,
    PageUp,
    PageDown,
    End,
    Home,
    Left,
    Up,
    Right,
    Down,
    Insert = 0x2D,
    Delete,
    Numpad0 = 0x60,
    Numpad1,
    Numpad2,
    Numpad3,
    Numpad4,
    Numpad5,
    Numpad6,
    Numpad7,
    Numpad8,
    Numpad9,
    Multiply,
    Add,
    Subtract = 0x6D,
    Decimal,
    Divide,
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,
    F13,
    Numlock = 0x90,
    Scroll,
    Semicolon = 0xBA,
    Equals,
    Comma,
    Minus,
    Slash,
    Grave, // ~
    LeftBracket = 0xDB,
    Backslash,
    RightBracket,
    Apostrophe,
    Mouse1 = -99,
    Mouse2,
    Mouse3,
    Mouse4,
    Mouse5,
    Mouse6,
    Mouse7
};

enum class GamepadBinding : int {
	None,
	South,
	West,
	East,
	North,
	DpadUp,
	DpadDown,
	DpadLeft,
	DpadRight,
	LeftStick,
	RightStick,
	LeftShoulder,
	RightShoulder,
	Select,
	Start,
	Touchpad,
	LeftTrigger = -100,
	RightTrigger
};

enum class InputType {
    Keyboard,
    Xbox
};

namespace Zenova {
    using ButtonCallback = std::function<void(bool)>;

    class Keybind {
    public:
        std::vector<int> mKeys;
        bool mCreateGui;

        Keybind() : mCreateGui(false) {}
        Keybind(const Keybind& other) : mKeys(other.mKeys), mCreateGui(other.mCreateGui) {}

        Keybind(std::vector<int>&& binds, bool gui) : mKeys(std::move(binds)), mCreateGui(gui) {}
        Keybind(int bind, bool gui = true) : mKeys({ bind }), mCreateGui(gui) {}

        void addKey(int bind) {
            mKeys.push_back(bind);
        }

        template<typename E, std::enable_if_t<std::is_enum_v<E>>* = nullptr>
        Keybind(std::vector<E>&& binds, bool gui) : mCreateGui(gui) {
            std::transform(binds.begin(), binds.end(), std::back_inserter(mKeys), [](E bind) { return enum_cast(bind); });
        }

        template<typename E, std::enable_if_t<std::is_enum_v<E>>* = nullptr>
        Keybind(E bind, bool gui) : Keybind(enum_cast(bind), gui) {}

        template<typename E, std::enable_if_t<std::is_enum_v<E>>* = nullptr>
        void addKey(E bind) {
            mKeys.push_back(enum_cast(bind));
        }

        explicit operator bool() const {
            return mKeys.size();
        }
    };

    struct Input {
        ButtonCallback mCallback;
        Keybind mKeyboard;
        Keybind mGamepad;

        Input(ButtonCallback callback) : mCallback(callback) {}
        Input(Input&& input) noexcept : mKeyboard(std::move(input.mKeyboard)),
            mGamepad(std::move(input.mGamepad)),
            mCallback(std::move(input.mCallback)) {}

        // ex: keymapping = 'C' or KMBinding::Mouse5, gui = false
        template<typename T>
        Input& setKeyboardMapping(T bind, bool gui = true) {
            mKeyboard = Keybind(bind, gui);
            return *this;
        }

        template<typename T>
        Input& setKeyboardMapping(std::initializer_list<T> binds, bool gui = true) {
            if constexpr (std::is_same_v<T, KeyboardBinding>) {
                mKeyboard = Keybind(binds, gui);
            }
            else if (std::is_integral_v<T>) {
                mKeyboard = Keybind(std::vector<int>(binds.begin(), binds.end()), gui);
            }

            return *this;
        }

        template<typename T>
        Input& addKeyboardBind(T bind) {
            mKeyboard.addKey(bind);

            return *this;
        }

        // ex: keymapping = { GamepadBinding::DpadLeft }
        template<typename T>
        Input& setGamepadMapping(T bind, bool gui = true) {
            mGamepad = Keybind(bind, gui);

            return *this;
        }
    };

    class EXPORT InputManager {
    public:
        static Input& addInput(const std::string& name, ButtonCallback callback);

        static const std::vector<std::pair<std::string, Input>>& getInputs();
    };      
}