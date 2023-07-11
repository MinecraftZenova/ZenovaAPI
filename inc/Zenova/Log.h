#pragma once

#include <iostream>
#include <mutex>
#include <utility>

#define FMT_HEADER_ONLY
#include <fmt/format.h>
#include <fmt/color.h>

#include "Common.h"
#include "Platform.h"

namespace Zenova {
	class Log {
	public:
		enum class Severity : int {
			None,
			Info,
			Warning,
			Error
		};

		static ZENOVA_EXPORT void Write(Severity severity, const std::string& name, const std::string& out);

		template <typename... Args>
		static void Write(Severity severity, const std::string& name, const std::string& format, const Args&... args) {
			auto out = fmt::format(format, args...);
			Write(severity, name, out); 
		}
	};
}

#ifdef LOG_CONTEXT
#define Zenova_Log(type, f, ...) Zenova::Log::Write(Zenova::Log::Severity::type, LOG_CONTEXT, f, __VA_ARGS__)
#else
#define Zenova_Log(type, f, ...) Zenova::Log::Write(Zenova::Log::Severity::type, FSIG, f, __VA_ARGS__)
#endif
#define Zenova_Info(f, ...) Zenova_Log(Info, f, __VA_ARGS__)
#define Zenova_Warn(f, ...) Zenova_Log(Warning, f, __VA_ARGS__)
#define Zenova_Error(f, ...) Zenova_Log(Error, f, __VA_ARGS__)