#pragma once

#include <string>
#include <vector>

#include "Zenova/JsonHelper.h"

namespace Zenova {
    struct ProfileInfo {
		std::string name, created, lastUsed, versionId, storagePath;
		std::vector<std::string> modNames;

		ProfileInfo() = default;
		ProfileInfo(const json::Value& profile);

		operator bool() const;
	};
}