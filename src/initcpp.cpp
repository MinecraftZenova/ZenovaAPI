// This file was automatically generated using tools/process_csv.py
// Generated on Thu Jun 18 2020 23:17:49 UTC

#include "initcpp.h"

extern "C" {
	void* _fromString_UUID_mce__SA_AV12_AEBV_$basic_string_DU_$char_traits_D_std__V_$allocator_D_2__std___Z_ptr;
	void* __parseVersionToString_SemVersion__AEAAXXZ_ptr;
	void* _getPacks_VanillaInPackagePacks__QEBA_AV_$vector_UMetaData_IInPackagePacks__V_$allocator_UMetaData_IInPackagePacks___std___std__W4PackType___Z_ptr;
	void* _initializeBehaviorStack_VanillaGameModuleServer__QEAAXAEBVGameRules__AEAVResourcePackRepository__AEAVResourcePackStack__AEBVBaseGameVersion___Z_ptr;
	void* _addPackFromPackId__YAXPEAULambdaPack1__AEBUPackIdVersion___Z_ptr;
	void* _initializeResourceStack_VanillaGameModuleClient__QEAAXAEAVResourcePackRepository__AEAVResourcePackStack__AEBVBaseGameVersion___Z_ptr;
	void* _initializeResourceStack2_VanillaGameModuleClient__QEAAXAEAVResourcePackRepository__AEAVResourcePackStack__AEBVBaseGameVersion__W4ResourceLoadingPhase_GameModuleClient___Z_ptr;
}

static std::unordered_map<std::string, size_t> pointerList[5];

void InitBedrockPointers() {
	pointerList[0]["1.14.60.5"] = 0x0E555A0;
	pointerList[0]["1.16.0.2"] = 0x0EEC9C0;
	pointerList[1]["1.14.60.5"] = 0x0E2CCB0;
	pointerList[1]["1.16.0.2"] = 0x0EBD350;
	pointerList[2]["1.14.60.5"] = 0x1AC2FC0;
	pointerList[2]["1.16.0.2"] = 0x1CF1680;
	pointerList[3]["1.14.60.5"] = 0x1AC6500;
	pointerList[3]["1.16.0.2"] = 0x1CF4450;
	pointerList[4]["1.14.60.5"] = 0x1ABF450;
	pointerList[4]["1.16.0.2"] = 0x1CE6600;
	_initializeResourceStack_VanillaGameModuleClient__QEAAXAEAVResourcePackRepository__AEAVResourcePackStack__AEBVBaseGameVersion___Z_ptr = reinterpret_cast<void*>(Zenova::Hook::SlideAddress(0x1ABF550));
	_initializeResourceStack2_VanillaGameModuleClient__QEAAXAEAVResourcePackRepository__AEAVResourcePackStack__AEBVBaseGameVersion__W4ResourceLoadingPhase_GameModuleClient___Z_ptr = reinterpret_cast<void*>(Zenova::Hook::SlideAddress(0x1CE5910));
}

void InitVersionPointers(std::string versionId) {
	_fromString_UUID_mce__SA_AV12_AEBV_$basic_string_DU_$char_traits_D_std__V_$allocator_D_2__std___Z_ptr = reinterpret_cast<void*>(Zenova::Hook::SlideAddress(pointerList[0][versionId]));
	__parseVersionToString_SemVersion__AEAAXXZ_ptr = reinterpret_cast<void*>(Zenova::Hook::SlideAddress(pointerList[1][versionId]));
	_getPacks_VanillaInPackagePacks__QEBA_AV_$vector_UMetaData_IInPackagePacks__V_$allocator_UMetaData_IInPackagePacks___std___std__W4PackType___Z_ptr = reinterpret_cast<void*>(Zenova::Hook::SlideAddress(pointerList[2][versionId]));
	_initializeBehaviorStack_VanillaGameModuleServer__QEAAXAEBVGameRules__AEAVResourcePackRepository__AEAVResourcePackStack__AEBVBaseGameVersion___Z_ptr = reinterpret_cast<void*>(Zenova::Hook::SlideAddress(pointerList[3][versionId]));
	_addPackFromPackId__YAXPEAULambdaPack1__AEBUPackIdVersion___Z_ptr = reinterpret_cast<void*>(Zenova::Hook::SlideAddress(pointerList[4][versionId]));
}