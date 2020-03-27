#include "Zenova/OS/OS.h"
#include "Zenova/Log.h"

namespace Zenova {
	OS::OS(OSType os) {
		type = os;
		Info("Using " + OSTypeToString(os) + " Preset");
	}
	
	std::string OS::OSTypeToString(const OSType& type) {
		switch(type) {
			case OSType::Windows: {
				return "Windows";
			}
			case OSType::Linux: {
				return "Linux";
			}
			default: {
				return "Unknown";
			}
		}
	}

	bool OS::CreateHook(const std::string& module, const std::string& function, void* funcJump, void** funcTrampoline) {
		return CreateHook(FindAddress(module, function), funcJump, funcTrampoline);
	}
}