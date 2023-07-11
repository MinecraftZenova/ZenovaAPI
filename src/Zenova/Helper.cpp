#define LOG_CONTEXT "Zenova"

#include "Helper.h"

#include <iostream> //std::cout (Zenova::MessageRedirection)
#include <algorithm> //std::find_if
#include <fstream>
#include <cstdio>
#include <chrono>
#include <cstdlib>

#include "Zenova.h"
#include "Zenova/JsonHelper.h"
#include "Zenova/Platform/PlatformImpl.h"
#include "Zenova/Profile/Manager.h"
#include "Utils/Utils.h"

namespace Zenova {
	bool start(void* platformArgs) {
		Platform::DebugPause();

		bool run = (PlatformImpl::Init(platformArgs) && !manager.dataFolder.empty());
		if (run) {
			Zenova_Info("Zenova Started");
			Zenova_Info("ZenovaData Location: {}", manager.dataFolder);

			manager.init();
		}
		return run;
	}

	u32 run() {
		using namespace std::chrono_literals;

		// a bit hacky but wait for static variables to be initialized
		// todo: hook into minecraft's main and signal this thread to unblock?
		std::this_thread::sleep_for(2s);

		manager.run();
		
		while (true) {
			manager.update();
		}

		Zenova_Info("Zenova Stopped");

		PlatformImpl::Destroy();
		return 0;
	}
}