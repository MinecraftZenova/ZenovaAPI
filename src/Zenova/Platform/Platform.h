#pragma once

#include <memory>
#include <string>

#include "Zenova/Common.h"
#include "Zenova/Mod.h"

namespace Zenova {
	namespace Platform {
		enum class PlatformType {
			Unknown,
			Windows,
			Linux,
		};
		extern PlatformType Type;
		std::string TypeToString(const PlatformType& type);

		//Reorganize this somehow :)
		EXPORT void* FindAddress(const std::string& module, const std::string& function);
		EXPORT bool CreateHook(void* address, void* funcJump, void** funcTrampoline);
		EXPORT bool CreateHook(const std::string& module, const std::string& function, void* funcJump, void** funcTrampoline);
		EXPORT void CreateFileStream(const std::string& path);
		EXPORT u32 GetRWProtect();
		EXPORT u32 SetPageProtect(void* addr, size_t len, u32 prot);
		EXPORT size_t GetVtableSize(uintptr_t** vtableToCheck);
		EXPORT void ErrorPrinter();
		EXPORT uintptr_t GetModuleBaseAddress(const std::string& modName);
		EXPORT uintptr_t GetModuleBaseAddress(const std::wstring& modName);
		EXPORT u32 GetModuleSize(const char*);
		EXPORT void DebugPause();
		EXPORT void OutputDebugMessage(const std::string& message); //Visual Studio, LogCat, something else?
		EXPORT void OutputDebugMessage(const std::wstring& message);
		EXPORT void* LoadModule(const std::string& module);
		EXPORT bool CloseModule(void*);
		EXPORT void* GetModuleFunction(void* module, const std::string& function);

	#ifdef ZENOVA_API
		inline void* CleanupVariables = nullptr;

		bool Init(void*);
		void Destroy();
	#endif // ZENOVA_API
	};
}