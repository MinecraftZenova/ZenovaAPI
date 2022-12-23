#pragma once

#include <iomanip>
#include <regex>
#include <string>
#include <sstream>
#include <vector>

class BaseGameVersion {};
class ResourcePackRepository {};
class ResourcePackStack {};
class GameRules {};

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
		UUID() {
			Data[0] = 0;
			Data[1] = 0;
		}
		
		UUID(uint64_t mostSignificantBits, uint64_t leastSignificantBits) {
			Data[0] = mostSignificantBits;
			Data[1] = leastSignificantBits;
		}

		static UUID fromString(const std::string& in) {
			uint64_t mostSignificantBits = 0, leastSignificantBits = 0;
			int count = 0;

			static std::regex UUIDRegex("[a-fA-F0-9]{8}-[a-fA-F0-9]{4}-[a-fA-F0-9]{4}-[a-fA-F0-9]{4}-[a-fA-F0-9]{12}");
			std::smatch regexMatch;
			if (std::regex_search(in, regexMatch, UUIDRegex) && 
				in.length() == regexMatch[0].length()) {
				for (uint32_t i = 0; i < in.length(); ++i) {
					char c = in[i];
					uint64_t ca = 0;
					
					if (c >= '0' && c <= '9') {
						ca = c - '0';
					}
					else if (c >= 'a' && c <= 'f') {
						ca = c - 'a' + 10;
					}
					else if (c >= 'A' && c <= 'F') {
						ca = c - 'A' + 10;
					}
					else if(c == '-') {
						continue;
					}
					else {
						break; //invalid character
					}

					if (++count > 16) {
						leastSignificantBits |= ca << (0x80 - (4 * count));
					}
					else {
						mostSignificantBits |= ca << (0x40 - (4 * count));
					}
				}

				return { mostSignificantBits, leastSignificantBits };
			}

			return {};
		}

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

	void _parseVersionToString() {
		std::stringstream stream;
		stream << mMajor << "." << mMinor << "." << mPatch;

		if (!mPreRelease.empty()) {
			stream << "-" << mPreRelease;
		}
		if (!mBuildMeta.empty()) {
			stream << "+" << mBuildMeta;
		}

		mFullVersionString = stream.str();
	}

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

class GameModuleClient {
public:
	enum class ResourceLoadingPhase : int {
		VRPhase,
		DefaultPhase,
		EducationPhase
	};
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

namespace lambda {
	struct Pack {
		ResourcePackRepository& repo;
		ResourcePackStack& tempStack;

		void addFromUUID(const PackIdVersion& packType);
	};
}

class VanillaInPackagePacks : public IInPackagePacks {
public:
	std::vector<IInPackagePacks::MetaData> getPacks(PackType) const; //virtual but I'm not dealing with that rn
};

// seems to be a renamed GameRules
class Experiments;

// todo: deal with this problem in the future
class VanillaGameModuleClient {
public:
	void initializeResourceStack(ResourcePackRepository&, ResourcePackStack&, const BaseGameVersion&);

	struct v1_16 {
		void initializeResourceStack(ResourcePackRepository&, ResourcePackStack&, const BaseGameVersion&, GameModuleClient::ResourceLoadingPhase);
	};

	struct v1_16_100 {
		void initializeResourceStack(const Experiments*, ResourcePackRepository&, ResourcePackStack&, const BaseGameVersion&, GameModuleClient::ResourceLoadingPhase);
	};
};

class VanillaGameModuleServer {
public:
	void initializeBehaviorStack(const GameRules&, ResourcePackRepository&, ResourcePackStack&, const BaseGameVersion&);
};