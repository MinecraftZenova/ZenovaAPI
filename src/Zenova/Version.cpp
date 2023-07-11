#include "Zenova/Version.h"

#include <array>
#include <algorithm>
#include <charconv>
#include <string_view>
#include <sstream>

#include <Zenova/Profile/Manager.h>

namespace Zenova {

	const Version& Version::launched() {
		static Version ver = manager.getVersion();
		return ver;
	}

	std::string Version::toString() const {
		std::stringstream stream;
		stream << mMajor << "." << mMinor << "." << mPatch << "." << mRevision;
		return stream.str();
	}

	Version Version::fromString(std::string_view ver) {
		std::string_view delim = ".";
		std::array<uint16_t, 4> output{};
		auto first = ver.begin();

		for (int i = 0; first != ver.end() && i < 4; ++i) {
			auto second = std::find_first_of(first, ver.end(), delim.begin(), delim.end());
			if (first != second) {
				int num = 0;
				auto result = std::from_chars(ver.data() + std::distance(ver.begin(), first), ver.data() + std::distance(ver.begin(), second), num);
				if (result.ec == std::errc()) {
					output[i] = num;
				}
			}

			if (second == ver.end()) {
				break;
			}

			first = std::next(second);
		}

		return { output[0], output[1], output[2], output[3] };
	}
}