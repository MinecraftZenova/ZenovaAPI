#include "Zenova/Minecraft/Inputs.h"
#include "Zenova/Log.h"
#include "Zenova/Platform.h"

namespace Zenova {
    inline std::vector<std::pair<std::string, Input>> buttons;

    Input& InputManager::addInput(const std::string& name, ButtonCallback input) {
        static Input safeguard(input);

        //I'm open to a better formatting for this
        if (std::find_if(
            buttons.begin(), buttons.end(),
            [&name](const std::pair<std::string, Input>& elem) {
                return name == elem.first;
            }) == buttons.end()) {
            return buttons.emplace_back(std::make_pair(name, Input(input))).second;
        }
        else {
            Zenova_Warn("Input {} already exists", name);
            return safeguard;
        }
    }

    const std::vector<std::pair<std::string, Input>>& InputManager::getInputs() {
        return buttons;
    }
}