#pragma once

#include <string>
#include <vector>

#include <Zenova/Hook.h>

namespace Zenova {
    struct ModInfo {
		std::string mNameId, mName = "", mDescription = "";
		std::string mVersion = "";

		void* mHandle = nullptr;
		std::string folder;

		ModInfo(const std::string& modFolder, const std::string& nameId = "");
		~ModInfo();
	};
}