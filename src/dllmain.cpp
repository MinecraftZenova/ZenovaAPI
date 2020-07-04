//
// This is for the Windows version of Zenova
//

#include <Windows.h>

#include "Zenova/Platform.h"
#include "Zenova/Helper.h"
#include "Zenova/Globals.h"
#include "generated/initcpp.h"

BOOL APIENTRY DllMain(HMODULE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
	Zenova::BaseAddress = Zenova::Platform::GetModuleBaseAddress("Minecraft.Windows.exe");
	if((fdwReason == DLL_PROCESS_ATTACH) && Zenova::BaseAddress) { //avoid being attached to anything besides Minecraft Win10
		InitBedrockPointers(); 
		HANDLE tHandle = CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(Zenova::start), reinterpret_cast<void*>(hinstDLL), 0, nullptr);
		if(!tHandle) return false;
		CloseHandle(tHandle);
	}

	return true;
}