#include "Zenova/Minecraft/Inputs.h"
#include "Zenova/Log.h"
#include "Zenova/Platform.h"

namespace Zenova {
    inline std::unordered_map<std::string, Input> buttons;

    Input& InputManager::addInput(const std::string& name, ButtonCallback input) {
        static Input safeguard(input);

        auto it = buttons.insert(std::make_pair(name, Input(input)));
        if (it.second) {
            return it.first->second;
        }
        else {
            Zenova_Warn("Input {} already exists", name);
            return safeguard;
        }
    }

    const std::unordered_map<std::string, Input>& InputManager::getInputs() {
        return buttons;
    }
}