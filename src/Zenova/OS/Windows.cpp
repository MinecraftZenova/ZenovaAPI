#include "Zenova/OS/Windows.h"

#include <TlHelp32.h>
#include <sddl.h>
#include <aclapi.h>
#include <stdexcept>
#include <iostream>
#include <array>
#include <tuple>

#include "Zenova/Log.h"
#include "Zenova/StorageResolver.h"
#include "MinHook.h"

using exception = std::runtime_error; //figure out where to put this later

namespace Zenova {
	Windows::Windows() : OS(OSType::Windows), canHook(true) {
		if(MH_Initialize() != MH_OK) {
			throw exception("MinHook failed to initialize, normal launch without hooks");
			canHook = false;
		}
	}

	Windows::~Windows() {
		FreeLibraryAndExitThread(*reinterpret_cast<HMODULE*>(dllHandle.get()), 0);
	}

	void* Windows::FindAddress(const std::string& module, const std::string& function) {
		HMODULE hModule = GetModuleHandleA(module.c_str());
		if(hModule == NULL)
			return 0;

		FARPROC hFunction = GetProcAddress(hModule, function.c_str());
		if(hFunction == NULL)
			return 0;

		return hFunction;
	}

	bool Windows::CreateHook(void* address, void* funcJump, void** funcTrampoline) {
		if(canHook) {
			if(MH_CreateHook(address, funcJump, funcTrampoline) != MH_OK)
				return false;

			if(MH_EnableHook(address) != MH_OK)
				return false;

			return true;
		}

		return false;
	}

	void Windows::CreateFileStream(const std::string& path) {

	}

	Mod* Windows::LoadMod(const std::string& mod) {
		//do stuff
		return nullptr;
	}

	u32 Windows::GetRWProtect() {
		return PAGE_READWRITE;
	}

	u32 Windows::SetPageProtect(void* addr, size_t len, u32 prot) {
		DWORD oldProt;

		if(VirtualProtect(addr, len, prot, &oldProt)) {
			return 0;
		}

		return oldProt;
	}

	/*All of the RTTI class object loaders start from 1, if there's an exception
	to this rule (in the original code that is) please let me know by filling an
	issue on the github page.
	*/
	size_t Windows::GetVtableSize(uintptr_t** vtableToCheck) {
		for(size_t size = 0; size < 1000; size++) { //1000 is arbitrary 
			if(*(vtableToCheck[size]) == 1) {
				return size;
			}
		}
	
		return 0;
	} 

	void Windows::DebugPause() {
		if(IsDebuggerPresent()) {
			DebugBreak();
		}
	}

#ifdef ZENOVA_API
	void Windows::SetupOS(void* variables) {
		dllHandle = std::shared_ptr<DWORD>(reinterpret_cast<DWORD*>(variables));

		Zenova::hostOS->CreateHook("KernelBase.dll", "CreateFileW", (void*) &Zenova::CreateFileW, (void**) &Zenova::pfnCreateFileW);
		Zenova::hostOS->CreateHook("KernelBase.dll", "CreateDirectoryW", (void*) &Zenova::CreateDirectoryW, (void**) &Zenova::pfnCreateDirectoryW);
		Zenova::hostOS->CreateHook("KernelBase.dll", "CreateDirectoryA", (void*) &Zenova::CreateDirectoryA, (void**) &Zenova::pfnCreateDirectoryA);
	}
#endif // ZENOVA_API

	/*
	bool FileExists(LPCWSTR fileName) {
		if (INVALID_FILE_ATTRIBUTES == GetFileAttributesW(fileName) && GetLastError() == ERROR_FILE_NOT_FOUND) return false;
		return true;
	}
	*/
	
	HANDLE(__stdcall *pfnCreateFileW)(LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);
	HANDLE(__stdcall *pfnCreateDirectoryW)(LPCWSTR, LPSECURITY_ATTRIBUTES);
	HANDLE(__stdcall *pfnCreateDirectoryA)(LPCSTR, LPSECURITY_ATTRIBUTES);

	HANDLE CreateFileW(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) {
		//Can't redirect File Creates to folders the UWP app doesn't have access to
		std::wstring_view filePath(lpFileName);
	
		//SecurityDescriptor sd;

		// Check if it's accessing resources; this method should work in all future updates
		if(filePath.rfind(L"mcworld") != filePath.npos) {
			//Zenova::Console console("Zenova::Windows::CreateFileW");
			Info(std::wstring(L"File: ") + filePath.data());
			Info("{ " + Util::getAccessRightString(dwDesiredAccess) + ", " + Util::getShareRightString(dwShareMode) +  ", " 
				+ Util::getCreationDispositionString(dwCreationDisposition) + ", " + Util::getFlagsAndAttributesString(dwFlagsAndAttributes) + " }");

			//std::wstring newPath(L"D:/minecraftWorlds/");
			//std::wstring_view strToFind(L"/minecraftWorlds/");
			//newPath += filePath.substr(filePath.find(strToFind) + strToFind.size(), filePath.size());

			// Check if a file exists at that path and reroute access to that file
			//if(FileExists(newPath.c_str()))
				//return pfnCreateFileW(newPath.c_str(), dwAccess, dwSharing, saAttributes, dwCreation, dwAttributes, hTemplate);
		}
	
		//Windows::DebugPause();

		//SECURITY_ATTRIBUTES sa{sizeof(SECURITY_ATTRIBUTES), nullptr, FALSE};
		//Log::info("Zenova::Windows::CreateFileW", std::to_string(ConvertStringSecurityDescriptorToSecurityDescriptor(szSD, SDDL_REVISION_1, &(sa.lpSecurityDescriptor), NULL)));
		//
		////ConvertStringSecurityDescriptorToSecurityDescriptorA("D:(D;OICI;GA;;;BG)(D;OICI;GA;;;AN)(A;OICI;GRGWGX;;;AU)(A;OICI;GA;;;BA)", SDDL_REVISION_1, &(sa.lpSecurityDescriptor), NULL);
		//Log::info("Zenova::Windows::CreateFileW", std::to_string(GetLastError()));
	
		HANDLE ret = pfnCreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
		//Log::info("Zenova::Windows::CreateFileW", std::to_string(GetLastError()));
		return ret;
	}

	HANDLE CreateDirectoryW(LPCWSTR lpFilename, LPSECURITY_ATTRIBUTES saAttributes) {
		//Can't redirect Directory Creates to folders the UWP app doesn't have access to
		std::wstring_view filePath(lpFilename);

		// Check if it's accessing resources; this method should work in all future updates
		if(filePath.find(StorageResolver::minecraft_path) != filePath.npos) {
			//Log::info(L"Zenova::Windows::CreateDirectoryW", filePath.data());
			std::wstring newPath(L"D:/minecraftWorlds/");
			std::wstring newDir(newPath);
			std::wstring_view strToFind(L"/minecraftWorlds/");
			size_t startPoint = filePath.find(strToFind) + strToFind.size();
			newPath += filePath.substr(startPoint);
		
			size_t endPoint = filePath.find(L"/", startPoint) - startPoint;
			newDir += filePath.substr(startPoint, endPoint);

			// Check if a file exists at that path and reroute access to that file
			//if(FileExists(newPath.c_str()))
				//return pfnCreateDirectoryW(newPath.c_str(), saAttributes);
		}

		return pfnCreateDirectoryW(lpFilename, saAttributes);
	}

	HANDLE CreateDirectoryA(LPCSTR lpFilename, LPSECURITY_ATTRIBUTES saAttributes) {
		//Can't redirect Directory Creates to folders the UWP app doesn't have access to
		std::string_view filePath(lpFilename);

		// Check if it's accessing resources; this method should work in all future updates
		if(filePath.find(StorageResolver::minecraft_path.str()) != filePath.npos) {
			//Log::info("Zenova::Windows::CreateDirectoryA", filePath.data());
			std::string newPath("D:/minecraftWorlds/");
			std::string newDir(newPath);
			std::string strToFind("/minecraftWorlds/");
			size_t startPoint = filePath.find(strToFind) + strToFind.size();
			newPath += filePath.substr(startPoint);
		
			size_t endPoint = filePath.find("/", startPoint) - startPoint;
			newDir += filePath.substr(startPoint, endPoint);

			// Check if a file exists at that path and reroute access to that file
			//if(FileExists(newPath.c_str()))
				//return pfnCreateDirectoryW(newPath.c_str(), saAttributes);
		}
		
		return pfnCreateDirectoryA(lpFilename, saAttributes);
	}
	
	namespace Util {
		uintptr_t GetModuleBaseAddressA(const char* modName) {
			DWORD procId = GetCurrentProcessId();
			uintptr_t modBaseAddr = 0;
			HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
			if(hSnap != INVALID_HANDLE_VALUE) {
				tagMODULEENTRY32 modEntry;
				modEntry.dwSize = sizeof(modEntry);
				if(Module32First(hSnap, &modEntry)) {
					do {
						if(!_stricmp(modEntry.szModule, modName)) {
							modBaseAddr = reinterpret_cast<uintptr_t>(modEntry.modBaseAddr);
							break;
						}
					} while(Module32Next(hSnap, &modEntry));
				}
			}
			CloseHandle(hSnap);
			return modBaseAddr;
		}

		uintptr_t GetModuleBaseAddressW(const wchar_t* modName) {
			DWORD procId = GetCurrentProcessId();
			uintptr_t modBaseAddr = 0;
			HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
			if(hSnap != INVALID_HANDLE_VALUE) {
				MODULEENTRY32W modEntry;
				modEntry.dwSize = sizeof(modEntry);
				if(Module32FirstW(hSnap, &modEntry)) {
					do {
						if(!_wcsicmp(modEntry.szModule, modName)) {
							modBaseAddr = reinterpret_cast<uintptr_t>(modEntry.modBaseAddr);
							break;
						}
					} while(Module32NextW(hSnap, &modEntry));
				}
			}
			CloseHandle(hSnap);
			return modBaseAddr;
		}

		//MS made the "normal" string functions unusuable with their stupid macros
		DWORD GetModuleSize(DWORD processID, const char* module) {
			HANDLE hSnap;
			MODULEENTRY32 xModule;
			hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processID);
			xModule.dwSize = sizeof(MODULEENTRY32);
			if(Module32First(hSnap, &xModule)) {
				while(Module32Next(hSnap, &xModule)) {
					if(!strncmp(xModule.szModule, module, 8)) {
						CloseHandle(hSnap);
						return xModule.modBaseSize;
					}
				}
			}
			CloseHandle(hSnap);
			return 0;
		}
		
		constexpr std::array<std::tuple<DWORD, const char *>, 4> access_rights{{
			std::make_tuple(GENERIC_READ, "GENERIC_READ"),
			std::make_tuple(GENERIC_WRITE, "GENERIC_WRITE"),
			std::make_tuple(GENERIC_EXECUTE, "GENERIC_EXECUTE"),
			std::make_tuple(GENERIC_ALL, "GENERIC_ALL")
		}};

		std::string getAccessRightString(DWORD dwDesiredAccess) {
			std::string return_str = "";
			for(const auto &[mask, str] : access_rights) {
				if(dwDesiredAccess & mask) {
					return_str.append(str).append(" | ");
				}
			}
			return return_str.substr(0, return_str.rfind(" | "));
		}
		
		constexpr std::array<std::tuple<DWORD, const char *>, 4> share_rights{{
			std::make_tuple(FILE_SHARE_DELETE, "FILE_SHARE_DELETE"),
			std::make_tuple(FILE_SHARE_READ, "FILE_SHARE_READ"),
			std::make_tuple(FILE_SHARE_WRITE, "FILE_SHARE_WRITE"),
			std::make_tuple(0, "No Sharing")
		}};

		std::string getShareRightString(DWORD dwDesiredAccess) {
			std::string return_str = "";
			for(const auto &[mask, str] : share_rights) {
				if(dwDesiredAccess & mask) {
					return_str.append(str).append(" | ");
				}
			}
			return return_str.substr(0, return_str.rfind(" | "));
		}

		std::string getCreationDispositionString(const DWORD& dwCreationDisposition) {
			switch(dwCreationDisposition) {
				case CREATE_ALWAYS:
					return "CREATE_ALWAYS";
				case CREATE_NEW:
					return "CREATE_NEW";
				case OPEN_ALWAYS:
					return "OPEN_ALWAYS";
				case OPEN_EXISTING:
					return "OPEN_EXISTING";
				case TRUNCATE_EXISTING:
					return "TRUNCATE_EXISTING";
				default:
					return std::to_string(dwCreationDisposition);
			}
		}

		std::string getFlagsAndAttributesString(const DWORD& dwFlagsAndAttributes) {
			switch(dwFlagsAndAttributes) { //only some of the basic ones
				case FILE_ATTRIBUTE_ARCHIVE:
					return "FILE_ATTRIBUTE_ARCHIVE";
				case FILE_ATTRIBUTE_ENCRYPTED:
					return "FILE_ATTRIBUTE_ENCRYPTED";
				case FILE_ATTRIBUTE_HIDDEN:
					return "FILE_ATTRIBUTE_HIDDEN";
				case FILE_ATTRIBUTE_NORMAL:
					return "FILE_ATTRIBUTE_NORMAL";
				case FILE_ATTRIBUTE_OFFLINE:
					return "FILE_ATTRIBUTE_OFFLINE";
				case FILE_ATTRIBUTE_READONLY:
					return "FILE_ATTRIBUTE_READONLY";
				case FILE_ATTRIBUTE_SYSTEM:
					return "FILE_ATTRIBUTE_SYSTEM";
				case FILE_ATTRIBUTE_TEMPORARY:
					return "FILE_ATTRIBUTE_TEMPORARY";
				default:
					return std::to_string(dwFlagsAndAttributes);
			}
		}
	}
}