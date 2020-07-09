#pragma once

#include <sstream>

#include "Zenova/Common.h"

namespace Zenova {
    class EXPORT Version {
        u16 mMajor = 0, mMinor = 0, mPatch = 0, mRevision = 0;

    public:
        Version(u16 major, u16 minor, u16 patch, u16 revision);
        Version(std::string_view versionStr);
        std::string toString() const;
        bool operator==(const Version& rhs) const;
        bool operator==(std::string_view rhs) const;
        bool operator!=(const Version& rhs) const;
        bool operator<(const Version& rhs) const;
        bool operator<=(const Version& rhs) const;
        bool operator>(const Version& rhs) const;
        bool operator>=(const Version& rhs) const;

        static Version fromString(std::string_view versionStr);

    private:
        const u64 _buildVersion() const;
    };
}