#pragma once

#include "Common.h"

#include <string>

namespace Zenova {
    class EXPORT Minecraft {
        Minecraft();

    public:
        std::string mVersion;

        static const Minecraft& instance();
    };
}