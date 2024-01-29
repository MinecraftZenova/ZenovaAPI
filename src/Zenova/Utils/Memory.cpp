#include "Memory.h"
#include "Windows.h"

namespace Zenova {
	void Memory::WriteOnProtectedAddress(void* address, void* data, size_t size) {
		if (!address || !data || !size)
			return;

		DWORD old = 0;
		VirtualProtect(address, size, PAGE_EXECUTE_READWRITE, &old);
		memcpy_s(address, size, data, size);
		VirtualProtect(address, size, old, NULL);
	}
}