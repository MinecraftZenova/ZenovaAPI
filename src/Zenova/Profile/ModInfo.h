#pragma once

#include <string>
#include <vector>

namespace Zenova {
	class Mod;

    struct ModInfo {
		Mod* mMod = nullptr;
		void* mHandle = nullptr;
		std::string mNameId, mName = "", mDescription = "";
		std::string mVersion = "";
		std::vector<std::string> mMcVersion;

		ModInfo(const std::string& name);
		ModInfo(ModInfo&&) noexcept;
		~ModInfo();
	};
}