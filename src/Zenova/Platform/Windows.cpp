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
#include <fcntl.h>

#include <array>
#include <algorithm>
#include <codecvt>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <tuple>

#include "Zenova.h"
#include "Zenova/Utils/Utils.h"

#include "MinHook.h"

namespace Zenova::PlatformImpl {
	inline void* CleanupVariables = nullptr;

	void MessageRedirection() {
		// Setup console logging
		if (!AllocConsole()) {
			Platform::ErrorPrinter();
			return;
		}

		SetConsoleTitleA("Zenova Log");
		if (!SetConsoleOutputCP(CP_UTF8)) {
			Platform::ErrorPrinter();
		}

		HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
		DWORD dwMode = 0;
		GetConsoleMode(hOut, &dwMode);
		dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
		SetConsoleMode(hOut, dwMode);

		FILE* dummy;
		freopen_s(&dummy, "CONIN$", "r", stdin);
		freopen_s(&dummy, "CONOUT$", "w", stderr);
		freopen_s(&dummy, "CONOUT$", "w", stdout);

		//Clear the error state for each of the C++ standard stream objects. 
		std::clog.clear();
		std::cout.clear();
		std::cerr.clear();
		std::cin.clear();
	}

	bool Init(void* platformArgs) {
		MessageRedirection();

		if (MH_Initialize() != MH_OK) {
			Zenova_Warn("MinHook failed to initialize, normal launch without hooks");
			return false;
		}

		CleanupVariables = platformArgs;

		return true;
	}

	void Destroy() {
		if (!FreeConsole()) {
			Platform::ErrorPrinter();
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

		Zenova_Error(message_buffer);
		LocalFree(message_buffer);
	}

	uintptr_t Platform::GetModuleBaseAddress(const char* module) {
		wchar_t* modName = (module) ? Util::CStrToCWstr(module) : NULL;
		return reinterpret_cast<uintptr_t>(GetModuleHandleW(modName));
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
		return LoadLibraryA(module.c_str());
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
		Zenova_Error(err.ErrorMessage());
		return L"";
	}

	std::string Platform::GetMinecraftFolder() {
		static std::string folder = Util::WstrToStr(FindMinecraftFolder());
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