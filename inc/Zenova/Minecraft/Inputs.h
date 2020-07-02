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
    std::initializer_list<int> mKeys;
    bool mCreateGui;
    
    Keybind() : mCreateGui(false) {}
    Keybind(const Keybind& other) : mKeys(other.mKeys), mCreateGui(other.mCreateGui) {}

    Keybind(std::initializer_list<int> binds, bool gui = true) : mKeys(binds), mCreateGui(gui) {}
    Keybind(int bind, bool gui = true) : mKeys({ bind }), mCreateGui(gui) {}
    
    template<typename E, std::enable_if_t<std::is_enum<E>::value>* = nullptr>
    Keybind(std::initializer_list<E> binds, bool gui = true) : mCreateGui(gui) {
        std::transform(binds.begin(), binds.end(), std::back_inserter(mKeys), [](KeyboardBinding bind) { return enum_cast(bind); });
    }
    
    template<> Keybind(std::initializer_list<KeyboardBinding> binds, bool gui);
    template<> Keybind(std::initializer_list<GamepadBinding> binds, bool gui);

    Keybind(KeyboardBinding bind, bool gui = true) : Keybind(enum_cast(bind), gui) {}
    Keybind(GamepadBinding bind, bool gui = true) : Keybind(enum_cast(bind), gui) {}

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

        // ex: keymapping = 'C' or KMBinding::Mouse5, gui = false
        template<typename... Args>
        Input& setKeyboardMapping(Args&&... args) {
            mKeyboard = Keybind(std::forward<Args>(args)...);
            return *this;
        }

        // ex: keymapping = { GamepadBinding::DpadLeft }
        template<typename... Args>
        Input& setGamepadMapping(Args&&... args) {
            mGamepad = Keybind(std::forward<Args>(args)...);
            return *this;
        }
    };

    class EXPORT InputManager {
    private:
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