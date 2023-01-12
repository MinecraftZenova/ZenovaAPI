#pragma once

#include <string>

#include "Common.h"

#define MOD_FUNCTION extern "C" __declspec(dllexport)

namespace Zenova {
	class EXPORT Mod {
	protected:
		struct Context {
			std::string folder;
		} ctx;

		Mod();
		NO_COPY_OR_MOVE(Mod);

	public:
		virtual ~Mod() = default;
		virtual void Start() = 0;
		virtual void Update() = 0;
		virtual void Tick() = 0;
		virtual void Stop() = 0;
	};
}