; This file was automatically generated using tools/process_csv.py
; Generated on Thu Jun 18 2020 23:17:49 UTC
bits 64
SECTION .data
extern _fromString_UUID_mce__SA_AV12_AEBV_$basic_string_DU_$char_traits_D_std__V_$allocator_D_2__std___Z_ptr
extern __parseVersionToString_SemVersion__AEAAXXZ_ptr
extern _getPacks_VanillaInPackagePacks__QEBA_AV_$vector_UMetaData_IInPackagePacks__V_$allocator_UMetaData_IInPackagePacks___std___std__W4PackType___Z_ptr
extern _initializeResourceStack_VanillaGameModuleClient__QEAAXAEAVResourcePackRepository__AEAVResourcePackStack__AEBVBaseGameVersion___Z_ptr
extern _initializeBehaviorStack_VanillaGameModuleServer__QEAAXAEBVGameRules__AEAVResourcePackRepository__AEAVResourcePackStack__AEBVBaseGameVersion___Z_ptr

SECTION .text
global ?fromString@UUID@mce@@SA?AV12@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z
?fromString@UUID@mce@@SA?AV12@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z:
	mov rax, [rel _fromString_UUID_mce__SA_AV12_AEBV_$basic_string_DU_$char_traits_D_std__V_$allocator_D_2__std___Z_ptr]
	jmp rax
global ?_parseVersionToString@SemVersion@@AEAAXXZ
?_parseVersionToString@SemVersion@@AEAAXXZ:
	mov rax, [rel __parseVersionToString_SemVersion__AEAAXXZ_ptr]
	jmp rax
global ?getPacks@VanillaInPackagePacks@@QEBA?AV?$vector@UMetaData@IInPackagePacks@@V?$allocator@UMetaData@IInPackagePacks@@@std@@@std@@W4PackType@@@Z
?getPacks@VanillaInPackagePacks@@QEBA?AV?$vector@UMetaData@IInPackagePacks@@V?$allocator@UMetaData@IInPackagePacks@@@std@@@std@@W4PackType@@@Z:
	mov rax, [rel _getPacks_VanillaInPackagePacks__QEBA_AV_$vector_UMetaData_IInPackagePacks__V_$allocator_UMetaData_IInPackagePacks___std___std__W4PackType___Z_ptr]
	jmp rax
global ?initializeResourceStack@VanillaGameModuleClient@@QEAAXAEAVResourcePackRepository@@AEAVResourcePackStack@@AEBVBaseGameVersion@@@Z
?initializeResourceStack@VanillaGameModuleClient@@QEAAXAEAVResourcePackRepository@@AEAVResourcePackStack@@AEBVBaseGameVersion@@@Z:
	mov rax, [rel _initializeResourceStack_VanillaGameModuleClient__QEAAXAEAVResourcePackRepository__AEAVResourcePackStack__AEBVBaseGameVersion___Z_ptr]
	jmp rax
global ?initializeBehaviorStack@VanillaGameModuleServer@@QEAAXAEBVGameRules@@AEAVResourcePackRepository@@AEAVResourcePackStack@@AEBVBaseGameVersion@@@Z
?initializeBehaviorStack@VanillaGameModuleServer@@QEAAXAEBVGameRules@@AEAVResourcePackRepository@@AEAVResourcePackStack@@AEBVBaseGameVersion@@@Z:
	mov rax, [rel _initializeBehaviorStack_VanillaGameModuleServer__QEAAXAEBVGameRules__AEAVResourcePackRepository__AEAVResourcePackStack__AEBVBaseGameVersion___Z_ptr]
	jmp rax
