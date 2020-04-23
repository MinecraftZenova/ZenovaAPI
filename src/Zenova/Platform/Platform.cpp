#include "Platform.h"

#include <filesystem>

#include "Zenova/Log.h"
#include "Utils/Utils.h"

namespace Zenova {
	std::string Platform::TypeToString(const PlatformType& type) {
		switch(type) {
			case PlatformType::Windows: {
				return "Windows";
			}
			case PlatformType::Linux: {
				return "Linux";
			}
			default: {
				return "Unknown";
			}
		}
	}

	bool Platform::CreateHook(const std::string& module, const std::string& function, void* funcJump, void** funcTrampoline) {
		return CreateHook(FindAddress(module, function), funcJump, funcTrampoline);
	}

#ifdef ZENOVA_API
#endif
}