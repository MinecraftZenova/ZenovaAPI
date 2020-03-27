#include "Zenova/Log.h"

#include <iostream>
#include <sstream>

#include "Zenova.h"

namespace Zenova {
	std::string SeverityToString(LogSeverity severity) {
		switch(severity) {
			case LogSeverity::info: {
				return "Info";
			}
			case LogSeverity::warning: {
				return "Warning";
			}
			case LogSeverity::error: {
				return "Error";
			}
			default: { //Unknown/None Severity
				return "";
			}
		}
	}

	Message::Message(LogSeverity severity, std::tstring message, std::tstring name) : Message(SeverityToString(severity), message, name) {}

	Message::Message(std::tstring severity, std::tstring message, std::tstring name) {
		if(!name.empty()) {
			message.insert(0, L"[" + name + L"] ");
		}

		if(!severity.empty()) {
			message.insert(0, L"[" + severity + L"] ");
		}

		message += L"\n";

		OutputDebugStringW(message.c_str());
		std::wcout << message << std::flush;
	}
}