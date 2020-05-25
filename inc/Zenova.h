#pragma once

#include "Zenova/Common.h"
#include "Zenova/Hook.h"
#include "Zenova/Log.h"
#include "Zenova/Mod.h"
#include "Zenova/Platform.h"

namespace Zenova::Util {
	std::wstring StrToWstr(const std::string& oldstr);
	bool IsDirectory(const std::string& folder);
}