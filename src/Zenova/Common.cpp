#include "Zenova/Common.h"

namespace Zenova {
	namespace Util {
		std::wstring StrToWstr(const std::string& oldstr) {
			return std::wstring(oldstr.begin(), oldstr.end());
		}
	}
}

namespace std {
	tstring::tstring(const std::string& _str) : wstring(Zenova::Util::StrToWstr(_str)), strData(_str) {}
	tstring::tstring(const char* _str) : wstring(Zenova::Util::StrToWstr(_str)), strData(_str) {}

	tstring::tstring(const std::wstring& _str) : wstring(_str), strData("") {}

	const string& tstring::str() {
		return strData;
	}
}