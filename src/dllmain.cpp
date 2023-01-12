//
// This is for the Windows version of Zenova
//

#include <Windows.h>

#include "Zenova/Platform.h"
#include "Zenova/Helper.h"
#include "generated/initcpp.h"

DWORD WINAPI call_run(LPVOID args) {
	return Zenova::run();
}

BOOL APIENTRY DllMain(HMODULE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) { 
	// avoid being attached to anything besides Minecraft Win10
	if ((fdwReason == DLL_PROCESS_ATTACH) && Zenova::Platform::GetMinecraftBaseAddress()) {
		InitBedrockPointers();

		auto args = reinterpret_cast<LPVOID>(hinstDLL);
		// block run till everything is initialized
		if (Zenova::start(args)) {
			HANDLE tHandle = CreateThread(nullptr, 0, call_run, args, 0, nullptr);
			if (!tHandle) return false;
			CloseHandle(tHandle);
		}
	}

	return true;
}