#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING //:)

#include "Zenova/Common.h"
#include <filesystem>
#include <locale>
#include <codecvt>

namespace Zenova {
	UniversalString::UniversalString() : wstr(L"") {}
	UniversalString::UniversalString(const std::string& pstr) : wstr(Util::StrToWstr(pstr)) {}
	UniversalString::UniversalString(const std::wstring& pwstr) : wstr(pwstr) {}
	UniversalString::UniversalString(const char* pstr) : wstr(Util::StrToWstr(pstr)) {}
	UniversalString::UniversalString(const wchar_t* pwstr) : wstr(pwstr) {}
}