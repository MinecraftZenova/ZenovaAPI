#pragma once

#include "Common.h"
#include "Minecraft/Version.h"

namespace Zenova {
    class EXPORT Minecraft {
    public:
        static const Version& version();

        static const Version v1_14_60_5;
        static const Version v1_16_0_2;
        static const Version v1_16_1_2;
    };
}