#pragma once

#include <cstdio>

namespace Zenova {
	class MessageRedirection { //currently used to make a console for the app
		FILE* console = nullptr;

	public:
		MessageRedirection();
		~MessageRedirection();
	};
}