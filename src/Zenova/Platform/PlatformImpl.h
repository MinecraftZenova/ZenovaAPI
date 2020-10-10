#pragma once

#include "Zenova/Common.h"

namespace Zenova {
	class StorageResolver {
	public:
		std::wstring minecraft_path;
		std::wstring moved_minecraft_path;

		StorageResolver();
		void setPath(const UniversalString& directory);
	};

	namespace PlatformImpl {
		bool Init(void*);
		void Destroy();
	}
}