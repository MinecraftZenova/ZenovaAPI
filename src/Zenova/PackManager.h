#pragma once

#include <vector>
#include <string>
#include <utility>

enum class PackType : char;

namespace Zenova {
    class PackManager {
	public:
		struct Pack {
			std::string path;
			std::string uuid;
		};

		static bool addMod(const std::string& path);

		static const std::vector<Pack>& getResourcePacks();
		static const std::vector<Pack>& getBehaviorPacks();

		static const std::vector<Pack>& getPacks(PackType);
	};
}