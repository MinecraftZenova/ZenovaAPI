#pragma once

#include "Zenova/Common.h"

namespace Zenova {
	class StorageResolver {
	public:
		std::string minecraft_path_s;
		std::wstring minecraft_path_w;
		UniversalString moved_minecraft_path;

		StorageResolver();
		void setPath(const UniversalString& directory);
	};

	namespace PlatformImpl {
		bool Init(void*);
		void Destroy();
	}
}