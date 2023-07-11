#include "Zenova/Platform.h"

#include <filesystem>

#include "Zenova/Log.h"

namespace Zenova {
	std::string Platform::TypeToString(PlatformType type) {
		switch (type) {
			case PlatformType::Windows: {
				return "Windows";
			}
			default: {
				return "Unknown";
			}
		}
	}

	bool Platform::CreateHook(const char* module, const char* function, void* funcJump, void* funcTrampoline) {
		return CreateHook(FindAddress(module, function), funcJump, funcTrampoline);
	}
}