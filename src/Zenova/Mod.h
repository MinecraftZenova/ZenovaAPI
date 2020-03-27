#pragma once

#include <string>

#include "Common.h"

namespace Zenova {
	class EXPORT Mod {
		const char *name, *description;

		struct Version {
			u8 major, minor, patch;
		} version;

	protected:
		Mod(const char* name, Version version, const char* description);
	
	public:
		virtual ~Mod();
		virtual void Start() = 0;
		virtual void Update() = 0;
		virtual void Stop() = 0;
	};
}