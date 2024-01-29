#pragma once

namespace Zenova {
	class Memory {
	public:
		static void WriteOnProtectedAddress(void* address, void* data, size_t dataSize);
	};
}