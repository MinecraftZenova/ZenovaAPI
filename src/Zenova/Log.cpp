#include "Zenova/Log.h"

namespace Zenova {
	namespace {
		const char* serverityToString(Log::Severity severity) {
			switch (severity) {
				case Log::Severity::Info: return "Info";
				case Log::Severity::Warning: return "Warning";
				case Log::Severity::Error: return "Error";
				default: return "Unknown";
			}
		}

		auto serverityToColor(Log::Severity severity) {
			switch (severity) {
				case Log::Severity::Info: return fmt::fg(fmt::color::green);
				case Log::Severity::Warning: return fmt::fg(fmt::color::yellow);
				case Log::Severity::Error: return fmt::fg(fmt::color::red);
				default: return fmt::fg(fmt::color::white);
			}
		}
	}

	void Log::Write(Severity severity, const std::string& name, const std::string& out) {
		static std::mutex logMutex;
		std::lock_guard<std::mutex> guard(logMutex);

		std::string s = "";
		if (severity != Log::Severity::None) {
			s += fmt::format("[{}] ", fmt::styled(serverityToString(severity), serverityToColor(severity)));
		}

		s += fmt::format("[{}] {}\n", name, out);
		
		Platform::OutputDebugMessage(s);
		std::cout << s << std::flush;
	}
}