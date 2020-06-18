#include "Zenova/Platform.h"

#include <Windows.h>
#include <TlHelp32.h>
#include <sddl.h>
#include <aclapi.h>
#include <Psapi.h>
#include <direct.h>

#include <stdexcept>
#include <iostream>
#include <array>
#include <tuple>

#include "Zenova/Common.h"
#include "Zenova/Log.h"
#include "Zenova/StorageResolver.h"

#include "MinHook.h"

using exception = std::runtime_error; //figure out where to put this later

//Helper functions for CreateFileW
template <size_t T>
std::string DWORDMaskToString(DWORD value, const std::array<std::tuple<DWORD, const char*>, T>& masks) {
	std::string return_str = "";
	for(const auto &[mask, str] : masks) {
		if(value &= mask) {
			return_str.append(str).append(" | ");
		}
	}

	if(value) {
		return_str += std::to_string(value) + " | ";
	}

	return return_str.substr(0, return_str.rfind(" | "));
}

constexpr std::array<std::tuple<DWORD, const char *>, 4> access_rights{{
	std::make_tuple(GENERIC_READ, "GENERIC_READ"),
	std::make_tuple(GENERIC_WRITE, "GENERIC_WRITE"),
	std::make_tuple(GENERIC_EXECUTE, "GENERIC_EXECUTE"),
	std::make_tuple(GENERIC_ALL, "GENERIC_ALL")
}};
		
constexpr std::array<std::tuple<DWORD, const char *>, 4> share_rights{{
	std::make_tuple(FILE_SHARE_DELETE, "FILE_SHARE_DELETE"),
	std::make_tuple(FILE_SHARE_READ, "FILE_SHARE_READ"),
	std::make_tuple(FILE_SHARE_WRITE, "FILE_SHARE_WRITE"),
	std::make_tuple(0, "No Sharing")
}};
		
constexpr std::array<std::tuple<DWORD, const char *>, 8> file_attributes{{
	std::make_tuple(FILE_ATTRIBUTE_ARCHIVE, "FILE_ATTRIBUTE_ARCHIVE"),
	std::make_tuple(FILE_ATTRIBUTE_ENCRYPTED, "FILE_ATTRIBUTE_ENCRYPTED"),
	std::make_tuple(FILE_ATTRIBUTE_HIDDEN, "FILE_ATTRIBUTE_HIDDEN"),
	std::make_tuple(FILE_ATTRIBUTE_NORMAL, "FILE_ATTRIBUTE_NORMAL"),
	std::make_tuple(FILE_ATTRIBUTE_OFFLINE, "FILE_ATTRIBUTE_OFFLINE"),
	std::make_tuple(FILE_ATTRIBUTE_READONLY, "FILE_ATTRIBUTE_READONLY"),
	std::make_tuple(FILE_ATTRIBUTE_SYSTEM, "FILE_ATTRIBUTE_SYSTEM"),
	std::make_tuple(FILE_ATTRIBUTE_TEMPORARY, "FILE_ATTRIBUTE_TEMPORARY")
}};

std::string getAccessRightString(DWORD dwDesiredAccess) {
	return DWORDMaskToString(dwDesiredAccess, access_rights);
}

std::string getShareRightString(DWORD dwDesiredAccess) {
	return DWORDMaskToString(dwDesiredAccess, share_rights);
}

std::string getFlagsAndAttributesString(DWORD dwFlagsAndAttributes) {
	return DWORDMaskToString(dwFlagsAndAttributes, file_attributes);
}

std::string getCreationDispositionString(DWORD dwCreationDisposition) {
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

//Hooks (hfn = hooked function; pfn = pointer (to original) function)
inline HANDLE(__stdcall *pfnCreateFileW)(LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);
HANDLE __stdcall hfnCreateFileW(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) {
	//Can't redirect File Creates to folders the UWP app doesn't have access to
	std::wstring_view filePath(lpFileName);
	
	//SecurityDescriptor sd;

	// Check if it's accessing resources; this method should work in all future updates
	if(filePath.rfind(L"mcworld") != filePath.npos) {
		//Zenova::Console console("Zenova::CreateFileW");
		Zenova_Info(std::wstring(L"File: ") + filePath.data());
		Zenova_Info("{ " + getAccessRightString(dwDesiredAccess) + ", " + getShareRightString(dwShareMode) +  ", "
			+ getCreationDispositionString(dwCreationDisposition) + ", " + getFlagsAndAttributesString(dwFlagsAndAttributes) + " }");

		//std::wstring newPath(L"D:/minecraftWorlds/");
		//std::wstring_view strToFind(L"/minecraftWorlds/");
		//newPath += filePath.substr(filePath.find(strToFind) + strToFind.size(), filePath.size());

		// Check if a file exists at that path and reroute access to that file
		//if(FileExists(newPath.c_str()))
			//return pfnCreateFileW(newPath.c_str(), dwAccess, dwSharing, saAttributes, dwCreation, dwAttributes, hTemplate);
	}
	
	//DebugPause();

	//SECURITY_ATTRIBUTES sa{sizeof(SECURITY_ATTRIBUTES), nullptr, FALSE};
	//Log::info("Zenova::CreateFileW", std::to_string(ConvertStringSecurityDescriptorToSecurityDescriptor(szSD, SDDL_REVISION_1, &(sa.lpSecurityDescriptor), NULL)));
	//
	////ConvertStringSecurityDescriptorToSecurityDescriptorA("D:(D;OICI;GA;;;BG)(D;OICI;GA;;;AN)(A;OICI;GRGWGX;;;AU)(A;OICI;GA;;;BA)", SDDL_REVISION_1, &(sa.lpSecurityDescriptor), NULL);
	//Log::info("Zenova::CreateFileW", std::to_string(GetLastError()));
	
	HANDLE ret = pfnCreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	//Log::info("Zenova::CreateFileW", std::to_string(GetLastError()));
	return ret;
}

inline HANDLE(__stdcall *pfnCreateDirectoryW)(LPCWSTR, LPSECURITY_ATTRIBUTES);
HANDLE __stdcall hfnCreateDirectoryW(LPCWSTR lpFileName, LPSECURITY_ATTRIBUTES lpSecurityAttributes) {
	//Can't redirect Directory Creates to folders the UWP app doesn't have access to
	std::wstring_view filePath(lpFileName);

	// Check if it's accessing resources; this method should work in all future updates
	if(filePath.find(Zenova::StorageResolver::minecraft_path.wstr) != filePath.npos) {
		//Log::info(L"Zenova::CreateDirectoryW", filePath.data());
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

	return pfnCreateDirectoryW(lpFileName, lpSecurityAttributes);
}

inline HANDLE(__stdcall *pfnCreateDirectoryA)(LPCSTR, LPSECURITY_ATTRIBUTES);
HANDLE __stdcall hfnCreateDirectoryA(LPCSTR lpFileName, LPSECURITY_ATTRIBUTES lpSecurityAttributes) {
	//Can't redirect Directory Creates to folders the UWP app doesn't have access to
	std::string_view filePath(lpFileName);

	// Check if it's accessing resources; this method should work in all future updates
	if(filePath.find(Zenova::StorageResolver::minecraft_path.str()) != filePath.npos) {
		//Log::info("Zenova::CreateDirectoryA", filePath.data());
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
		
	return pfnCreateDirectoryA(lpFileName, lpSecurityAttributes);
}

namespace Zenova {
	namespace Platform {
		PlatformType Type = PlatformType::Windows;

	#ifdef ZENOVA_API
		bool Init(void* vars) {
			if(MH_Initialize() != MH_OK) {
				Zenova_Info("MinHook failed to initialize, normal launch without hooks");
				return false;
			}

			CleanupVariables = vars;
			//CreateHook("KernelBase.dll", "CreateFileW", (void*)&hfnCreateFileW, (void**)&pfnCreateFileW);
			//CreateHook("KernelBase.dll", "CreateDirectoryW", (void*)&hfnCreateDirectoryW, (void**)&pfnCreateDirectoryW);
			//CreateHook("KernelBase.dll", "CreateDirectoryA", (void*)&hfnCreateDirectoryA, (void**)&pfnCreateDirectoryA);
			return true;
		}

		void Destroy() {
			if(CleanupVariables) {
				FreeLibraryAndExitThread(reinterpret_cast<HMODULE>(CleanupVariables), 0);
			}
		}
	#endif // ZENOVA_API

		void* LoadModule(const std::string& module) {
			return LoadLibraryA((module + ".dll").c_str());
		}

		bool CloseModule(void* module) {
			return FreeLibrary(reinterpret_cast<HMODULE>(module));
		}

		void* GetModuleFunction(void* module, const std::string& function) {
			return GetProcAddress(reinterpret_cast<HMODULE>(module), function.c_str());
		}

		void* FindAddress(const std::string& module, const std::string& function) {
			HMODULE hModule = GetModuleHandleA(module.c_str());
			if(hModule == NULL)
				return 0;

			FARPROC hFunction = GetProcAddress(hModule, function.c_str());
			if(hFunction == NULL)
				return 0;

			return hFunction;
		}

		inline std::unordered_map<void*, void**> hooks;

		bool CreateHook(void* address, void* funcJump, void** funcTrampoline) {
			auto& existingHook = hooks.find(address);
			if(existingHook != hooks.end()) {
				auto tmp = *existingHook->second;
				*existingHook->second = funcJump;
				*funcTrampoline = tmp;

				existingHook->second = funcTrampoline;
			}
			else {
				if(MH_CreateHook(address, funcJump, funcTrampoline) != MH_OK)
					return false;

				if(MH_EnableHook(address) != MH_OK)
					return false;

				hooks.insert({ address, funcTrampoline });
			}



			return true;
		}

		void CreateFileStream(const std::string& path) {

		}

		u32 GetRWProtect() {
			return PAGE_READWRITE;
		}

		u32 SetPageProtect(void* addr, size_t len, u32 prot) {
			DWORD oldProt{};
			VirtualProtect(addr, len, prot, &oldProt);
			return oldProt;
		}

		/*All of the RTTI class object loaders start from 1, if there's an exception
		to this rule (in the original code that is) please let me know by filling an
		issue on the github page.
		*/
		size_t GetVtableSize(uintptr_t** vtableToCheck) {
			for(size_t size = 0; size < 1000; size++) { //1000 is arbitrary 
				if(*(vtableToCheck[size]) == 1) {
					return size;
				}
			}

			return 0;
		}

		void DebugPause() {
			if(IsDebuggerPresent()) {
				DebugBreak();
			}
		}

		void OutputDebugMessage(const std::string& message) {
			OutputDebugStringA(message.c_str());
		}
		
		void OutputDebugMessage(const std::wstring& message) {
			OutputDebugStringW(message.c_str());
		}

		uintptr_t GetModuleBaseAddress(const std::string& modName) {
			return reinterpret_cast<uintptr_t>(GetModuleHandleA(modName.c_str()));
		}

		uintptr_t GetModuleBaseAddress(const std::wstring& modName) {
			return reinterpret_cast<uintptr_t>(GetModuleHandleW(modName.c_str()));
		}

		u32 GetModuleSize(const char* module) {
			MODULEINFO module_info{};

			HMODULE base = GetModuleHandleA(module);
			if(base == nullptr || !GetModuleInformation(GetCurrentProcess(), base, &module_info, sizeof(MODULEINFO))) {
				return 0;
			}

			return module_info.SizeOfImage;
		}

		void ErrorPrinter() {
			// Convert GetLastError() to an actual string using the windows api
			LPSTR message_buffer{};

			FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), message_buffer, 0, NULL);
			if(!message_buffer) {
				return;
			}

			Zenova_Error(message_buffer);
			LocalFree(message_buffer);
		}
	}
}