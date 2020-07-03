// This file was automatically generated using tools/process_csv.py
// Generated on Fri Jul 03 2020 04:39:16 UTC

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

static std::unordered_map<std::string, size_t> pointerList[3];

void InitBedrockPointers() {
	pointerList[0]["1.14.60.5"] = 0x1AC2FC0;
	pointerList[0]["1.16.0.2"] = 0x1CF1680;
	pointerList[1]["1.14.60.5"] = 0x1AC6500;
	pointerList[1]["1.16.0.2"] = 0x1CF4450;
	pointerList[2]["1.14.60.5"] = 0x1ABF450;
	pointerList[2]["1.16.0.2"] = 0x1CE6600;
	_initializeResourceStack_VanillaGameModuleClient__QEAAXAEAVResourcePackRepository__AEAVResourcePackStack__AEBVBaseGameVersion___Z_ptr = reinterpret_cast<void*>(Zenova::Hook::SlideAddress(0x1ABF550));
	_initializeResourceStack2_VanillaGameModuleClient__QEAAXAEAVResourcePackRepository__AEAVResourcePackStack__AEBVBaseGameVersion__W4ResourceLoadingPhase_GameModuleClient___Z_ptr = reinterpret_cast<void*>(Zenova::Hook::SlideAddress(0x1CE5910));

	_registerButtonDownHandler_InputHandler__QEAAXV_$basic_string_DU_$char_traits_D_std__V_$allocator_D_2__std__V_$function_$$A6AXW4FocusImpact__AEAVIClientInstance___Z_3__N_Z_ptr = reinterpret_cast<void*>(Zenova::Hook::SigscanCall("\xE8\x00\x00\x00\x00\x49\x8B\x5E\x08\x48\x8D\x45\xCF", "x????xxxxxxxx"));
	_registerButtonUpHandler_InputHandler__QEAAXV_$basic_string_DU_$char_traits_D_std__V_$allocator_D_2__std__V_$function_$$A6AXW4FocusImpact__AEAVIClientInstance___Z_3__N_Z_ptr = reinterpret_cast<void*>(Zenova::Hook::SigscanCall("\xE8\x00\x00\x00\x00\x4C\x8D\x9C\x24\x00\x00\x00\x00\x49\x8B\x5B\x30\x49\x8B\x73\x38\x49\x8B\xE3\x41\x5E\x5F\x5D\xC3\xCC\x40\x55\x56\x57\x41\x56", "x????xxxx????xxxxxxxxxxxxxxxxxxxxxxx"));
	__registerInputHandlers_MinecraftInputHandler__QEAAXXZ_ptr = reinterpret_cast<void*>(Zenova::Hook::Sigscan("\x48\x8B\xC4\x55\x57\x41\x56\x48\x8D\xA8\x00\x00\x00\x00\x48\x81\xEC\x00\x00\x00\x00\x48\xC7\x45\x00\x00\x00\x00\x00\x48\x89\x58\x18\x48\x89\x70\x20\x4C\x8B\xF1", "xxxxxxxxxx????xxx????xxx?????xxxxxxxxxxx"));
	_assignDefaultMapping_RemappingLayout__QEAAX$$QEAV_$vector_VKeymapping__V_$allocator_VKeymapping___std___std___Z_ptr = reinterpret_cast<void*>(Zenova::Hook::Sigscan("\x40\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x8B\xEC\x48\x83\xEC\x60\x48\xC7\x45\x00\x00\x00\x00\x00\x48\x89\x9C\x24\x00\x00\x00\x00\x48\x8B\x05\x00\x00\x00\x00\x48\x33\xC4\x48\x89\x45\xF8\x4C\x8B\xF9\x48\x89\x4D\xC8\x48\x8D\x71\x20\x48\x3B\xF2\x74\x0F\x4C\x8B\x42\x08\x48\x8B\x12\x48\x8B\xCE", "xxxxxxxxxxxxxxxxxxxxxx?????xxxx????xxx????xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"));
	__populateKeyboardDefaults_VanillaClientInputMappingFactory__QEAAXAEAVRemappingLayout___Z_ptr = reinterpret_cast<void*>(Zenova::Hook::SigscanCall("\xE8\x00\x00\x00\x00\x90\xBE\x00\x00\x00\x00\x48\x85\xDB\x74\x2A\x8B\xC6\xF0\x0F\xC1\x43\x00\x83\xF8\x01\x75\x1E\x48\x8B\x03\x48\x8B\xCB\xFF\x10\x8B\xC6\xF0\x0F\xC1\x43\x00\x83\xF8\x01\x75\x0A\x48\x8B\x03\x48\x8B\xCB\xFF\x50\x08\x90\x49\x8B\x96\x00\x00\x00\x00\x48\x8B\x5A\x18", "x????xx????xxxxxxxxxxx?xxxxxxxxxxxxxxxxxxx?xxxxxxxxxxxxxxxxxx????xxxx"));
	__populateGamepadDefaults_VanillaClientInputMappingFactory__QEAAXAEAVRemappingLayout___Z_ptr = reinterpret_cast<void*>(Zenova::Hook::SigscanCall("\xE8\x00\x00\x00\x00\x49\x8B\x96\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x49\x8B\x96\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x49\x8B\x96\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x49\x8B\x96\x00\x00\x00\x00", "x????xxx????x????xxx????x????xxx????x????xxx????"));
	__addFullKeyboardGamePlayControls_VanillaClientInputMappingFactory__QEAAXAEAVKeyboardInputMapping__AEAVMouseInputMapping___Z_ptr = reinterpret_cast<void*>(Zenova::Hook::Sigscan("\x40\x55\x56\x57\x48\x8B\xEC\x48\x83\xEC\x60\x48\xC7\x45\x00\x00\x00\x00\x00\x48\x89\x9C\x24\x00\x00\x00\x00\x49\x8B\xF0\x48\x8B\xFA\x48\x8B\xD9\x66\x0F\x6F\x05\x00\x00\x00\x00", "xxxxxxxxxxxxxx?????xxxx????xxxxxxxxxxxxx????"));
	__addInvariantGamePlayGameControllerControls_VanillaClientInputMappingFactory__QEAAXAEAVGameControllerInputMapping__W4MappingControllerType_ClientInputMappingFactory___Z_ptr = reinterpret_cast<void*>(Zenova::Hook::SigscanCall("\xE8\x00\x00\x00\x00\x48\xC7\x45\x00\x00\x00\x00\x00\xC6\x45\xE7\x00\x48\xC7\x45\x00\x00\x00\x00\x00\x41\xB8\x00\x00\x00\x00\x48\x8D\x15\x00\x00\x00\x00\x48\x8D\x4D\xE7\xE8\x00\x00\x00\x00\xC6\x45\xF6\x00\xF3\x0F\x10\x35\x00\x00\x00\x00", "x????xxx?????xxxxxxx?????xx????xxx????xxxxx????xxxxxxxx????"));
	__handleDuplicates_ControlsSettingsScreenController__QEAAXAEAVRemappingLayout___Z_ptr = reinterpret_cast<void*>(Zenova::Hook::SigscanCall("\xE8\x00\x00\x00\x00\x4C\x8B\x43\x08\x48\x8D\x53\x10\x48\x8B\xCF\xE8\x00\x00\x00\x00\x48\x8B\x5C\x24\x00\xB8\x00\x00\x00\x00", "x????xxxxxxxxxxxx????xxxx?x????"));

	Zenova::Platform::DebugPause();
}

void InitVersionPointers(std::string versionId) {
	_getPacks_VanillaInPackagePacks__QEBA_AV_$vector_UMetaData_IInPackagePacks__V_$allocator_UMetaData_IInPackagePacks___std___std__W4PackType___Z_ptr = reinterpret_cast<void*>(Zenova::Hook::SlideAddress(pointerList[0][versionId]));
	_initializeBehaviorStack_VanillaGameModuleServer__QEAAXAEBVGameRules__AEAVResourcePackRepository__AEAVResourcePackStack__AEBVBaseGameVersion___Z_ptr = reinterpret_cast<void*>(Zenova::Hook::SlideAddress(pointerList[1][versionId]));
	_addPackFromPackId__YAXPEAULambdaPack1__AEBUPackIdVersion___Z_ptr = reinterpret_cast<void*>(Zenova::Hook::SlideAddress(pointerList[2][versionId]));
}