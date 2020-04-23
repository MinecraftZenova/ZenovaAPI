#pragma once

#include "Zenova/Common.h"
#include "Zenova/Hook.h"
#include "Zenova/Log.h"
#include "Zenova/Mod.h"
#include "Zenova/Platform/Platform.h"

#include <utility>

namespace Zenova {
#ifdef ZENOVA_API
	unsigned long __stdcall Start(void* dllHandle); //don't try to run this : )
#endif // ZENOVA_API

	namespace Util {
		std::wstring StrToWstr(const std::string& oldstr);
		bool IsDirectory(const std::string& folder);
	}
}