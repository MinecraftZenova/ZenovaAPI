#include "Zenova/Log.h"


namespace Zenova {
	const wchar_t* Log::_serverityToString(Severity severity) {
		switch (severity) {
			case Severity::None:
			{
				return L"";
			}
			case Severity::Info:
			{
				return L"Info";
			}
			case Severity::Warning:
			{
				return L"Warning";
			}
			case Severity::Error:
			{
				return L"Error";
			}
			default:
			{
				return L"Unknown";
			}
		}
	}

	void Log::Write(Severity severity, const UniversalString& name, const UniversalString& format, const fmt::wformat_args& args) {
		static std::mutex logMutex;
		std::lock_guard<std::mutex> guard(logMutex);

		std::wstring s = _serverityToString(severity);
		if (s == L"") {
			s = fmt::format(L"[{}] ", name.wstr());
		}
		else {
			s = fmt::format(L"[{}] [{}] ", s, name.wstr());
		}

		s += fmt::vformat(format.wstr() + L"\n", args);

		Platform::OutputDebugMessage(s);
		std::wcout << s << std::flush;
	}
}