//
// This is for the Windows version of Zenova
//

#include "Zenova.h"
#include <Windows.h>

BOOL APIENTRY DllMain(HMODULE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
	switch(fdwReason) {
		case DLL_PROCESS_ATTACH: {
			if(Zenova::Platform::GetModuleBaseAddress("Minecraft.Windows.exe")) { //to avoid being attached to the runtime broker
				HANDLE tHandle = CreateThread(nullptr, 0, Zenova::Start, reinterpret_cast<void*>(hinstDLL), 0, nullptr);
				if(!tHandle) return false;
				CloseHandle(tHandle);
			}
		} break;
	}

	return true;
}