//
// This is for the Windows version of Zenova
//

#include "Zenova.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	if(ul_reason_for_call == DLL_PROCESS_ATTACH) {
		if(Zenova::Util::GetModuleBaseAddressA("Minecraft.Windows.exe")) { //to avoid being attached to the runtime broker
			HANDLE tHandle = CreateThread(nullptr, 0, Zenova::Start, new HMODULE(hModule), 0, nullptr);
			if(tHandle) 
				CloseHandle(tHandle);
		}
	}
	return TRUE;
}