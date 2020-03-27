#pragma once

#include "Zenova/OS/OS.h"

namespace Zenova {
	class Linux : public OS {
	public:
		Linux();
		
		~Linux();
		virtual void* FindAddress(const std::string& module, const std::string& function);
		virtual bool CreateHook(void* address, void* funcJump, void** funcTrampoline);
		virtual void CreateFileStream(const std::string& path) = 0;

	#ifdef ZENOVA_API
		virtual void SetupOS(void* variables); //meant to only be run once through ZenovaAPI.dll
	#endif // ZENOVA_API
	};
}