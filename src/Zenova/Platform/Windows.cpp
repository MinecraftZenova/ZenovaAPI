#include "PlatformImpl.h"
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

#include "Zenova.h"
#include "Zenova/Globals.h"
#include "Zenova/StorageResolver.h"

#include "MinHook.h"

namespace Zenova {
	namespace PlatformImpl {
		inline void* CleanupVariables = nullptr;

		bool Init(void* platformArgs) {
			if (MH_Initialize() != MH_OK) {
				logger.info("MinHook failed to initialize, normal launch without hooks");
				return false;
			}

			CleanupVariables = platformArgs;
			//CreateHook("KernelBase.dll", "CreateFileW", (void*)&hfnCreateFileW, (void**)&pfnCreateFileW);
			//CreateHook("KernelBase.dll", "CreateDirectoryW", (void*)&hfnCreateDirectoryW, (void**)&pfnCreateDirectoryW);
			//CreateHook("KernelBase.dll", "CreateDirectoryA", (void*)&hfnCreateDirectoryA, (void**)&pfnCreateDirectoryA);
			return true;
		}

		void Destroy() {
			if (CleanupVariables) {
				FreeLibraryAndExitThread(reinterpret_cast<HMODULE>(CleanupVariables), 0);
			}
		}
	}
}

//Helper functions for CreateFileW
template<std::size_t N>
std::string DWORDMaskToString(DWORD value, const std::tuple<DWORD, const char*> (&masks)[N]) {
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

constexpr std::tuple<DWORD, const char*> access_rights[] = {
	std::make_tuple(GENERIC_READ, "GENERIC_READ"),
	std::make_tuple(GENERIC_WRITE, "GENERIC_WRITE"),
	std::make_tuple(GENERIC_EXECUTE, "GENERIC_EXECUTE"),
	std::make_tuple(GENERIC_ALL, "GENERIC_ALL")
};
		
constexpr std::tuple<DWORD, const char*> share_rights[] = {
	std::make_tuple(FILE_SHARE_DELETE, "FILE_SHARE_DELETE"),
	std::make_tuple(FILE_SHARE_READ, "FILE_SHARE_READ"),
	std::make_tuple(FILE_SHARE_WRITE, "FILE_SHARE_WRITE"),
	std::make_tuple(0, "No Sharing")
};
		
constexpr std::tuple<DWORD, const char*> file_attributes[] = {
	std::make_tuple(FILE_ATTRIBUTE_ARCHIVE, "FILE_ATTRIBUTE_ARCHIVE"),
	std::make_tuple(FILE_ATTRIBUTE_ENCRYPTED, "FILE_ATTRIBUTE_ENCRYPTED"),
	std::make_tuple(FILE_ATTRIBUTE_HIDDEN, "FILE_ATTRIBUTE_HIDDEN"),
	std::make_tuple(FILE_ATTRIBUTE_NORMAL, "FILE_ATTRIBUTE_NORMAL"),
	std::make_tuple(FILE_ATTRIBUTE_OFFLINE, "FILE_ATTRIBUTE_OFFLINE"),
	std::make_tuple(FILE_ATTRIBUTE_READONLY, "FILE_ATTRIBUTE_READONLY"),
	std::make_tuple(FILE_ATTRIBUTE_SYSTEM, "FILE_ATTRIBUTE_SYSTEM"),
	std::make_tuple(FILE_ATTRIBUTE_TEMPORARY, "FILE_ATTRIBUTE_TEMPORARY")
};

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
	std::wstring_view filePath(lpFileName);
	
	/*
	if(filePath.rfind(L"mcworld") != filePath.npos) {
		//Zenova::Console console("Zenova::CreateFileW");
		Zenova::logger.info("File: {}", filePath.data());
		Zenova::logger.info("{{ {}, {}, {}, {} }}", 
							getAccessRightString(dwDesiredAccess), 
							getShareRightString(dwShareMode),
							getCreationDispositionString(dwCreationDisposition), 
							getFlagsAndAttributesString(dwFlagsAndAttributes));
	}
	*/

	return pfnCreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

inline HANDLE(__stdcall *pfnCreateDirectoryW)(LPCWSTR, LPSECURITY_ATTRIBUTES);
HANDLE __stdcall hfnCreateDirectoryW(LPCWSTR lpFileName, LPSECURITY_ATTRIBUTES lpSecurityAttributes) {
	std::wstring_view filePath(lpFileName);

	//need to rewrite this

	return pfnCreateDirectoryW(lpFileName, lpSecurityAttributes);
}

inline HANDLE(__stdcall *pfnCreateDirectoryA)(LPCSTR, LPSECURITY_ATTRIBUTES);
HANDLE __stdcall hfnCreateDirectoryA(LPCSTR lpFileName, LPSECURITY_ATTRIBUTES lpSecurityAttributes) {
	std::string_view filePath(lpFileName);

	//need to rewrite this
		
	return pfnCreateDirectoryA(lpFileName, lpSecurityAttributes);
}

namespace Zenova {
	const PlatformType Platform::Type = PlatformType::Windows;

	void* Platform::FindAddress(const std::string& module, const std::string& function) {
		HMODULE hModule = GetModuleHandleA(module.c_str());
		if(hModule == NULL)
			return 0;

		FARPROC hFunction = GetProcAddress(hModule, function.c_str());
		if(hFunction == NULL)
			return 0;

		return hFunction;
	}

	inline std::unordered_map<void*, void**> hooks;

	bool Platform::CreateHook(void* address, void* funcJump, void** funcTrampoline) {
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

	u32 Platform::SetPageProtect(void* addr, size_t len, u32 prot) {
		DWORD oldProt{};
		VirtualProtect(addr, len, prot, &oldProt);
		return oldProt;
	}

	inline bool operator&(ProtectionFlags a, ProtectionFlags b) {
		return static_cast<std::underlying_type_t<ProtectionFlags>>(a)& static_cast<std::underlying_type_t<ProtectionFlags>>(b);
	}

	u32 Platform::SetPageProtect(void* addr, size_t len, ProtectionFlags prot) {
		DWORD dwordProt = enum_cast(ProtectionFlags::None);

		if (!(prot & ProtectionFlags::None)) {
			if (prot & ProtectionFlags::Write) {
				dwordProt = enum_cast(ProtectionFlags::Write);
			}
			else if (prot & ProtectionFlags::Read) {
				dwordProt = enum_cast(ProtectionFlags::Read);
			}

			if (prot & ProtectionFlags::Execute) {
				dwordProt <<= 4;
			}
		}

		return SetPageProtect(addr, len, dwordProt);
	}

	void Platform::ErrorPrinter() {
		// Convert GetLastError() to an actual string using the windows api
		LPSTR message_buffer{};

		FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
					   NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), message_buffer, 0, NULL);
		if (!message_buffer) {
			return;
		}

		logger.error(message_buffer);
		LocalFree(message_buffer);
	}

	uintptr_t Platform::GetModuleBaseAddress(const std::string& modName) {
		return reinterpret_cast<uintptr_t>(GetModuleHandleA(modName.c_str()));
	}

	uintptr_t Platform::GetModuleBaseAddress(const std::wstring& modName) {
		return reinterpret_cast<uintptr_t>(GetModuleHandleW(modName.c_str()));
	}

	u32 Platform::GetModuleSize(const char* module) {
		MODULEINFO module_info{};

		HMODULE base = GetModuleHandleA(module);
		if(base == nullptr || !GetModuleInformation(GetCurrentProcess(), base, &module_info, sizeof(MODULEINFO))) {
			return 0;
		}

		return module_info.SizeOfImage;
	}

	void* Platform::LoadModule(const std::string& module) {
		return LoadLibraryA((module + ".dll").c_str());
	}

	bool Platform::CloseModule(void* module) {
		return FreeLibrary(reinterpret_cast<HMODULE>(module));
	}

	void* Platform::GetModuleFunction(void* module, const std::string& function) {
		return GetProcAddress(reinterpret_cast<HMODULE>(module), function.c_str());
	}

	uintptr_t Platform::GetMinecraftBaseAddress() {
		static uintptr_t base = GetModuleBaseAddress("Minecraft.Windows.exe");
		return base;
	}

	void Platform::OutputDebugMessage(const std::string& message) {
		OutputDebugStringA(message.c_str());
	}

	void Platform::OutputDebugMessage(const std::wstring& message) {
		OutputDebugStringW(message.c_str());
	}

	void Platform::DebugPause() {
		if (IsDebuggerPresent()) {
			DebugBreak();
		}
	}
}