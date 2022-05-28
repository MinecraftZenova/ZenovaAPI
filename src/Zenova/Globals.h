#pragma once

#include "Zenova/Log.h"
#include "Zenova/Profile/Manager.h"
#include "Zenova/Platform/PlatformImpl.h"

namespace Zenova {
	inline Manager manager;
	inline Log logger("Zenova");
	inline StorageResolver storage;
}