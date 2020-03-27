#include "Zenova/OS/Linux.h"

namespace Zenova {
	Linux::Linux() : OS(OSType::Linux) {}

	void* Linux::FindAddress(const std::string& module, const std::string& function) {

		return nullptr;
	}
	
	bool Linux::CreateHook(void* address, void* funcJump, void** funcTrampoline) {
		//MSHookFunction(address, funcJump, funcTrampoline); android

		return true;
	}

#ifdef ZENOVA_API
	void Linux::SetupOS(void* variables) {
		
	}
#endif // ZENOVA_API
}