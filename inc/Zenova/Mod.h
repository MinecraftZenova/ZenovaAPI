#pragma once

#include <string>

#include "Common.h"

#define MOD_FUNCTION extern "C" __declspec(dllexport)

//:)
#pragma warning(disable:4251)

namespace Zenova {
	class Manager;

	class EXPORT Mod {
	protected:
		Manager* manager;

		Mod();
	
	public:
		virtual ~Mod();
		virtual void Start() = 0;
		virtual void Update() = 0;
		virtual void Tick() = 0;
		virtual void Stop() = 0;
		void SetManager(Manager* parent) { manager = parent; }
		const Manager& GetManager() { return *manager; }
	};
}