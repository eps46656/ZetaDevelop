	.text
	.def	@feat.00;
	.scl	3;
	.type	0;
	.endef
	.globl	@feat.00
.set @feat.00, 0
	.intel_syntax noprefix
	.file	"test_cpuid.c"
	.file	1 "D:/ZetaDevelop/ZetaExe" "test_cpuid.c"
	.def	main;
	.scl	2;
	.type	32;
	.endef
	.globl	main                            # -- Begin function main
	.p2align	4, 0x90
main:                                   # @main
.Lfunc_begin0:
	.loc	1 24 0                          # test_cpuid.c:24:0
.seh_proc main
# %bb.0:
	push	rbp
	.seh_pushreg rbp
	push	rsi
	.seh_pushreg rsi
	push	rdi
	.seh_pushreg rdi
	sub	rsp, 256
	.seh_stackalloc 256
	lea	rbp, [rsp + 128]
	.seh_setframe rbp, 128
	movaps	xmmword ptr [rbp + 112], xmm15  # 16-byte Spill
	.seh_savexmm xmm15, 240
	movaps	xmmword ptr [rbp + 96], xmm14   # 16-byte Spill
	.seh_savexmm xmm14, 224
	movaps	xmmword ptr [rbp + 80], xmm13   # 16-byte Spill
	.seh_savexmm xmm13, 208
	movaps	xmmword ptr [rbp + 64], xmm12   # 16-byte Spill
	.seh_savexmm xmm12, 192
	movaps	xmmword ptr [rbp + 48], xmm11   # 16-byte Spill
	.seh_savexmm xmm11, 176
	movaps	xmmword ptr [rbp + 32], xmm10   # 16-byte Spill
	.seh_savexmm xmm10, 160
	movaps	xmmword ptr [rbp + 16], xmm9    # 16-byte Spill
	.seh_savexmm xmm9, 144
	movaps	xmmword ptr [rbp], xmm8         # 16-byte Spill
	.seh_savexmm xmm8, 128
	movaps	xmmword ptr [rbp - 16], xmm7    # 16-byte Spill
	.seh_savexmm xmm7, 112
	movaps	xmmword ptr [rbp - 32], xmm6    # 16-byte Spill
	.seh_savexmm xmm6, 96
	.seh_endprologue
	call	__main
	mov	dword ptr [rbp - 36], 0
.Ltmp0:
	.loc	1 27 5 prologue_end             # test_cpuid.c:27:5
	lea	rdi, [rbp - 80]
	call	GetCpuInfo
	.loc	1 29 5                          # test_cpuid.c:29:5
	lea	rcx, [rip + .L.str]
	lea	rdx, [rip + .L.str.1]
	mov	r8d, 29
	call	printf
	.loc	1 31 5                          # test_cpuid.c:31:5
	lea	rcx, [rbp - 80]
	add	rcx, 4
	mov	rax, rsp
	mov	qword ptr [rax + 32], rcx
	lea	rcx, [rip + .L.str.2]
	lea	rdx, [rip + .L.str.1]
	lea	r9, [rip + .L.str.3]
	mov	r8d, 31
	call	printf
	.loc	1 33 5                          # test_cpuid.c:33:5
	xor	eax, eax
	movaps	xmm6, xmmword ptr [rbp - 32]    # 16-byte Reload
	movaps	xmm7, xmmword ptr [rbp - 16]    # 16-byte Reload
	movaps	xmm8, xmmword ptr [rbp]         # 16-byte Reload
	movaps	xmm9, xmmword ptr [rbp + 16]    # 16-byte Reload
	movaps	xmm10, xmmword ptr [rbp + 32]   # 16-byte Reload
	movaps	xmm11, xmmword ptr [rbp + 48]   # 16-byte Reload
	movaps	xmm12, xmmword ptr [rbp + 64]   # 16-byte Reload
	movaps	xmm13, xmmword ptr [rbp + 80]   # 16-byte Reload
	movaps	xmm14, xmmword ptr [rbp + 96]   # 16-byte Reload
	movaps	xmm15, xmmword ptr [rbp + 112]  # 16-byte Reload
	.loc	1 33 5 epilogue_begin is_stmt 0 # test_cpuid.c:33:5
	add	rsp, 256
	pop	rdi
	pop	rsi
	pop	rbp
	ret
.Ltmp1:
.Lfunc_end0:
	.seh_endproc
                                        # -- End function
	.def	printf;
	.scl	3;
	.type	32;
	.endef
	.p2align	4, 0x90                         # -- Begin function printf
printf:                                 # @printf
.Lfunc_begin1:
	.file	2 "C:/Program Files/llvm-mingw-20231003-msvcrt-x86_64/include" "stdio.h"
	.loc	2 380 0 is_stmt 1               # C:/Program Files/llvm-mingw-20231003-msvcrt-x86_64/include/stdio.h:380:0
.seh_proc printf
# %bb.0:
	sub	rsp, 56
	.seh_stackalloc 56
	.seh_endprologue
	mov	qword ptr [rsp + 88], r9
	mov	qword ptr [rsp + 80], r8
	mov	qword ptr [rsp + 72], rdx
	mov	qword ptr [rsp + 48], rcx
	lea	rax, [rsp + 72]
.Ltmp2:
	.loc	2 382 35 prologue_end           # C:/Program Files/llvm-mingw-20231003-msvcrt-x86_64/include/stdio.h:382:35
	mov	qword ptr [rsp + 32], rax
	.loc	2 383 32                        # C:/Program Files/llvm-mingw-20231003-msvcrt-x86_64/include/stdio.h:383:32
	mov	rax, qword ptr [rip + __imp___acrt_iob_func]
	mov	ecx, 1
	call	rax
	mov	rcx, rax
	.loc	2 383 40 is_stmt 0              # C:/Program Files/llvm-mingw-20231003-msvcrt-x86_64/include/stdio.h:383:40
	mov	rdx, qword ptr [rsp + 48]
	.loc	2 383 50                        # C:/Program Files/llvm-mingw-20231003-msvcrt-x86_64/include/stdio.h:383:50
	mov	r8, qword ptr [rsp + 32]
	.loc	2 383 14                        # C:/Program Files/llvm-mingw-20231003-msvcrt-x86_64/include/stdio.h:383:14
	call	__mingw_vfprintf
	.loc	2 383 12                        # C:/Program Files/llvm-mingw-20231003-msvcrt-x86_64/include/stdio.h:383:12
	mov	dword ptr [rsp + 44], eax
	.loc	2 385 10 is_stmt 1              # C:/Program Files/llvm-mingw-20231003-msvcrt-x86_64/include/stdio.h:385:10
	mov	eax, dword ptr [rsp + 44]
	.loc	2 385 3 is_stmt 0               # C:/Program Files/llvm-mingw-20231003-msvcrt-x86_64/include/stdio.h:385:3
	.loc	2 385 3 epilogue_begin          # C:/Program Files/llvm-mingw-20231003-msvcrt-x86_64/include/stdio.h:385:3
	add	rsp, 56
	ret
.Ltmp3:
.Lfunc_end1:
	.seh_endproc
                                        # -- End function
	.section	.rdata,"dr"
.L.str:                                 # @.str
	.asciz	"print pos at %s:%d\n"

.L.str.1:                               # @.str.1
	.asciz	"D:/ZetaDevelop/ZetaExe/test_cpuid.c"

.L.str.2:                               # @.str.2
	.asciz	"print var at %s:%d\n    %s = %s\n"

.L.str.3:                               # @.str.3
	.asciz	"cpu_info.vender_id"

	.section	.debug_abbrev,"dr"
.Lsection_abbrev:
	.byte	1                               # Abbreviation Code
	.byte	17                              # DW_TAG_compile_unit
	.byte	1                               # DW_CHILDREN_yes
	.byte	37                              # DW_AT_producer
	.byte	14                              # DW_FORM_strp
	.byte	19                              # DW_AT_language
	.byte	5                               # DW_FORM_data2
	.byte	3                               # DW_AT_name
	.byte	14                              # DW_FORM_strp
	.byte	16                              # DW_AT_stmt_list
	.byte	23                              # DW_FORM_sec_offset
	.byte	27                              # DW_AT_comp_dir
	.byte	14                              # DW_FORM_strp
	.byte	17                              # DW_AT_low_pc
	.byte	1                               # DW_FORM_addr
	.byte	18                              # DW_AT_high_pc
	.byte	6                               # DW_FORM_data4
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	2                               # Abbreviation Code
	.byte	52                              # DW_TAG_variable
	.byte	0                               # DW_CHILDREN_no
	.byte	73                              # DW_AT_type
	.byte	19                              # DW_FORM_ref4
	.byte	58                              # DW_AT_decl_file
	.byte	11                              # DW_FORM_data1
	.byte	59                              # DW_AT_decl_line
	.byte	11                              # DW_FORM_data1
	.byte	2                               # DW_AT_location
	.byte	24                              # DW_FORM_exprloc
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	3                               # Abbreviation Code
	.byte	1                               # DW_TAG_array_type
	.byte	1                               # DW_CHILDREN_yes
	.byte	73                              # DW_AT_type
	.byte	19                              # DW_FORM_ref4
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	4                               # Abbreviation Code
	.byte	33                              # DW_TAG_subrange_type
	.byte	0                               # DW_CHILDREN_no
	.byte	73                              # DW_AT_type
	.byte	19                              # DW_FORM_ref4
	.byte	55                              # DW_AT_count
	.byte	11                              # DW_FORM_data1
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	5                               # Abbreviation Code
	.byte	36                              # DW_TAG_base_type
	.byte	0                               # DW_CHILDREN_no
	.byte	3                               # DW_AT_name
	.byte	14                              # DW_FORM_strp
	.byte	62                              # DW_AT_encoding
	.byte	11                              # DW_FORM_data1
	.byte	11                              # DW_AT_byte_size
	.byte	11                              # DW_FORM_data1
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	6                               # Abbreviation Code
	.byte	36                              # DW_TAG_base_type
	.byte	0                               # DW_CHILDREN_no
	.byte	3                               # DW_AT_name
	.byte	14                              # DW_FORM_strp
	.byte	11                              # DW_AT_byte_size
	.byte	11                              # DW_FORM_data1
	.byte	62                              # DW_AT_encoding
	.byte	11                              # DW_FORM_data1
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	7                               # Abbreviation Code
	.byte	46                              # DW_TAG_subprogram
	.byte	1                               # DW_CHILDREN_yes
	.byte	17                              # DW_AT_low_pc
	.byte	1                               # DW_FORM_addr
	.byte	18                              # DW_AT_high_pc
	.byte	6                               # DW_FORM_data4
	.byte	64                              # DW_AT_frame_base
	.byte	24                              # DW_FORM_exprloc
	.byte	3                               # DW_AT_name
	.byte	14                              # DW_FORM_strp
	.byte	58                              # DW_AT_decl_file
	.byte	11                              # DW_FORM_data1
	.byte	59                              # DW_AT_decl_line
	.byte	11                              # DW_FORM_data1
	.byte	73                              # DW_AT_type
	.byte	19                              # DW_FORM_ref4
	.byte	63                              # DW_AT_external
	.byte	25                              # DW_FORM_flag_present
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	8                               # Abbreviation Code
	.byte	52                              # DW_TAG_variable
	.byte	0                               # DW_CHILDREN_no
	.byte	2                               # DW_AT_location
	.byte	24                              # DW_FORM_exprloc
	.byte	3                               # DW_AT_name
	.byte	14                              # DW_FORM_strp
	.byte	58                              # DW_AT_decl_file
	.byte	11                              # DW_FORM_data1
	.byte	59                              # DW_AT_decl_line
	.byte	11                              # DW_FORM_data1
	.byte	73                              # DW_AT_type
	.byte	19                              # DW_FORM_ref4
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	9                               # Abbreviation Code
	.byte	46                              # DW_TAG_subprogram
	.byte	1                               # DW_CHILDREN_yes
	.byte	17                              # DW_AT_low_pc
	.byte	1                               # DW_FORM_addr
	.byte	18                              # DW_AT_high_pc
	.byte	6                               # DW_FORM_data4
	.byte	64                              # DW_AT_frame_base
	.byte	24                              # DW_FORM_exprloc
	.byte	3                               # DW_AT_name
	.byte	14                              # DW_FORM_strp
	.byte	58                              # DW_AT_decl_file
	.byte	11                              # DW_FORM_data1
	.byte	59                              # DW_AT_decl_line
	.byte	5                               # DW_FORM_data2
	.byte	39                              # DW_AT_prototyped
	.byte	25                              # DW_FORM_flag_present
	.byte	73                              # DW_AT_type
	.byte	19                              # DW_FORM_ref4
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	10                              # Abbreviation Code
	.byte	5                               # DW_TAG_formal_parameter
	.byte	0                               # DW_CHILDREN_no
	.byte	2                               # DW_AT_location
	.byte	24                              # DW_FORM_exprloc
	.byte	3                               # DW_AT_name
	.byte	14                              # DW_FORM_strp
	.byte	58                              # DW_AT_decl_file
	.byte	11                              # DW_FORM_data1
	.byte	59                              # DW_AT_decl_line
	.byte	5                               # DW_FORM_data2
	.byte	73                              # DW_AT_type
	.byte	19                              # DW_FORM_ref4
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	11                              # Abbreviation Code
	.byte	52                              # DW_TAG_variable
	.byte	0                               # DW_CHILDREN_no
	.byte	2                               # DW_AT_location
	.byte	24                              # DW_FORM_exprloc
	.byte	3                               # DW_AT_name
	.byte	14                              # DW_FORM_strp
	.byte	58                              # DW_AT_decl_file
	.byte	11                              # DW_FORM_data1
	.byte	59                              # DW_AT_decl_line
	.byte	5                               # DW_FORM_data2
	.byte	73                              # DW_AT_type
	.byte	19                              # DW_FORM_ref4
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	12                              # Abbreviation Code
	.byte	24                              # DW_TAG_unspecified_parameters
	.byte	0                               # DW_CHILDREN_no
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	13                              # Abbreviation Code
	.byte	22                              # DW_TAG_typedef
	.byte	0                               # DW_CHILDREN_no
	.byte	73                              # DW_AT_type
	.byte	19                              # DW_FORM_ref4
	.byte	3                               # DW_AT_name
	.byte	14                              # DW_FORM_strp
	.byte	58                              # DW_AT_decl_file
	.byte	11                              # DW_FORM_data1
	.byte	59                              # DW_AT_decl_line
	.byte	11                              # DW_FORM_data1
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	14                              # Abbreviation Code
	.byte	19                              # DW_TAG_structure_type
	.byte	1                               # DW_CHILDREN_yes
	.byte	3                               # DW_AT_name
	.byte	14                              # DW_FORM_strp
	.byte	11                              # DW_AT_byte_size
	.byte	11                              # DW_FORM_data1
	.byte	58                              # DW_AT_decl_file
	.byte	11                              # DW_FORM_data1
	.byte	59                              # DW_AT_decl_line
	.byte	11                              # DW_FORM_data1
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	15                              # Abbreviation Code
	.byte	13                              # DW_TAG_member
	.byte	0                               # DW_CHILDREN_no
	.byte	3                               # DW_AT_name
	.byte	14                              # DW_FORM_strp
	.byte	73                              # DW_AT_type
	.byte	19                              # DW_FORM_ref4
	.byte	58                              # DW_AT_decl_file
	.byte	11                              # DW_FORM_data1
	.byte	59                              # DW_AT_decl_line
	.byte	11                              # DW_FORM_data1
	.byte	56                              # DW_AT_data_member_location
	.byte	11                              # DW_FORM_data1
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	16                              # Abbreviation Code
	.byte	15                              # DW_TAG_pointer_type
	.byte	0                               # DW_CHILDREN_no
	.byte	73                              # DW_AT_type
	.byte	19                              # DW_FORM_ref4
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	17                              # Abbreviation Code
	.byte	38                              # DW_TAG_const_type
	.byte	0                               # DW_CHILDREN_no
	.byte	73                              # DW_AT_type
	.byte	19                              # DW_FORM_ref4
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	18                              # Abbreviation Code
	.byte	22                              # DW_TAG_typedef
	.byte	0                               # DW_CHILDREN_no
	.byte	73                              # DW_AT_type
	.byte	19                              # DW_FORM_ref4
	.byte	3                               # DW_AT_name
	.byte	14                              # DW_FORM_strp
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	0                               # EOM(3)
	.section	.debug_info,"dr"
.Lsection_info:
.Lcu_begin0:
	.long	.Ldebug_info_end0-.Ldebug_info_start0 # Length of Unit
.Ldebug_info_start0:
	.short	4                               # DWARF version number
	.secrel32	.Lsection_abbrev        # Offset Into Abbrev. Section
	.byte	8                               # Address Size (in bytes)
	.byte	1                               # Abbrev [1] 0xb:0x1c5 DW_TAG_compile_unit
	.secrel32	.Linfo_string0          # DW_AT_producer
	.short	29                              # DW_AT_language
	.secrel32	.Linfo_string1          # DW_AT_name
	.secrel32	.Lline_table_start0     # DW_AT_stmt_list
	.secrel32	.Linfo_string2          # DW_AT_comp_dir
	.quad	.Lfunc_begin0                   # DW_AT_low_pc
	.long	.Lfunc_end1-.Lfunc_begin0       # DW_AT_high_pc
	.byte	2                               # Abbrev [2] 0x2a:0x11 DW_TAG_variable
	.long	59                              # DW_AT_type
	.byte	1                               # DW_AT_decl_file
	.byte	29                              # DW_AT_decl_line
	.byte	9                               # DW_AT_location
	.byte	3
	.quad	.L.str
	.byte	3                               # Abbrev [3] 0x3b:0xc DW_TAG_array_type
	.long	71                              # DW_AT_type
	.byte	4                               # Abbrev [4] 0x40:0x6 DW_TAG_subrange_type
	.long	78                              # DW_AT_type
	.byte	20                              # DW_AT_count
	.byte	0                               # End Of Children Mark
	.byte	5                               # Abbrev [5] 0x47:0x7 DW_TAG_base_type
	.secrel32	.Linfo_string3          # DW_AT_name
	.byte	6                               # DW_AT_encoding
	.byte	1                               # DW_AT_byte_size
	.byte	6                               # Abbrev [6] 0x4e:0x7 DW_TAG_base_type
	.secrel32	.Linfo_string4          # DW_AT_name
	.byte	8                               # DW_AT_byte_size
	.byte	7                               # DW_AT_encoding
	.byte	2                               # Abbrev [2] 0x55:0x11 DW_TAG_variable
	.long	102                             # DW_AT_type
	.byte	1                               # DW_AT_decl_file
	.byte	29                              # DW_AT_decl_line
	.byte	9                               # DW_AT_location
	.byte	3
	.quad	.L.str.1
	.byte	3                               # Abbrev [3] 0x66:0xc DW_TAG_array_type
	.long	71                              # DW_AT_type
	.byte	4                               # Abbrev [4] 0x6b:0x6 DW_TAG_subrange_type
	.long	78                              # DW_AT_type
	.byte	36                              # DW_AT_count
	.byte	0                               # End Of Children Mark
	.byte	2                               # Abbrev [2] 0x72:0x11 DW_TAG_variable
	.long	131                             # DW_AT_type
	.byte	1                               # DW_AT_decl_file
	.byte	31                              # DW_AT_decl_line
	.byte	9                               # DW_AT_location
	.byte	3
	.quad	.L.str.2
	.byte	3                               # Abbrev [3] 0x83:0xc DW_TAG_array_type
	.long	71                              # DW_AT_type
	.byte	4                               # Abbrev [4] 0x88:0x6 DW_TAG_subrange_type
	.long	78                              # DW_AT_type
	.byte	32                              # DW_AT_count
	.byte	0                               # End Of Children Mark
	.byte	2                               # Abbrev [2] 0x8f:0x11 DW_TAG_variable
	.long	160                             # DW_AT_type
	.byte	1                               # DW_AT_decl_file
	.byte	31                              # DW_AT_decl_line
	.byte	9                               # DW_AT_location
	.byte	3
	.quad	.L.str.3
	.byte	3                               # Abbrev [3] 0xa0:0xc DW_TAG_array_type
	.long	71                              # DW_AT_type
	.byte	4                               # Abbrev [4] 0xa5:0x6 DW_TAG_subrange_type
	.long	78                              # DW_AT_type
	.byte	19                              # DW_AT_count
	.byte	0                               # End Of Children Mark
	.byte	7                               # Abbrev [7] 0xac:0x29 DW_TAG_subprogram
	.quad	.Lfunc_begin0                   # DW_AT_low_pc
	.long	.Lfunc_end0-.Lfunc_begin0       # DW_AT_high_pc
	.byte	1                               # DW_AT_frame_base
	.byte	86
	.secrel32	.Linfo_string5          # DW_AT_name
	.byte	1                               # DW_AT_decl_file
	.byte	24                              # DW_AT_decl_line
	.long	286                             # DW_AT_type
                                        # DW_AT_external
	.byte	8                               # Abbrev [8] 0xc5:0xf DW_TAG_variable
	.byte	3                               # DW_AT_location
	.byte	145
	.ascii	"\260\177"
	.secrel32	.Linfo_string8          # DW_AT_name
	.byte	1                               # DW_AT_decl_file
	.byte	25                              # DW_AT_decl_line
	.long	293                             # DW_AT_type
	.byte	0                               # End Of Children Mark
	.byte	9                               # Abbrev [9] 0xd5:0x49 DW_TAG_subprogram
	.quad	.Lfunc_begin1                   # DW_AT_low_pc
	.long	.Lfunc_end1-.Lfunc_begin1       # DW_AT_high_pc
	.byte	1                               # DW_AT_frame_base
	.byte	87
	.secrel32	.Linfo_string7          # DW_AT_name
	.byte	2                               # DW_AT_decl_file
	.short	379                             # DW_AT_decl_line
                                        # DW_AT_prototyped
	.long	286                             # DW_AT_type
	.byte	10                              # Abbrev [10] 0xef:0xf DW_TAG_formal_parameter
	.byte	2                               # DW_AT_location
	.byte	145
	.byte	48
	.secrel32	.Linfo_string20         # DW_AT_name
	.byte	2                               # DW_AT_decl_file
	.short	379                             # DW_AT_decl_line
	.long	439                             # DW_AT_type
	.byte	11                              # Abbrev [11] 0xfe:0xf DW_TAG_variable
	.byte	2                               # DW_AT_location
	.byte	145
	.byte	44
	.secrel32	.Linfo_string21         # DW_AT_name
	.byte	2                               # DW_AT_decl_file
	.short	381                             # DW_AT_decl_line
	.long	286                             # DW_AT_type
	.byte	11                              # Abbrev [11] 0x10d:0xf DW_TAG_variable
	.byte	2                               # DW_AT_location
	.byte	145
	.byte	32
	.secrel32	.Linfo_string22         # DW_AT_name
	.byte	2                               # DW_AT_decl_file
	.short	382                             # DW_AT_decl_line
	.long	449                             # DW_AT_type
	.byte	12                              # Abbrev [12] 0x11c:0x1 DW_TAG_unspecified_parameters
	.byte	0                               # End Of Children Mark
	.byte	5                               # Abbrev [5] 0x11e:0x7 DW_TAG_base_type
	.secrel32	.Linfo_string6          # DW_AT_name
	.byte	5                               # DW_AT_encoding
	.byte	4                               # DW_AT_byte_size
	.byte	13                              # Abbrev [13] 0x125:0xb DW_TAG_typedef
	.long	304                             # DW_AT_type
	.secrel32	.Linfo_string19         # DW_AT_name
	.byte	1                               # DW_AT_decl_file
	.byte	6                               # DW_AT_decl_line
	.byte	14                              # Abbrev [14] 0x130:0x69 DW_TAG_structure_type
	.secrel32	.Linfo_string19         # DW_AT_name
	.byte	44                              # DW_AT_byte_size
	.byte	1                               # DW_AT_decl_file
	.byte	6                               # DW_AT_decl_line
	.byte	15                              # Abbrev [15] 0x138:0xc DW_TAG_member
	.secrel32	.Linfo_string9          # DW_AT_name
	.long	409                             # DW_AT_type
	.byte	1                               # DW_AT_decl_file
	.byte	7                               # DW_AT_decl_line
	.byte	0                               # DW_AT_data_member_location
	.byte	15                              # Abbrev [15] 0x144:0xc DW_TAG_member
	.secrel32	.Linfo_string12         # DW_AT_name
	.long	427                             # DW_AT_type
	.byte	1                               # DW_AT_decl_file
	.byte	9                               # DW_AT_decl_line
	.byte	4                               # DW_AT_data_member_location
	.byte	15                              # Abbrev [15] 0x150:0xc DW_TAG_member
	.secrel32	.Linfo_string13         # DW_AT_name
	.long	409                             # DW_AT_type
	.byte	1                               # DW_AT_decl_file
	.byte	11                              # DW_AT_decl_line
	.byte	20                              # DW_AT_data_member_location
	.byte	15                              # Abbrev [15] 0x15c:0xc DW_TAG_member
	.secrel32	.Linfo_string14         # DW_AT_name
	.long	409                             # DW_AT_type
	.byte	1                               # DW_AT_decl_file
	.byte	13                              # DW_AT_decl_line
	.byte	24                              # DW_AT_data_member_location
	.byte	15                              # Abbrev [15] 0x168:0xc DW_TAG_member
	.secrel32	.Linfo_string15         # DW_AT_name
	.long	409                             # DW_AT_type
	.byte	1                               # DW_AT_decl_file
	.byte	14                              # DW_AT_decl_line
	.byte	28                              # DW_AT_data_member_location
	.byte	15                              # Abbrev [15] 0x174:0xc DW_TAG_member
	.secrel32	.Linfo_string16         # DW_AT_name
	.long	409                             # DW_AT_type
	.byte	1                               # DW_AT_decl_file
	.byte	16                              # DW_AT_decl_line
	.byte	32                              # DW_AT_data_member_location
	.byte	15                              # Abbrev [15] 0x180:0xc DW_TAG_member
	.secrel32	.Linfo_string17         # DW_AT_name
	.long	409                             # DW_AT_type
	.byte	1                               # DW_AT_decl_file
	.byte	17                              # DW_AT_decl_line
	.byte	36                              # DW_AT_data_member_location
	.byte	15                              # Abbrev [15] 0x18c:0xc DW_TAG_member
	.secrel32	.Linfo_string18         # DW_AT_name
	.long	409                             # DW_AT_type
	.byte	1                               # DW_AT_decl_file
	.byte	19                              # DW_AT_decl_line
	.byte	40                              # DW_AT_data_member_location
	.byte	0                               # End Of Children Mark
	.byte	13                              # Abbrev [13] 0x199:0xb DW_TAG_typedef
	.long	420                             # DW_AT_type
	.secrel32	.Linfo_string11         # DW_AT_name
	.byte	1                               # DW_AT_decl_file
	.byte	3                               # DW_AT_decl_line
	.byte	5                               # Abbrev [5] 0x1a4:0x7 DW_TAG_base_type
	.secrel32	.Linfo_string10         # DW_AT_name
	.byte	7                               # DW_AT_encoding
	.byte	4                               # DW_AT_byte_size
	.byte	3                               # Abbrev [3] 0x1ab:0xc DW_TAG_array_type
	.long	71                              # DW_AT_type
	.byte	4                               # Abbrev [4] 0x1b0:0x6 DW_TAG_subrange_type
	.long	78                              # DW_AT_type
	.byte	16                              # DW_AT_count
	.byte	0                               # End Of Children Mark
	.byte	16                              # Abbrev [16] 0x1b7:0x5 DW_TAG_pointer_type
	.long	444                             # DW_AT_type
	.byte	17                              # Abbrev [17] 0x1bc:0x5 DW_TAG_const_type
	.long	71                              # DW_AT_type
	.byte	18                              # Abbrev [18] 0x1c1:0x9 DW_TAG_typedef
	.long	458                             # DW_AT_type
	.secrel32	.Linfo_string23         # DW_AT_name
	.byte	16                              # Abbrev [16] 0x1ca:0x5 DW_TAG_pointer_type
	.long	71                              # DW_AT_type
	.byte	0                               # End Of Children Mark
.Ldebug_info_end0:
	.section	.debug_str,"dr"
.Linfo_string:
.Linfo_string0:
	.asciz	"clang version 17.0.2 (https://github.com/llvm/llvm-project.git b2417f51dbbd7435eb3aaf203de24de6754da50e)" # string offset=0
.Linfo_string1:
	.asciz	"D:/ZetaDevelop/ZetaExe/test_cpuid.c" # string offset=105
.Linfo_string2:
	.asciz	"D:/ZetaDevelop/ZetaExe"        # string offset=141
.Linfo_string3:
	.asciz	"char"                          # string offset=164
.Linfo_string4:
	.asciz	"__ARRAY_SIZE_TYPE__"           # string offset=169
.Linfo_string5:
	.asciz	"main"                          # string offset=189
.Linfo_string6:
	.asciz	"int"                           # string offset=194
.Linfo_string7:
	.asciz	"printf"                        # string offset=198
.Linfo_string8:
	.asciz	"cpu_info"                      # string offset=205
.Linfo_string9:
	.asciz	"max_func_num"                  # string offset=214
.Linfo_string10:
	.asciz	"unsigned int"                  # string offset=227
.Linfo_string11:
	.asciz	"EX"                            # string offset=240
.Linfo_string12:
	.asciz	"vender_id"                     # string offset=243
.Linfo_string13:
	.asciz	"type"                          # string offset=253
.Linfo_string14:
	.asciz	"family"                        # string offset=258
.Linfo_string15:
	.asciz	"ext_family"                    # string offset=265
.Linfo_string16:
	.asciz	"model"                         # string offset=276
.Linfo_string17:
	.asciz	"ext_model"                     # string offset=282
.Linfo_string18:
	.asciz	"step"                          # string offset=292
.Linfo_string19:
	.asciz	"CpuInfo"                       # string offset=297
.Linfo_string20:
	.asciz	"__format"                      # string offset=305
.Linfo_string21:
	.asciz	"__retval"                      # string offset=314
.Linfo_string22:
	.asciz	"__local_argv"                  # string offset=323
.Linfo_string23:
	.asciz	"__builtin_va_list"             # string offset=336
	.addrsig
	.addrsig_sym GetCpuInfo
	.addrsig_sym printf
	.addrsig_sym __mingw_vfprintf
	.section	.debug_line,"dr"
.Lsection_line:
.Lline_table_start0:
