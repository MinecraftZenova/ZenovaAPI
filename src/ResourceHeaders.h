#pragma once

#include <string>

namespace Core {
	typedef std::string HeapPathBuffer;
	typedef std::string Path;
}

enum class PackType : char {
	Behavior = 4,
	Resources = 6
};

enum class PackCategory : int {
	Custom = 4
}; 

class IInPackagePacks {
public:
	struct MetaData {
		Core::HeapPathBuffer mPath;
		bool mIsHidden;
		PackCategory mPackCategory;

		MetaData(const Core::Path& path, const bool isHidden, PackCategory packCategory) :
			mPath(path), mIsHidden(isHidden), mPackCategory(packCategory) {}
	};
};

namespace mce {
	class UUID {
		uint64_t Data[2];

	public:
		static UUID fromString(const std::string&);

		uint64_t getMostSignificantBits() const {
			return Data[0];
		}

		uint64_t getLeastSignificantBits() const {
			return Data[1];
		}

		std::string asString() const {
			std::stringstream stream;
			stream << std::setfill('0') << std::setw(8) << std::hex << uint32_t(getMostSignificantBits() >> 32) << '-';
			stream << std::setfill('0') << std::setw(4) << std::hex << uint16_t(getMostSignificantBits() >> 16) << '-';
			stream << std::setfill('0') << std::setw(4) << std::hex << uint16_t(getMostSignificantBits()) << '-';
			stream << std::setfill('0') << std::setw(4) << std::hex << uint16_t(getLeastSignificantBits() >> 48) << '-';
			stream << std::setfill('0') << std::setw(8) << std::hex << uint32_t(getLeastSignificantBits() >> 16);
			stream << std::setfill('0') << std::setw(8) << std::hex << uint16_t(getLeastSignificantBits());

			return stream.str();
		}
	};
}

class SemVersion {
	uint16_t mMajor;
	uint16_t mMinor;
	uint16_t mPatch;
	std::string mPreRelease;
	std::string mBuildMeta;
	std::string mFullVersionString;
	bool mValidVersion;
	bool mAnyVersion;

	void _parseVersionToString();

public:
	SemVersion() {
		mMajor = 0;
		mMinor = 0;
		mPatch = 0;
		mValidVersion = false;
		mAnyVersion = false;
		_parseVersionToString();
	}

	SemVersion(uint16_t major, uint16_t minor, uint16_t patch, std::string preRelease = "", std::string buildMeta = "") {
		mMajor = major;
		mMinor = minor;
		mPatch = patch;
		mPreRelease = preRelease;
		mBuildMeta = buildMeta;
		mValidVersion = true;
		mAnyVersion = false;
		_parseVersionToString();
	}
};

struct PackIdVersion {
	mce::UUID mId;
	SemVersion mVersion;
	PackType mPackType;

	PackIdVersion(mce::UUID id, SemVersion version, PackType packType) {
		mId = id;
		mVersion = version;
		mPackType = packType;
	}
};

class ResourcePackRepository {};
class ResourcePackStack {};
class VanillaInPackagePacks {};
struct GameRules {};
struct VanillaGameModuleClient {};
struct VanillaGameModuleServer {};