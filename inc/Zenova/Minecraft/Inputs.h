#pragma once

#include <algorithm>
#include <functional>
#include <iterator>
#include <string>
#include <utility>
#include <unordered_map>
#include <initializer_list>

#include "Zenova/Minecraft.h"

enum class KeyboardBinding : int {
    None,
    Mouse1 = -99,
    Mouse2,
    Mouse3,
    Mouse4,
    Mouse5,
    Mouse6
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

class Keybind {
public:
    std::vector<int> mKeys;
    bool mCreateGui;
    
    Keybind() : mCreateGui(false) {}
    Keybind(const Keybind& other) : mKeys(other.mKeys), mCreateGui(other.mCreateGui) {}

    Keybind(const std::vector<int>& binds, bool gui) : mKeys(binds), mCreateGui(gui) {}
    Keybind(int bind, bool gui = true) : mKeys({ bind }), mCreateGui(gui) {}
    
    template<typename E, std::enable_if_t<std::is_enum<E>::value>* = nullptr>
    Keybind(const std::vector<E>& binds, bool gui) : mCreateGui(gui) {
        std::transform(binds.begin(), binds.end(), std::back_inserter(mKeys), [](E bind) { return enum_cast(bind); });
    }

    Keybind(KeyboardBinding bind, bool gui) : Keybind(enum_cast(bind), gui) {}
    Keybind(GamepadBinding bind, bool gui) : Keybind(enum_cast(bind), gui) {}

    operator bool() const {
        return mKeys.size();
    }
};

namespace Zenova {
    using ButtonCallback = std::function<void(bool)>;

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
            if constexpr (std::is_enum_v<T>) {
                mKeyboard = Keybind(binds, gui);
            }
            else if (std::is_integral_v<T>) {
                mKeyboard = Keybind(std::vector<int>(binds.begin(), binds.end()), gui);
            }

            return *this;
        }

        // ex: keymapping = { GamepadBinding::DpadLeft }
        template<typename T>
        Input& setGamepadMapping(T bind, bool gui = true) {
            mGamepad = Keybind(bind, gui);
            return *this;
        }

        template<typename T>
        Input& setGamepadMapping(std::initializer_list<T> binds, bool gui = true) {
            if constexpr (std::is_enum_v<T>) {
                mGamepad = Keybind(binds, gui);
            }
            else if (std::is_integral_v<T>) {
                mGamepad = Keybind(std::vector<int>(binds.begin(), binds.end()), gui);
            }

            return *this;
        }
    };

    class EXPORT InputManager {
        InputManager();

        std::unordered_map<std::string, Input> buttons;

    public:
        InputManager(const InputManager&) = delete;
        void operator=(const InputManager&) = delete;

        static InputManager& instance();

        Input& addInput(const std::string& name, ButtonCallback callback);

        std::unordered_map<std::string, Input>& getInputs();
    };      
}