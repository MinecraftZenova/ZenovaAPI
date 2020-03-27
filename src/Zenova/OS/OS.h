#pragma once

#include <memory>
#include <string>

#include "Zenova/Common.h"
#include "Zenova/Mod.h"

namespace Zenova {
	enum class OSType {
		Windows,
		Linux,
		Unknown
	};

	class OS {
		OS() = delete;

	protected:
		OS(OSType type);

	public:
		OSType type;

		std::string OSTypeToString(const OSType& type);

		virtual void* FindAddress(const std::string& module, const std::string& function) = 0;
		virtual bool CreateHook(void* address, void* funcJump, void** funcTrampoline) = 0;
		virtual bool CreateHook(const std::string& module, const std::string& function, void* funcJump, void** funcTrampoline);
		virtual void CreateFileStream(const std::string& path) = 0;
		virtual Mod* LoadMod(const std::string& mod) = 0;
		virtual u32 GetRWProtect() = 0;
		virtual u32 SetPageProtect(void* addr, size_t len, u32 prot) = 0;
		virtual size_t GetVtableSize(uintptr_t** vtableToCheck) = 0;
		//createdir

	#ifdef ZENOVA_API
		virtual void SetupOS(void* variables) = 0; //Only to be run once through ZenovaAPI.dll
	#endif // ZENOVA_API
	};

	extern std::shared_ptr<OS> hostOS; //Initalized in Zenova.cpp
}