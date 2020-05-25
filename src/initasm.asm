; This file was automatically generated using tools/process_csv.py
; Generated on Sun May 17 2020 06:49:43 UTC
bits 64
SECTION .data
extern _fromString_UUID_mce__SA_AV12_AEBV_$basic_string_DU_$char_traits_D_std__V_$allocator_D_2__std___Z_ptr
extern __parseVersionToString_SemVersion__AEAAXXZ_ptr

SECTION .text
global ?fromString@UUID@mce@@SA?AV12@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z
?fromString@UUID@mce@@SA?AV12@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z:
	mov rax, [rel _fromString_UUID_mce__SA_AV12_AEBV_$basic_string_DU_$char_traits_D_std__V_$allocator_D_2__std___Z_ptr]
	jmp rax
global ?_parseVersionToString@SemVersion@@AEAAXXZ
?_parseVersionToString@SemVersion@@AEAAXXZ:
	mov rax, [rel __parseVersionToString_SemVersion__AEAAXXZ_ptr]
	jmp rax
