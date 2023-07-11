#pragma once

#include <functional>
#include <deque>

#include "Common.h"

#define MOD_FUNCTION extern "C" __declspec(dllexport)

#define CALL_MOD_FUNC(handle, func, ...) \
    if (handle) {  \
        auto modFunc = reinterpret_cast<decltype(&func)>(Zenova::Platform::GetModuleFunction(handle, #func)); \
        if (modFunc) { \
            modFunc(__VA_ARGS__); \
        } \
    }

namespace Zenova {
    struct ZENOVA_EXPORT ModContext {
        struct ModInfo* const info;

        void* GetHandle();
        std::string GetFolder();
    };

    ZENOVA_EXPORT std::deque<ModContext>& GetMods();
}

// Define any/all of these functions

// Called when the Mod is loaded
MOD_FUNCTION void ModLoad(Zenova::ModContext& ctx);

// Called after main
// todo: implement above (currently called after ~2 seconds)
MOD_FUNCTION void ModStart();

// Called repeatedly after ModStart
MOD_FUNCTION void ModUpdate();

// Called on clean up/exit
MOD_FUNCTION void ModStop();