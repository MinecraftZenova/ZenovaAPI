#include "Zenova/Minecraft/Inputs.h"
#include "Zenova/Log.h"
#include "Zenova/Platform.h"

namespace Zenova {
    InputManager& InputManager::instance() {
        static InputManager singleton;
        return singleton;
    }

    InputManager::InputManager() {}

    Input& InputManager::addInput(const std::string& name, ButtonCallback input) {
        static Input safeguard(input);

        auto it = buttons.insert({ name, { input }});
        if (it.second) {
            return it.first->second;
        }
        else {
            Zenova_Warn("Input {} already exists", name);
            return safeguard;
        }
    }

    std::unordered_map<std::string, Input>& InputManager::getInputs() {
        return buttons;
    }
}