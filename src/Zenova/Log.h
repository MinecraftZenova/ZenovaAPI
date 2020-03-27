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
		Message(LogSeverity severity, std::tstring message, std::tstring name);
		Message(std::tstring severity, std::tstring message, std::tstring name);
	};

	#define Info(message) Message(LogSeverity::info, message, __FUNCTION__)
	#define Warn(message) Message(LogSeverity::warning, message, __FUNCTION__)
	#define Error(message) Message(LogSeverity::error, message, __FUNCTION__)
}