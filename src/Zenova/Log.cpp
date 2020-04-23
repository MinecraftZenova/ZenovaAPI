#include "Zenova/Log.h"

#include <iostream>
#include <sstream>

#include "Zenova.h"

namespace Zenova {
	std::string SeverityToString(LogSeverity severity) {
		switch(severity) {
			case LogSeverity::none: {
				return "";
			}
			case LogSeverity::info: {
				return "Info";
			}
			case LogSeverity::warning: {
				return "Warning";
			}
			case LogSeverity::error: {
				return "Error";
			}
			default: {
				return "Unknown";
			}
		}
	}

	Message::Message(LogSeverity severity, UniversalString message, UniversalString name) : Message(SeverityToString(severity), message, name) {}

	Message::Message(UniversalString severity, UniversalString message, UniversalString name) {
		if(!name.wstr.empty()) {
			message.wstr.insert(0, L"[" + name.wstr + L"] ");
		}

		if(!severity.wstr.empty()) {
			message.wstr.insert(0, L"[" + severity.wstr + L"] ");
		}

		message.wstr += L"\n";

		Platform::OutputDebugMessage(message.wstr);
		std::wcout << message.wstr << std::flush;
	}
}