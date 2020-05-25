#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING //:)

#include "Zenova/Common.h"
#include <filesystem>
#include <locale>
#include <codecvt>

namespace Zenova {
	namespace Util {
		std::string WstrToStr(const std::wstring& oldwstr) {
			return std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(oldwstr);
		}

		std::wstring StrToWstr(const std::string& oldstr) {
			return std::wstring(oldstr.begin(), oldstr.end());
		}

		bool IsDirectory(const std::string& folder) {
			return std::filesystem::is_directory(std::filesystem::status(folder));
		}
	}
	
	UniversalString::UniversalString() : wstr(L"") {}
	UniversalString::UniversalString(const std::string& pstr) : wstr(Util::StrToWstr(pstr)) {}
	UniversalString::UniversalString(const std::wstring& pwstr) : wstr(pwstr) {}
	UniversalString::UniversalString(const char* pstr) : wstr(Util::StrToWstr(pstr)) {}
	UniversalString::UniversalString(const wchar_t* pwstr) : wstr(pwstr) {}
}