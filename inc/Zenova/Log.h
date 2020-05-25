#pragma once

#include <vector>

#include "Common.h"

namespace Zenova {
	enum class LogSeverity : int {
		none = 0,
		info,
		warning,
		error
	};

	std::string SeverityToString(LogSeverity severity);

	class EXPORT Message {
	public:
		Message(LogSeverity severity, UniversalString message, UniversalString name);
		Message(UniversalString severity, UniversalString message, UniversalString name);
	};
}

#define Zenova_Info(zenova_macro_message) Zenova::Message(Zenova::LogSeverity::info, zenova_macro_message, FSIG)
#define Zenova_Warn(zenova_macro_message) Zenova::Message(Zenova::LogSeverity::warning, zenova_macro_message, FSIG)
#define Zenova_Error(zenova_macro_message) Zenova::Message(Zenova::LogSeverity::error, zenova_macro_message, FSIG)