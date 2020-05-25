// This file was automatically generated using tools/process_csv.py
// Generated on Sun May 17 2020 06:49:43 UTC

#include <Zenova/Hook.h>


extern "C" {
	void* _fromString_UUID_mce__SA_AV12_AEBV_$basic_string_DU_$char_traits_D_std__V_$allocator_D_2__std___Z_ptr;
	void* __parseVersionToString_SemVersion__AEAAXXZ_ptr;
}

void InitBedrockPointers() {
	_fromString_UUID_mce__SA_AV12_AEBV_$basic_string_DU_$char_traits_D_std__V_$allocator_D_2__std___Z_ptr = reinterpret_cast<void*>(Zenova::Hook::SlideAddress(0x0E555A0));
	__parseVersionToString_SemVersion__AEAAXXZ_ptr = reinterpret_cast<void*>(Zenova::Hook::SlideAddress(0x0E2CCB0));
}