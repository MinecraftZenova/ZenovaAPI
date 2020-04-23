#pragma once

#include <string>

#include "Common.h"

#define MOD_FUNCTION extern "C" __declspec(dllexport)

namespace Zenova {
	class EXPORT Mod {
	protected:
		Mod();
	
	public:
		virtual ~Mod();
		virtual void Start() = 0;
		virtual void Update() = 0;
		virtual void Tick() = 0;
		virtual void Stop() = 0;
	};
}