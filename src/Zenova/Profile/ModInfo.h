#pragma once

#include <string>
#include <vector>

namespace Zenova {
	class Mod;

    struct ModInfo {
		void* mHandle = nullptr;
		std::string mNameId, mName = "", mDescription = "";
		std::string mVersion = "";

		ModInfo(const std::string& modFolder);
		ModInfo(ModInfo&&) noexcept;
		~ModInfo();
	};
}