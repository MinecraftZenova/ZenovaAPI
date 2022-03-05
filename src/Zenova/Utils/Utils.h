#pragma once

#include <string>

namespace Util {
	/* Converts a C string to a wchar_t* */
	wchar_t* CStrToCWstr(const char* charptr);
	/* Converts a wstring to a C++ string */
	std::string WstrToStr(const std::wstring&);
	/* Converts a standard string to a wide string */
	std::wstring StrToWstr(const std::string&);

	/* FolderUtils */
	bool IsDirectory(const std::string& folder);
	bool IsFile(const std::string& file);
	std::wstring GetAppDirectoryW();
	std::string GetAppDirectoryA();
};