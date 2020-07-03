// This file was automatically generated using tools/process_csv.py
// Generated on Thu Jul 02 2020 23:45:53 UTC

#include "initcpp.h"


extern "C" {
	void* _getPacks_VanillaInPackagePacks__QEBA_AV_$vector_UMetaData_IInPackagePacks__V_$allocator_UMetaData_IInPackagePacks___std___std__W4PackType___Z_ptr;
	void* _initializeBehaviorStack_VanillaGameModuleServer__QEAAXAEBVGameRules__AEAVResourcePackRepository__AEAVResourcePackStack__AEBVBaseGameVersion___Z_ptr;
	void* _addPackFromPackId__YAXPEAULambdaPack1__AEBUPackIdVersion___Z_ptr;
	void* _initializeResourceStack_VanillaGameModuleClient__QEAAXAEAVResourcePackRepository__AEAVResourcePackStack__AEBVBaseGameVersion___Z_ptr;
	void* _initializeResourceStack2_VanillaGameModuleClient__QEAAXAEAVResourcePackRepository__AEAVResourcePackStack__AEBVBaseGameVersion__W4ResourceLoadingPhase_GameModuleClient___Z_ptr;
	void* _registerButtonDownHandler_InputHandler__QEAAXV_$basic_string_DU_$char_traits_D_std__V_$allocator_D_2__std__V_$function_$$A6AXW4FocusImpact__AEAVIClientInstance___Z_3__N_Z_ptr;
	void* _registerButtonUpHandler_InputHandler__QEAAXV_$basic_string_DU_$char_traits_D_std__V_$allocator_D_2__std__V_$function_$$A6AXW4FocusImpact__AEAVIClientInstance___Z_3__N_Z_ptr;
	void* __registerInputHandlers_MinecraftInputHandler__QEAAXXZ_ptr;
	void* _assignDefaultMapping_RemappingLayout__QEAAX$$QEAV_$vector_VKeymapping__V_$allocator_VKeymapping___std___std___Z_ptr;
	void* __populateKeyboardDefaults_VanillaClientInputMappingFactory__QEAAXAEAVRemappingLayout___Z_ptr;
	void* __populateGamepadDefaults_VanillaClientInputMappingFactory__QEAAXAEAVRemappingLayout___Z_ptr;
	void* __addFullKeyboardGamePlayControls_VanillaClientInputMappingFactory__QEAAXAEAVKeyboardInputMapping__AEAVMouseInputMapping___Z_ptr;
	void* __addInvariantGamePlayGameControllerControls_VanillaClientInputMappingFactory__QEAAXAEAVGameControllerInputMapping__W4MappingControllerType_ClientInputMappingFactory___Z_ptr;
	void* __handleDuplicates_ControlsSettingsScreenController__QEAAXAEAVRemappingLayout___Z_ptr;
}

static std::unordered_map<std::string, size_t> pointerList[12];

void InitBedrockPointers() {
	pointerList[0]["1.14.60.5"] = 0x1AC2FC0;
	pointerList[0]["1.16.0.2"] = 0x1CF1680;
	pointerList[1]["1.14.60.5"] = 0x1AC6500;
	pointerList[1]["1.16.0.2"] = 0x1CF4450;
	pointerList[2]["1.14.60.5"] = 0x1ABF450;
	pointerList[2]["1.16.0.2"] = 0x1CE6600;
	_initializeResourceStack_VanillaGameModuleClient__QEAAXAEAVResourcePackRepository__AEAVResourcePackStack__AEBVBaseGameVersion___Z_ptr = reinterpret_cast<void*>(Zenova::Hook::SlideAddress(0x1ABF550));
	_initializeResourceStack2_VanillaGameModuleClient__QEAAXAEAVResourcePackRepository__AEAVResourcePackStack__AEBVBaseGameVersion__W4ResourceLoadingPhase_GameModuleClient___Z_ptr = reinterpret_cast<void*>(Zenova::Hook::SlideAddress(0x1CE5910));
	pointerList[3]["1.14.60.5"] = 0x19597B0;
	pointerList[3]["1.16.0.2"] = 0x1AAF790;
	pointerList[4]["1.14.60.5"] = 0x1959900;
	pointerList[4]["1.16.0.2"] = 0x1AAF8E0;
	pointerList[5]["1.14.60.5"] = 0x079D610;
	pointerList[5]["1.16.0.2"] = 0x07FEBB0;
	pointerList[6]["1.14.60.5"] = 0x07A4E20;
	pointerList[6]["1.16.0.2"] = 0x0806630;
	pointerList[7]["1.14.60.5"] = 0x1AA0FC0;
	pointerList[7]["1.16.0.2"] = 0x1CC0A40;
	pointerList[8]["1.14.60.5"] = 0x1AA3860;
	pointerList[8]["1.16.0.2"] = 0x1CC2B50;
	pointerList[9]["1.14.60.5"] = 0x1AAEB80;
	pointerList[9]["1.16.0.2"] = 0x1CCCEB0;
	pointerList[10]["1.14.60.5"] = 0x1AB07D0;
	pointerList[10]["1.16.0.2"] = 0x1CCEB90;
	pointerList[11]["1.14.60.5"] = 0x0305470;
	pointerList[11]["1.16.0.2"] = 0x0338980;
}

void InitVersionPointers(std::string versionId) {
	_getPacks_VanillaInPackagePacks__QEBA_AV_$vector_UMetaData_IInPackagePacks__V_$allocator_UMetaData_IInPackagePacks___std___std__W4PackType___Z_ptr = reinterpret_cast<void*>(Zenova::Hook::SlideAddress(pointerList[0][versionId]));
	_initializeBehaviorStack_VanillaGameModuleServer__QEAAXAEBVGameRules__AEAVResourcePackRepository__AEAVResourcePackStack__AEBVBaseGameVersion___Z_ptr = reinterpret_cast<void*>(Zenova::Hook::SlideAddress(pointerList[1][versionId]));
	_addPackFromPackId__YAXPEAULambdaPack1__AEBUPackIdVersion___Z_ptr = reinterpret_cast<void*>(Zenova::Hook::SlideAddress(pointerList[2][versionId]));
	_registerButtonDownHandler_InputHandler__QEAAXV_$basic_string_DU_$char_traits_D_std__V_$allocator_D_2__std__V_$function_$$A6AXW4FocusImpact__AEAVIClientInstance___Z_3__N_Z_ptr = reinterpret_cast<void*>(Zenova::Hook::SlideAddress(pointerList[3][versionId]));
	_registerButtonUpHandler_InputHandler__QEAAXV_$basic_string_DU_$char_traits_D_std__V_$allocator_D_2__std__V_$function_$$A6AXW4FocusImpact__AEAVIClientInstance___Z_3__N_Z_ptr = reinterpret_cast<void*>(Zenova::Hook::SlideAddress(pointerList[4][versionId]));
	__registerInputHandlers_MinecraftInputHandler__QEAAXXZ_ptr = reinterpret_cast<void*>(Zenova::Hook::SlideAddress(pointerList[5][versionId]));
	_assignDefaultMapping_RemappingLayout__QEAAX$$QEAV_$vector_VKeymapping__V_$allocator_VKeymapping___std___std___Z_ptr = reinterpret_cast<void*>(Zenova::Hook::SlideAddress(pointerList[6][versionId]));
	__populateKeyboardDefaults_VanillaClientInputMappingFactory__QEAAXAEAVRemappingLayout___Z_ptr = reinterpret_cast<void*>(Zenova::Hook::SlideAddress(pointerList[7][versionId]));
	__populateGamepadDefaults_VanillaClientInputMappingFactory__QEAAXAEAVRemappingLayout___Z_ptr = reinterpret_cast<void*>(Zenova::Hook::SlideAddress(pointerList[8][versionId]));
	__addFullKeyboardGamePlayControls_VanillaClientInputMappingFactory__QEAAXAEAVKeyboardInputMapping__AEAVMouseInputMapping___Z_ptr = reinterpret_cast<void*>(Zenova::Hook::SlideAddress(pointerList[9][versionId]));
	__addInvariantGamePlayGameControllerControls_VanillaClientInputMappingFactory__QEAAXAEAVGameControllerInputMapping__W4MappingControllerType_ClientInputMappingFactory___Z_ptr = reinterpret_cast<void*>(Zenova::Hook::SlideAddress(pointerList[10][versionId]));
	__handleDuplicates_ControlsSettingsScreenController__QEAAXAEAVRemappingLayout___Z_ptr = reinterpret_cast<void*>(Zenova::Hook::SlideAddress(pointerList[11][versionId]));
}