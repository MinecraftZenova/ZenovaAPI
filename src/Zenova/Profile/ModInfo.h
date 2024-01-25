#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "Zenova/Minecraft/Version.h"

namespace Zenova {
	class Mod;

    struct ModInfo {
		void* mHandle = nullptr;
    	std::string mModFolder = "";
		std::string mNameId = "";
    	std::string mName = "";
    	std::string mDescription = "";
		std::string mVersion = "";
    	std::vector<std::string> mDependencies = {};

		ModInfo(const std::string& modFolder);
		ModInfo(ModInfo&&) noexcept;
		~ModInfo();

    	void* loadModule();
    	void loadDependencies() const;
	};
}
