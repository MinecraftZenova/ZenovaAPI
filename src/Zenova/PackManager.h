#pragma once

#include <vector>
#include <string>
#include <utility>

namespace Zenova {
    class PackManager {
	public:
		static bool addMod(const std::string& path);

		static const std::vector<std::pair<std::string, std::string>>& getResourcePacks();
		static const std::vector<std::pair<std::string, std::string>>& getBehaviorPacks();
	};
}