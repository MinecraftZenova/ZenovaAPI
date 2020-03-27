#pragma once

#include "Zenova/Common.h"
#include "Zenova/Hook.h"
#include "Zenova/Log.h"
#include "Zenova/Mod.h"

#ifdef _WINDOWS
	#include "Zenova/OS/Windows.h"
#elif Linux
	#include "Zenova/OS/Linux.h"
#endif

//implement a string to wstring, switch to only use wstring
	
namespace Zenova {
#ifdef ZENOVA_API
	unsigned long __stdcall Start(void* dllHandle); //don't try to run this : )
#endif // ZENOVA_API

	namespace Util {
		std::wstring StrToWstr(const std::string& oldstr);
	}
}