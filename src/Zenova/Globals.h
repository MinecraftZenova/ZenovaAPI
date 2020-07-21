#pragma once

#include <string>

#include "Zenova/Log.h"
#include "Zenova/Profile/Manager.h"
#include "Zenova/Platform/PlatformImpl.h"

namespace Zenova {
	inline Manager manager;
	inline std::string dataFolder("");
	inline Log logger("Zenova");
	inline StorageResolver storage;
}