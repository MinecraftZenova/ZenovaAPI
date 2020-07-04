#pragma once

#include <string>

#include "Zenova/Log.h"
#include "Zenova/Profile/Manager.h"

namespace Zenova {
	inline Manager manager;
	inline std::string dataFolder("");
	inline uintptr_t BaseAddress = 0;
	inline Log logger("Zenova");
}