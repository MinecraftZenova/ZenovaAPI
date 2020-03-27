#pragma once

#include "Zenova/OS/OS.h"

#include <Windows.h>
#include <iomanip> //to_hex_string
#include <sstream>
#include <direct.h>

#include "Zenova/Common.h"

namespace Zenova {
	class Windows : public OS {
		std::shared_ptr<DWORD> dllHandle;
		bool canHook;

	public:
		Windows();
		~Windows();

		virtual void* FindAddress(const std::string& module, const std::string& function);
		virtual bool CreateHook(void* address, void* funcJump, void** funcTrampoline);
		virtual void CreateFileStream(const std::string& path);
		virtual Mod* LoadMod(const std::string& mod);
		virtual u32 GetRWProtect();
		virtual u32 SetPageProtect(void* addr, size_t len, u32 prot);
		virtual size_t GetVtableSize(uintptr_t** vtableToCheck);

		static void EXPORT DebugPause();

	#ifdef ZENOVA_API
		virtual void SetupOS(void* variables); //meant to only be run once through ZenovaAPI.dll
	#endif // ZENOVA_API
	};
	
	extern HANDLE(__stdcall *pfnCreateFileW)(LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);
	extern HANDLE(__stdcall *pfnCreateDirectoryW)(LPCWSTR, LPSECURITY_ATTRIBUTES);
	extern HANDLE(__stdcall *pfnCreateDirectoryA)(LPCSTR, LPSECURITY_ATTRIBUTES);
	HANDLE __stdcall CreateFileW(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
	HANDLE __stdcall CreateDirectoryW(LPCWSTR lpFileName, LPSECURITY_ATTRIBUTES lpSecurityAttributes);
	HANDLE __stdcall CreateDirectoryA(LPCSTR lpFileName, LPSECURITY_ATTRIBUTES lpSecurityAttributes);

	namespace Util {
		template<typename T>
		std::string to_hex_string(T num) {
			std::stringstream stream;
			stream << "0x" << std::setfill('0') << std::setw(sizeof(T)*2) << std::hex << num;
			return stream.str();
		}

		EXPORT uintptr_t GetModuleBaseAddressA(const char* modName);
		EXPORT uintptr_t GetModuleBaseAddressW(const wchar_t* modName);
		DWORD GetModuleSize(DWORD, const char*);

		//WinAPI to std::string
		std::string getAccessRightString(DWORD dwDesiredAccess);
		std::string getShareRightString(DWORD dwShareMode);
		std::string getCreationDispositionString(const DWORD& dwCreationDisposition);
		std::string getFlagsAndAttributesString(const DWORD& dwFlagsAndAttributes);
	}
}