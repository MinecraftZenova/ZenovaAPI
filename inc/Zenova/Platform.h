#pragma once

#include <memory>
#include <string>

#include "Zenova/Common.h"

//This honestly needs to be rewritten

namespace Zenova {
	enum class PlatformType : int {
		Unknown,
		Windows,
		Linux,
		Count
	};

	enum class ProtectionFlags : int {
		None = 1,
		Read = 2,
		Write = 4,
		Execute = 8
	};

	inline ProtectionFlags operator|(ProtectionFlags a, ProtectionFlags b) {
		return static_cast<ProtectionFlags>(static_cast<std::underlying_type_t<ProtectionFlags>>(a) | static_cast<std::underlying_type_t<ProtectionFlags>>(b));
	}

	class EXPORT Platform {
	public:
		static const PlatformType Type;

		static std::string TypeToString(const PlatformType& type);

		static void* FindAddress(const std::string& module, const std::string& function);
		static bool CreateHook(void* address, void* funcJump, void** funcTrampoline);
		static bool CreateHook(const std::string& module, const std::string& function, void* funcJump, void** funcTrampoline);
		
		static u32 SetPageProtect(void* addr, size_t len, u32 prot);
		static u32 SetPageProtect(void* addr, size_t len, ProtectionFlags prot);
		
		static void ErrorPrinter();

		static uintptr_t GetModuleBaseAddress(const std::string& modName);
		static uintptr_t GetModuleBaseAddress(const std::wstring& modName);
		static u32 GetModuleSize(const char*);
		static void* LoadModule(const std::string& module);
		static bool CloseModule(void*);
		static void* GetModuleFunction(void* module, const std::string& function);

		static uintptr_t GetMinecraftBaseAddress();

		static void OutputDebugMessage(const std::string& message); //Visual Studio, LogCat, something else?
		static void OutputDebugMessage(const std::wstring& message);
		static void DebugPause();
	};
}