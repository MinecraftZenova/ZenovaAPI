#include "Zenova/Minecraft/Inputs.h"
#include "Zenova/Log.h"
#include "Zenova/Platform.h"

namespace Zenova {
    inline std::vector<InputManager::Button> buttons;
    static Input safeguard(nullptr);

    Input& InputManager::addInput(const std::string& name, ButtonCallback callback) {
        auto it = std::find_if(buttons.begin(), buttons.end(),
            [&name](const Button& elem) {
                return name == elem.rawName;
            });

        if (it != buttons.end()) {
            Zenova_Warn("Input {} already exists", name);
            // this is intentional, we don't want the problematic mod to change binds
            return safeguard;
        }

        Button newButton = {
            name,
            "button." + name,
            "key." + name,
            Input(callback)
        };
        
        return buttons.emplace_back(newButton).input;
    }

    const std::vector<InputManager::Button>& InputManager::getInputs() {
        return buttons;
    }
}