#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING //:)

#include "Zenova/Common.h"
#include <filesystem>
#include <locale>
#include <codecvt>

namespace Zenova {
	UniversalString::UniversalString() : s(L"") {}
	UniversalString::UniversalString(const std::string& pstr) : s(Util::StrToWstr(pstr)) {}
	UniversalString::UniversalString(const std::wstring& pwstr) : s(pwstr) {}
	UniversalString::UniversalString(const char* pstr) : s(Util::StrToWstr(pstr)) {}
	UniversalString::UniversalString(const wchar_t* pwstr) : s(pwstr) {}

	std::string UniversalString::str() const { return Util::WstrToStr(s); }
	std::wstring UniversalString::wstr() const { return s; }
}