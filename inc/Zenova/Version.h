#pragma once

#include "Zenova/Common.h"

#define VER(major, minor, patch, revision) \
    ((major << 0x30) | (minor << 0x20) | (patch << 0x10) | revision)

#ifdef GAME_MAJOR
#define GAME_VER VER(GAME_MAJOR, GAME_MINOR, GAME_PATCH, GAME_PATCH)
#endif

namespace Zenova {
    class ZENOVA_EXPORT Version {
        u16 mMajor = 0, mMinor = 0, mPatch = 0, mRevision = 0;

    public:
        static const Version& launched();

        Version(u16 major, u16 minor, u16 patch, u16 revision) :
            mMajor(major), mMinor(minor), mPatch(patch), mRevision(revision) {}

        Version(std::string_view versionStr) {
            *this = fromString(versionStr);
        }

        bool operator==(const Version& rhs) const {
            return _buildVersion() == rhs._buildVersion();
        }

        bool operator==(std::string_view versionStr) const {
            return (*this == fromString(versionStr));
        }

        bool operator!=(const Version& rhs) const {
            return _buildVersion() != rhs._buildVersion();
        }

        bool operator<(const Version& rhs) const {
            return _buildVersion() < rhs._buildVersion();
        }

        bool operator<=(const Version& rhs) const {
            return _buildVersion() <= rhs._buildVersion();
        }

        bool operator>(const Version& rhs) const {
            return _buildVersion() > rhs._buildVersion();
        }

        bool operator>=(const Version& rhs) const {
            return _buildVersion() >= rhs._buildVersion();
        }

        std::string toString() const;
        static Version fromString(std::string_view versionStr);

    private:
        const u64 _buildVersion() const {
            return VER(u64(mMajor), u64(mMinor), u64(mPatch), u64(mRevision));
        }
    };
}