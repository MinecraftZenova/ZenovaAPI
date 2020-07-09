#include "Zenova/Minecraft/Version.h"

#include <array>
#include <algorithm>
#include <charconv>
#include <string_view>
#include <sstream>

namespace Zenova {
	Version::Version(u16 major, u16 minor, u16 patch, u16 revision) : mMajor(major), mMinor(minor), mPatch(patch), mRevision(revision) {}

	Version::Version(std::string_view ver) {
		*this = fromString(ver);
	}

	std::string Version::toString() const {
		std::stringstream stream;
		stream << mMajor << "." << mMinor << "." << mPatch << "." << mRevision;
		return stream.str();
	}

	bool Version::operator==(const Version& rhs) const {
		return (mMajor == rhs.mMajor) && (mMinor == rhs.mMinor) && (mPatch == rhs.mPatch) && (mRevision == rhs.mRevision);
	}

	bool Version::operator==(std::string_view rhs) const {
		return *this == fromString(rhs);
	}

	bool Version::operator!=(const Version& rhs) const {
		return !(*this == rhs);
	}

	bool Version::operator<(const Version& rhs) const {
		return _buildVersion() < rhs._buildVersion();
	}

	bool Version::operator<=(const Version& rhs) const {
		if (*this == rhs) {
			return true;
		}

		return *this < rhs;
	}

	bool Version::operator>(const Version& rhs) const {
		return _buildVersion() > rhs._buildVersion();
	}

	bool Version::operator>=(const Version& rhs) const {
		if (*this == rhs) {
			return true;
		}

		return *this > rhs;
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

	const u64 Version::_buildVersion() const {
		return u64(mMajor) << 0x30 | u64(mMinor) << 0x20 | u64(mPatch) << 0x10 | u64(mRevision);
	}
}