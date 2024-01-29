#pragma once

#include "Zenova/Common.h"

namespace Zenova {
	namespace PlatformImpl {
		bool Init(void*);
		void Destroy();
		void* LoadModModuleAndResolveImports(const std::string& module);
		void ResolveModModuleImports(void* hModule, const std::string& moduleName);
	}
}