#define _CRT_SECURE_NO_WARNINGS // std::getenv

#include "PlatformImpl.h"
#include "Zenova/Platform.h"

#include <Windows.h>
#include <TlHelp32.h>
#include <sddl.h>
#include <aclapi.h>
#include <Psapi.h>
#include <direct.h>
#include <Shlobj.h>
#include <comdef.h>

#include <array>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <tuple>

#include "Zenova.h"
#include "Zenova/Globals.h"
#include "Zenova/Utils/Utils.h"
#include "Zenova/Utils/Memory.h"

#include "MinHook.h"

#if 0
#define ZenovaDLog(severity, format, ...) logger.write(severity, format, __VA_ARGS__)
#else
#define ZenovaDLog(severity, format, ...)
#endif

namespace Zenova::PlatformImpl {
	inline void* CleanupVariables = nullptr;
	inline FILE* console = nullptr;

	void MessageRedirection() {
		// Setup console logging
		if (!AllocConsole()) {
			Platform::ErrorPrinter();
			return;
		}

		if (!console) {
			// Handle std::cout, std::clog, std::cerr, std::cin
			freopen_s(&console, "CONOUT$", "w", stdout);
			freopen_s(&console, "CONOUT$", "w", stderr);
			freopen_s(&console, "CONIN$", "r", stdin);

			HANDLE hConOut = CreateFileA("CONOUT$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			HANDLE hConIn = CreateFileA("CONIN$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			SetStdHandle(STD_OUTPUT_HANDLE, hConOut);
			SetStdHandle(STD_ERROR_HANDLE, hConOut);
			SetStdHandle(STD_INPUT_HANDLE, hConIn);

			std::cout.clear();
			std::clog.clear();
			std::cerr.clear();
			std::cin.clear();
			std::wcout.clear();
			std::wclog.clear();
			std::wcerr.clear();
			std::wcin.clear();
		}
	}

	bool Init(void* platformArgs) {
		MessageRedirection();

		if (MH_Initialize() != MH_OK) {
			logger.warn("MinHook failed to initialize, normal launch without hooks");
			return false;
		}

		CleanupVariables = platformArgs;

		return true;
	}

	void Destroy() {
		if (!FreeConsole()) {
			Platform::ErrorPrinter();
		}

		if (console) {
			fclose(console);
		}

		MH_Uninitialize();

		if (CleanupVariables) {
			FreeLibraryAndExitThread(reinterpret_cast<HMODULE>(CleanupVariables), 0);
		}
	}
}

namespace Zenova {
	const PlatformType Platform::Type = PlatformType::Windows;

	void* Platform::FindAddress(const char* module, const char* function) {
		HMODULE hModule = GetModuleHandleA(module);
		if(hModule == NULL)
			return 0;

		FARPROC hFunction = GetProcAddress(hModule, function);
		if(hFunction == NULL)
			return 0;

		return hFunction;
	}

	inline std::unordered_map<void*, void**> hooks;

	bool Platform::CreateHook(void* address, void* funcJump, void* _funcTrampoline) {
		auto funcTrampoline = reinterpret_cast<void**>(_funcTrampoline);

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
		return enum_cast(a) & enum_cast(b);
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

	// Convert GetLastError() to an actual string
	void Platform::ErrorPrinter() {
		LPSTR message_buffer = nullptr;

		FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
					   nullptr, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&message_buffer, 0, nullptr);
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



	void* Platform::LoadModModuleAndResolveImports(const ModInfo& modInfo, const std::string& module)
	{
		void* hModule = LoadLibraryExA((module + ".dll").c_str(), NULL, DONT_RESOLVE_DLL_REFERENCES);
		ResolveModModuleImports(modInfo, hModule, module);
		return hModule;
	}

	typedef BOOL(APIENTRY* DllMainFunction)(HMODULE, DWORD, LPVOID);
	void Platform::ResolveModModuleImports(const ModInfo& modInfo, void* hModule, const std::string& moduleName)
	{
		IMAGE_DOS_HEADER* pDOSHeader = (IMAGE_DOS_HEADER*)hModule;
		IMAGE_NT_HEADERS* pNTHeaders = (IMAGE_NT_HEADERS*)((BYTE*)pDOSHeader + pDOSHeader->e_lfanew);
		IMAGE_DATA_DIRECTORY importDir = pNTHeaders->OptionalHeader.DataDirectory[1];
		IMAGE_IMPORT_DESCRIPTOR* imports = reinterpret_cast<IMAGE_IMPORT_DESCRIPTOR*>((uintptr_t)hModule + importDir.VirtualAddress);

		while (imports->Characteristics != 0) {
			PIMAGE_THUNK_DATA thunkILT = (PIMAGE_THUNK_DATA)((uintptr_t)hModule + imports->OriginalFirstThunk);
			PIMAGE_THUNK_DATA thunkIAT = (PIMAGE_THUNK_DATA)((uintptr_t)hModule + imports->FirstThunk);
			LPCSTR moduleName = reinterpret_cast<LPCSTR>((uintptr_t)hModule + imports->Name);
			std::string importModNameId(moduleName);
			{
				size_t dot = importModNameId.find_last_of('.');
				if (dot != std::string::npos) {
					importModNameId = importModNameId.substr(0, dot);
				}
			}
			
			std::unordered_map<std::string, ULONGLONG*> iatAddresses{};
			while (thunkILT->u1.AddressOfData != 0) {
				if (!(thunkILT->u1.Ordinal & IMAGE_ORDINAL_FLAG)) {
					PIMAGE_IMPORT_BY_NAME nameData =
						reinterpret_cast<PIMAGE_IMPORT_BY_NAME>((uintptr_t)hModule + thunkILT->u1.AddressOfData);

					iatAddresses.insert({ nameData->Name, &thunkIAT->u1.Function });
				}
				thunkILT++;
				thunkIAT++;
			}

			uintptr_t moduleBase = Platform::GetModuleBaseAddress(moduleName);
			if (!moduleBase) {
				moduleBase = (uintptr_t)manager.loadMod(importModNameId);
				if (moduleBase)
					ZenovaDLog(Log::Severity::Info, "Loaded {} earlier because {} depends on it.", importModNameId, modInfo.mNameId);
				else
					moduleBase = (uintptr_t)LoadLibraryA(moduleName);
			}

			if (moduleBase != 0) {
				for (auto& [name, address] : iatAddresses) {
					FARPROC proc = (FARPROC)Platform::GetModuleFunction((void*)moduleBase, name.c_str());
					if (proc) {
						Memory::WriteOnProtectedAddress(address, &proc, sizeof(ULONGLONG));
					}
				}
			}
			else {
				ZenovaDLog(Log::Severity::Error, "Could not load the IAT {}::{} because the dependency or module {} doesn't exist.", modInfo.mNameId, importModNameId, importModNameId);
			}
			
			imports++;
		}
		
		DllMainFunction dllMain = reinterpret_cast<DllMainFunction>((uintptr_t)hModule + pNTHeaders->OptionalHeader.AddressOfEntryPoint);
		if (dllMain) {
			dllMain((HMODULE)hModule, DLL_PROCESS_ATTACH, nullptr);
		}
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

	uintptr_t Platform::GetMinecraftSize() {
		static uintptr_t size = GetModuleSize("Minecraft.Windows.exe");
		return size;
	}

	std::wstring FindMinecraftFolder() {
		// todo: have to change this later to meet 1607 expectations of really long filepaths
		std::array<wchar_t, 1000> szPathW;
		HRESULT code = SHGetFolderPathW(NULL, CSIDL_LOCAL_APPDATA, NULL, SHGFP_TYPE_CURRENT, szPathW.data());
		if (SUCCEEDED(code)) {
			// Get the path to the textures folder
			std::wstring_view szPathView(szPathW.data());
			std::wstring appData(szPathView.substr(0, szPathView.rfind(L"AC")));
			appData += L"LocalState/games/com.mojang/";

			if (std::filesystem::is_symlink(appData)) {
				appData = std::filesystem::read_symlink(appData).wstring();
			}
			else {
				std::wstring_view replaced = L"microsoft.minecraftuwp";
				appData.replace(appData.find(replaced), replaced.length(), L"Microsoft.MinecraftUWP");
			}

			return std::filesystem::absolute(appData).wstring();
		}

		_com_error err(code);
		logger.error(err.ErrorMessage());
		return L"";
	}

	std::wstring Platform::GetMinecraftFolder() {
		static std::wstring folder = FindMinecraftFolder();
		return folder;
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

	// should this be accessible through Platform?
	// todo: make this safe and without MAX_PATH?
	std::string GetModulePath(const char* moduleName) {
		char szPath[MAX_PATH];
		GetModuleFileNameA(GetModuleHandleA(moduleName), szPath, MAX_PATH);
		return std::filesystem::path(szPath).remove_filename().string();
	}

	std::string FindZenovaFolder() {
		std::string paths[] = {
			// standard install
			std::getenv("ZENOVA_DATA"),
			// standalone install
			GetModulePath("ZenovaAPI.dll") + "\\data",
		};

		for (auto& pathstr : paths) {
			if (!pathstr.empty() && Util::IsFile(pathstr + "\\ZenovaAPI.dll")) {
				return pathstr;
			}
		}

		// todo: log
		return "";
	}

	std::string Platform::GetZenovaFolder() {
		static std::string folder = FindZenovaFolder();
		return folder;
	}
}