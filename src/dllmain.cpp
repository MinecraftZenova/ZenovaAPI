//
// This is for the Windows version of Zenova
//

#include "Zenova/Platform.h"
#include "Zenova/Helper.h"
#include <Windows.h>

BOOL APIENTRY DllMain(HMODULE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
	if((fdwReason == DLL_PROCESS_ATTACH) && Zenova::Platform::GetModuleBaseAddress("Minecraft.Windows.exe")) { //avoid being attached to the runtime broker
		HANDLE tHandle = CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(Zenova::Start), reinterpret_cast<void*>(hinstDLL), 0, nullptr);
		if(!tHandle) return false;
		CloseHandle(tHandle);
	}

	return true;
}