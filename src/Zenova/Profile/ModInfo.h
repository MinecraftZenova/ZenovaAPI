#pragma once

#include <string>

namespace Zenova {
	class Mod;

    struct ModInfo {
		Mod* mMod = nullptr;
		void* mHandle = nullptr;
		std::string mNameId, mName = "", mDescription = "";
		std::string mMinVersion, mMaxVersion, mVersion = "";

		ModInfo(const std::string& name);
		~ModInfo();
	};
}