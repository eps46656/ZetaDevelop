	.text
	.def	@feat.00;
	.scl	3;
	.type	0;
	.endef
	.globl	@feat.00
.set @feat.00, 0
	.file	"test_context.c"
	.file	1 "D:/ZetaDevelop/ZetaExe" "test_context.c"
	.file	2 "C:/Program Files/llvm-mingw-20231003-msvcrt-x86_64/include" "corecrt.h"
	.def	Thread_Create;
	.scl	2;
	.type	32;
	.endef
	.globl	Thread_Create                   # -- Begin function Thread_Create
	.p2align	4, 0x90
Thread_Create:                          # @Thread_Create
.Lfunc_begin0:
	.loc	1 54 0                          # test_context.c:54:0
.seh_proc Thread_Create
# %bb.0:
	pushq	%rsi
	.seh_pushreg %rsi
	pushq	%rdi
	.seh_pushreg %rdi
	subq	$232, %rsp
	.seh_stackalloc 232
	movaps	%xmm15, 208(%rsp)               # 16-byte Spill
	.seh_savexmm %xmm15, 208
	movaps	%xmm14, 192(%rsp)               # 16-byte Spill
	.seh_savexmm %xmm14, 192
	movaps	%xmm13, 176(%rsp)               # 16-byte Spill
	.seh_savexmm %xmm13, 176
	movaps	%xmm12, 160(%rsp)               # 16-byte Spill
	.seh_savexmm %xmm12, 160
	movaps	%xmm11, 144(%rsp)               # 16-byte Spill
	.seh_savexmm %xmm11, 144
	movaps	%xmm10, 128(%rsp)               # 16-byte Spill
	.seh_savexmm %xmm10, 128
	movaps	%xmm9, 112(%rsp)                # 16-byte Spill
	.seh_savexmm %xmm9, 112
	movaps	%xmm8, 96(%rsp)                 # 16-byte Spill
	.seh_savexmm %xmm8, 96
	movaps	%xmm7, 80(%rsp)                 # 16-byte Spill
	.seh_savexmm %xmm7, 80
	movaps	%xmm6, 64(%rsp)                 # 16-byte Spill
	.seh_savexmm %xmm6, 64
	.seh_endprologue
	movq	%rcx, 56(%rsp)
	movq	%rdx, 48(%rsp)
.Ltmp0:
	.loc	1 55 18 prologue_end            # test_context.c:55:18
	movq	$16777216, 40(%rsp)             # imm = 0x1000000
	.loc	1 58 38                         # test_context.c:58:38
	movl	$16777216, %ecx                 # imm = 0x1000000
	callq	malloc
	movq	%rax, %rdi
	.loc	1 58 57 is_stmt 0               # test_context.c:58:57
	addq	$16777216, %rdi                 # imm = 0x1000000
	.loc	1 58 71                         # test_context.c:58:71
	movq	48(%rsp), %rsi
	.loc	1 58 9                          # test_context.c:58:9
	callq	Thread_Create_
	movq	%rax, %rcx
	.loc	1 57 5 is_stmt 1                # test_context.c:57:5
	movq	56(%rsp), %rax
	.loc	1 57 19 is_stmt 0               # test_context.c:57:19
	movq	%rcx, (%rax)
	movaps	64(%rsp), %xmm6                 # 16-byte Reload
	movaps	80(%rsp), %xmm7                 # 16-byte Reload
	movaps	96(%rsp), %xmm8                 # 16-byte Reload
	movaps	112(%rsp), %xmm9                # 16-byte Reload
	movaps	128(%rsp), %xmm10               # 16-byte Reload
	movaps	144(%rsp), %xmm11               # 16-byte Reload
	movaps	160(%rsp), %xmm12               # 16-byte Reload
	movaps	176(%rsp), %xmm13               # 16-byte Reload
	movaps	192(%rsp), %xmm14               # 16-byte Reload
	movaps	208(%rsp), %xmm15               # 16-byte Reload
	.loc	1 59 1 is_stmt 1                # test_context.c:59:1
	.loc	1 59 1 epilogue_begin is_stmt 0 # test_context.c:59:1
	addq	$232, %rsp
	popq	%rdi
	popq	%rsi
	retq
.Ltmp1:
.Lfunc_end0:
	.seh_endproc
                                        # -- End function
	.def	Func1;
	.scl	2;
	.type	32;
	.endef
	.globl	Func1                           # -- Begin function Func1
	.p2align	4, 0x90
Func1:                                  # @Func1
.Lfunc_begin1:
	.loc	1 67 0 is_stmt 1                # test_context.c:67:0
.seh_proc Func1
# %bb.0:
	subq	$40, %rsp
	.seh_stackalloc 40
	.seh_endprologue
.Ltmp2:
	.loc	1 68 5 prologue_end             # test_context.c:68:5
	leaq	.L.str(%rip), %rcx
	leaq	.L.str.1(%rip), %rdx
	movl	$68, %r8d
	callq	printf
	.loc	1 69 5                          # test_context.c:69:5
	leaq	.L.str.2(%rip), %rcx
	leaq	main_thread(%rip), %rdx
	callq	printf
	.loc	1 70 54                         # test_context.c:70:54
	movq	main_thread(%rip), %rdx
	.loc	1 70 5 is_stmt 0                # test_context.c:70:5
	leaq	.L.str.3(%rip), %rcx
	callq	printf
	.loc	1 71 28 is_stmt 1               # test_context.c:71:28
	movq	main_thread(%rip), %rax
	.loc	1 71 8 is_stmt 0                # test_context.c:71:8
	movq	%rax, 32(%rsp)
	.loc	1 72 42 is_stmt 1               # test_context.c:72:42
	movq	32(%rsp), %rdx
	.loc	1 72 5 is_stmt 0                # test_context.c:72:5
	leaq	.L.str.3(%rip), %rcx
	callq	printf
	.loc	1 73 5 is_stmt 1                # test_context.c:73:5
	leaq	.L.str(%rip), %rcx
	leaq	.L.str.1(%rip), %rdx
	movl	$73, %r8d
	callq	printf
	.loc	1 74 5                          # test_context.c:74:5
	xorl	%ecx, %ecx
	callq	exit
	int3
.Ltmp3:
.Lfunc_end1:
	.seh_endproc
                                        # -- End function
	.def	printf;
	.scl	3;
	.type	32;
	.endef
	.p2align	4, 0x90                         # -- Begin function printf
printf:                                 # @printf
.Lfunc_begin2:
	.file	3 "C:/Program Files/llvm-mingw-20231003-msvcrt-x86_64/include" "stdio.h"
	.loc	3 380 0                         # C:/Program Files/llvm-mingw-20231003-msvcrt-x86_64/include/stdio.h:380:0
.seh_proc printf
# %bb.0:
	subq	$56, %rsp
	.seh_stackalloc 56
	.seh_endprologue
	movq	%r9, 88(%rsp)
	movq	%r8, 80(%rsp)
	movq	%rdx, 72(%rsp)
	movq	%rcx, 48(%rsp)
	leaq	72(%rsp), %rax
.Ltmp4:
	.loc	3 382 35 prologue_end           # C:/Program Files/llvm-mingw-20231003-msvcrt-x86_64/include/stdio.h:382:35
	movq	%rax, 32(%rsp)
	.loc	3 383 32                        # C:/Program Files/llvm-mingw-20231003-msvcrt-x86_64/include/stdio.h:383:32
	movq	__imp___acrt_iob_func(%rip), %rax
	movl	$1, %ecx
	callq	*%rax
	movq	%rax, %rcx
	.loc	3 383 40 is_stmt 0              # C:/Program Files/llvm-mingw-20231003-msvcrt-x86_64/include/stdio.h:383:40
	movq	48(%rsp), %rdx
	.loc	3 383 50                        # C:/Program Files/llvm-mingw-20231003-msvcrt-x86_64/include/stdio.h:383:50
	movq	32(%rsp), %r8
	.loc	3 383 14                        # C:/Program Files/llvm-mingw-20231003-msvcrt-x86_64/include/stdio.h:383:14
	callq	__mingw_vfprintf
	.loc	3 383 12                        # C:/Program Files/llvm-mingw-20231003-msvcrt-x86_64/include/stdio.h:383:12
	movl	%eax, 44(%rsp)
	.loc	3 385 10 is_stmt 1              # C:/Program Files/llvm-mingw-20231003-msvcrt-x86_64/include/stdio.h:385:10
	movl	44(%rsp), %eax
	.loc	3 385 3 is_stmt 0               # C:/Program Files/llvm-mingw-20231003-msvcrt-x86_64/include/stdio.h:385:3
	.loc	3 385 3 epilogue_begin          # C:/Program Files/llvm-mingw-20231003-msvcrt-x86_64/include/stdio.h:385:3
	addq	$56, %rsp
	retq
.Ltmp5:
.Lfunc_end2:
	.seh_endproc
                                        # -- End function
	.def	Func2;
	.scl	2;
	.type	32;
	.endef
	.globl	Func2                           # -- Begin function Func2
	.p2align	4, 0x90
Func2:                                  # @Func2
.Lfunc_begin3:
	.loc	1 80 0 is_stmt 1                # test_context.c:80:0
.seh_proc Func2
# %bb.0:
	pushq	%rsi
	.seh_pushreg %rsi
	pushq	%rdi
	.seh_pushreg %rdi
	subq	$216, %rsp
	.seh_stackalloc 216
	movaps	%xmm15, 192(%rsp)               # 16-byte Spill
	.seh_savexmm %xmm15, 192
	movaps	%xmm14, 176(%rsp)               # 16-byte Spill
	.seh_savexmm %xmm14, 176
	movaps	%xmm13, 160(%rsp)               # 16-byte Spill
	.seh_savexmm %xmm13, 160
	movaps	%xmm12, 144(%rsp)               # 16-byte Spill
	.seh_savexmm %xmm12, 144
	movaps	%xmm11, 128(%rsp)               # 16-byte Spill
	.seh_savexmm %xmm11, 128
	movaps	%xmm10, 112(%rsp)               # 16-byte Spill
	.seh_savexmm %xmm10, 112
	movaps	%xmm9, 96(%rsp)                 # 16-byte Spill
	.seh_savexmm %xmm9, 96
	movaps	%xmm8, 80(%rsp)                 # 16-byte Spill
	.seh_savexmm %xmm8, 80
	movaps	%xmm7, 64(%rsp)                 # 16-byte Spill
	.seh_savexmm %xmm7, 64
	movaps	%xmm6, 48(%rsp)                 # 16-byte Spill
	.seh_savexmm %xmm6, 48
	.seh_endprologue
	.loc	1 81 5 prologue_end             # test_context.c:81:5
	leaq	.L.str.4(%rip), %rcx
	callq	printf
.Ltmp6:
	.loc	1 83 14                         # test_context.c:83:14
	movl	$0, 44(%rsp)
.LBB3_1:                                # =>This Inner Loop Header: Depth=1
.Ltmp7:
	.loc	1 83 23 is_stmt 0               # test_context.c:83:23
	cmpl	$10, 44(%rsp)
.Ltmp8:
	.loc	1 83 5                          # test_context.c:83:5
	jge	.LBB3_4
# %bb.2:                                #   in Loop: Header=BB3_1 Depth=1
.Ltmp9:
	.loc	1 84 21 is_stmt 1               # test_context.c:84:21
	movl	44(%rsp), %eax
	.loc	1 84 19 is_stmt 0               # test_context.c:84:19
	movl	%eax, func2_ret(%rip)
	.loc	1 85 9 is_stmt 1                # test_context.c:85:9
	leaq	func2_thread(%rip), %rdi
	leaq	main_thread(%rip), %rsi
	callq	Thread_Switch
.Ltmp10:
# %bb.3:                                #   in Loop: Header=BB3_1 Depth=1
	.loc	1 83 29                         # test_context.c:83:29
	movl	44(%rsp), %eax
	addl	$1, %eax
	movl	%eax, 44(%rsp)
	.loc	1 83 5 is_stmt 0                # test_context.c:83:5
	jmp	.LBB3_1
.Ltmp11:
.LBB3_4:
	.loc	1 88 5 is_stmt 1                # test_context.c:88:5
	jmp	.LBB3_5
.LBB3_5:                                # =>This Inner Loop Header: Depth=1
.Ltmp12:
	.loc	1 89 19                         # test_context.c:89:19
	movl	$-1, func2_ret(%rip)
	.loc	1 90 9                          # test_context.c:90:9
	leaq	func2_thread(%rip), %rdi
	leaq	main_thread(%rip), %rsi
	callq	Thread_Switch
.Ltmp13:
	.loc	1 88 5                          # test_context.c:88:5
	jmp	.LBB3_5
.Ltmp14:
.Lfunc_end3:
	.seh_endproc
                                        # -- End function
	.def	Func3;
	.scl	2;
	.type	32;
	.endef
	.globl	Func3                           # -- Begin function Func3
	.p2align	4, 0x90
Func3:                                  # @Func3
.Lfunc_begin4:
	.loc	1 94 0                          # test_context.c:94:0
.seh_proc Func3
# %bb.0:
	subq	$40, %rsp
	.seh_stackalloc 40
	.seh_endprologue
.Ltmp15:
	.loc	1 94 16 prologue_end            # test_context.c:94:16
	leaq	.L.str.5(%rip), %rcx
	callq	printf
	.loc	1 94 35 is_stmt 0               # test_context.c:94:35
	nop
	.loc	1 94 35 epilogue_begin          # test_context.c:94:35
	addq	$40, %rsp
	retq
.Ltmp16:
.Lfunc_end4:
	.seh_endproc
                                        # -- End function
	.def	PrintPCB;
	.scl	2;
	.type	32;
	.endef
	.globl	PrintPCB                        # -- Begin function PrintPCB
	.p2align	4, 0x90
PrintPCB:                               # @PrintPCB
.Lfunc_begin5:
	.loc	1 96 0 is_stmt 1                # test_context.c:96:0
.seh_proc PrintPCB
# %bb.0:
	subq	$40, %rsp
	.seh_stackalloc 40
	.seh_endprologue
	movq	%rcx, 32(%rsp)
.Ltmp17:
	.loc	1 97 33 prologue_end            # test_context.c:97:33
	movq	32(%rsp), %rax
	.loc	1 97 38 is_stmt 0               # test_context.c:97:38
	movq	(%rax), %rdx
	.loc	1 97 43                         # test_context.c:97:43
	movq	32(%rsp), %rax
	.loc	1 97 48                         # test_context.c:97:48
	movq	(%rax), %r8
	.loc	1 97 5                          # test_context.c:97:5
	leaq	.L.str.6(%rip), %rcx
	callq	printf
	.loc	1 98 33 is_stmt 1               # test_context.c:98:33
	movq	32(%rsp), %rax
	.loc	1 98 38 is_stmt 0               # test_context.c:98:38
	movq	8(%rax), %rdx
	.loc	1 98 43                         # test_context.c:98:43
	movq	32(%rsp), %rax
	.loc	1 98 48                         # test_context.c:98:48
	movq	8(%rax), %r8
	.loc	1 98 5                          # test_context.c:98:5
	leaq	.L.str.7(%rip), %rcx
	callq	printf
	.loc	1 99 33 is_stmt 1               # test_context.c:99:33
	movq	32(%rsp), %rax
	.loc	1 99 38 is_stmt 0               # test_context.c:99:38
	movq	16(%rax), %rdx
	.loc	1 99 43                         # test_context.c:99:43
	movq	32(%rsp), %rax
	.loc	1 99 48                         # test_context.c:99:48
	movq	16(%rax), %r8
	.loc	1 99 5                          # test_context.c:99:5
	leaq	.L.str.8(%rip), %rcx
	callq	printf
	.loc	1 100 33 is_stmt 1              # test_context.c:100:33
	movq	32(%rsp), %rax
	.loc	1 100 38 is_stmt 0              # test_context.c:100:38
	movq	24(%rax), %rdx
	.loc	1 100 43                        # test_context.c:100:43
	movq	32(%rsp), %rax
	.loc	1 100 48                        # test_context.c:100:48
	movq	24(%rax), %r8
	.loc	1 100 5                         # test_context.c:100:5
	leaq	.L.str.9(%rip), %rcx
	callq	printf
	.loc	1 102 33 is_stmt 1              # test_context.c:102:33
	movq	32(%rsp), %rax
	.loc	1 102 38 is_stmt 0              # test_context.c:102:38
	movq	32(%rax), %rdx
	.loc	1 102 43                        # test_context.c:102:43
	movq	32(%rsp), %rax
	.loc	1 102 48                        # test_context.c:102:48
	movq	32(%rax), %r8
	.loc	1 102 5                         # test_context.c:102:5
	leaq	.L.str.10(%rip), %rcx
	callq	printf
	.loc	1 103 33 is_stmt 1              # test_context.c:103:33
	movq	32(%rsp), %rax
	.loc	1 103 38 is_stmt 0              # test_context.c:103:38
	movq	40(%rax), %rdx
	.loc	1 103 43                        # test_context.c:103:43
	movq	32(%rsp), %rax
	.loc	1 103 48                        # test_context.c:103:48
	movq	40(%rax), %r8
	.loc	1 103 5                         # test_context.c:103:5
	leaq	.L.str.11(%rip), %rcx
	callq	printf
	.loc	1 105 33 is_stmt 1              # test_context.c:105:33
	movq	32(%rsp), %rax
	.loc	1 105 38 is_stmt 0              # test_context.c:105:38
	movq	48(%rax), %rdx
	.loc	1 105 43                        # test_context.c:105:43
	movq	32(%rsp), %rax
	.loc	1 105 48                        # test_context.c:105:48
	movq	48(%rax), %r8
	.loc	1 105 5                         # test_context.c:105:5
	leaq	.L.str.12(%rip), %rcx
	callq	printf
	.loc	1 106 33 is_stmt 1              # test_context.c:106:33
	movq	32(%rsp), %rax
	.loc	1 106 38 is_stmt 0              # test_context.c:106:38
	movq	56(%rax), %rdx
	.loc	1 106 43                        # test_context.c:106:43
	movq	32(%rsp), %rax
	.loc	1 106 48                        # test_context.c:106:48
	movq	56(%rax), %r8
	.loc	1 106 5                         # test_context.c:106:5
	leaq	.L.str.13(%rip), %rcx
	callq	printf
	.loc	1 108 32 is_stmt 1              # test_context.c:108:32
	movq	32(%rsp), %rax
	.loc	1 108 37 is_stmt 0              # test_context.c:108:37
	movq	64(%rax), %rdx
	.loc	1 108 41                        # test_context.c:108:41
	movq	32(%rsp), %rax
	.loc	1 108 46                        # test_context.c:108:46
	movq	64(%rax), %r8
	.loc	1 108 5                         # test_context.c:108:5
	leaq	.L.str.14(%rip), %rcx
	callq	printf
	.loc	1 109 32 is_stmt 1              # test_context.c:109:32
	movq	32(%rsp), %rax
	.loc	1 109 37 is_stmt 0              # test_context.c:109:37
	movq	72(%rax), %rdx
	.loc	1 109 41                        # test_context.c:109:41
	movq	32(%rsp), %rax
	.loc	1 109 46                        # test_context.c:109:46
	movq	72(%rax), %r8
	.loc	1 109 5                         # test_context.c:109:5
	leaq	.L.str.15(%rip), %rcx
	callq	printf
	.loc	1 110 33 is_stmt 1              # test_context.c:110:33
	movq	32(%rsp), %rax
	.loc	1 110 38 is_stmt 0              # test_context.c:110:38
	movq	80(%rax), %rdx
	.loc	1 110 43                        # test_context.c:110:43
	movq	32(%rsp), %rax
	.loc	1 110 48                        # test_context.c:110:48
	movq	80(%rax), %r8
	.loc	1 110 5                         # test_context.c:110:5
	leaq	.L.str.16(%rip), %rcx
	callq	printf
	.loc	1 111 33 is_stmt 1              # test_context.c:111:33
	movq	32(%rsp), %rax
	.loc	1 111 38 is_stmt 0              # test_context.c:111:38
	movq	88(%rax), %rdx
	.loc	1 111 43                        # test_context.c:111:43
	movq	32(%rsp), %rax
	.loc	1 111 48                        # test_context.c:111:48
	movq	88(%rax), %r8
	.loc	1 111 5                         # test_context.c:111:5
	leaq	.L.str.17(%rip), %rcx
	callq	printf
	.loc	1 113 33 is_stmt 1              # test_context.c:113:33
	movq	32(%rsp), %rax
	.loc	1 113 38 is_stmt 0              # test_context.c:113:38
	movq	96(%rax), %rdx
	.loc	1 113 43                        # test_context.c:113:43
	movq	32(%rsp), %rax
	.loc	1 113 48                        # test_context.c:113:48
	movq	96(%rax), %r8
	.loc	1 113 5                         # test_context.c:113:5
	leaq	.L.str.18(%rip), %rcx
	callq	printf
	.loc	1 114 33 is_stmt 1              # test_context.c:114:33
	movq	32(%rsp), %rax
	.loc	1 114 38 is_stmt 0              # test_context.c:114:38
	movq	104(%rax), %rdx
	.loc	1 114 43                        # test_context.c:114:43
	movq	32(%rsp), %rax
	.loc	1 114 48                        # test_context.c:114:48
	movq	104(%rax), %r8
	.loc	1 114 5                         # test_context.c:114:5
	leaq	.L.str.19(%rip), %rcx
	callq	printf
	.loc	1 115 33 is_stmt 1              # test_context.c:115:33
	movq	32(%rsp), %rax
	.loc	1 115 38 is_stmt 0              # test_context.c:115:38
	movq	112(%rax), %rdx
	.loc	1 115 43                        # test_context.c:115:43
	movq	32(%rsp), %rax
	.loc	1 115 48                        # test_context.c:115:48
	movq	112(%rax), %r8
	.loc	1 115 5                         # test_context.c:115:5
	leaq	.L.str.20(%rip), %rcx
	callq	printf
	.loc	1 116 33 is_stmt 1              # test_context.c:116:33
	movq	32(%rsp), %rax
	.loc	1 116 38 is_stmt 0              # test_context.c:116:38
	movq	120(%rax), %rdx
	.loc	1 116 43                        # test_context.c:116:43
	movq	32(%rsp), %rax
	.loc	1 116 48                        # test_context.c:116:48
	movq	120(%rax), %r8
	.loc	1 116 5                         # test_context.c:116:5
	leaq	.L.str.21(%rip), %rcx
	callq	printf
	.loc	1 118 36 is_stmt 1              # test_context.c:118:36
	movq	32(%rsp), %rax
	.loc	1 118 41 is_stmt 0              # test_context.c:118:41
	movq	128(%rax), %rdx
	.loc	1 118 49                        # test_context.c:118:49
	movq	32(%rsp), %rax
	.loc	1 118 54                        # test_context.c:118:54
	movq	128(%rax), %r8
	.loc	1 118 5                         # test_context.c:118:5
	leaq	.L.str.22(%rip), %rcx
	callq	printf
	.loc	1 120 33 is_stmt 1              # test_context.c:120:33
	movq	32(%rsp), %rax
	.loc	1 120 38 is_stmt 0              # test_context.c:120:38
	movq	136(%rax), %rdx
	.loc	1 120 43                        # test_context.c:120:43
	movq	32(%rsp), %rax
	.loc	1 120 48                        # test_context.c:120:48
	movq	136(%rax), %r8
	.loc	1 120 5                         # test_context.c:120:5
	leaq	.L.str.23(%rip), %rcx
	callq	printf
	.loc	1 121 1 is_stmt 1               # test_context.c:121:1
	nop
	.loc	1 121 1 epilogue_begin is_stmt 0 # test_context.c:121:1
	addq	$40, %rsp
	retq
.Ltmp18:
.Lfunc_end5:
	.seh_endproc
                                        # -- End function
	.def	main;
	.scl	2;
	.type	32;
	.endef
	.globl	main                            # -- Begin function main
	.p2align	4, 0x90
main:                                   # @main
.Lfunc_begin6:
	.loc	1 123 0 is_stmt 1               # test_context.c:123:0
.seh_proc main
# %bb.0:
	pushq	%rbp
	.seh_pushreg %rbp
	pushq	%rsi
	.seh_pushreg %rsi
	pushq	%rdi
	.seh_pushreg %rdi
	subq	$208, %rsp
	.seh_stackalloc 208
	leaq	128(%rsp), %rbp
	.seh_setframe %rbp, 128
	movaps	%xmm15, 64(%rbp)                # 16-byte Spill
	.seh_savexmm %xmm15, 192
	movaps	%xmm14, 48(%rbp)                # 16-byte Spill
	.seh_savexmm %xmm14, 176
	movaps	%xmm13, 32(%rbp)                # 16-byte Spill
	.seh_savexmm %xmm13, 160
	movaps	%xmm12, 16(%rbp)                # 16-byte Spill
	.seh_savexmm %xmm12, 144
	movaps	%xmm11, (%rbp)                  # 16-byte Spill
	.seh_savexmm %xmm11, 128
	movaps	%xmm10, -16(%rbp)               # 16-byte Spill
	.seh_savexmm %xmm10, 112
	movaps	%xmm9, -32(%rbp)                # 16-byte Spill
	.seh_savexmm %xmm9, 96
	movaps	%xmm8, -48(%rbp)                # 16-byte Spill
	.seh_savexmm %xmm8, 80
	movaps	%xmm7, -64(%rbp)                # 16-byte Spill
	.seh_savexmm %xmm7, 64
	movaps	%xmm6, -80(%rbp)                # 16-byte Spill
	.seh_savexmm %xmm6, 48
	.seh_endprologue
	callq	__main
	movl	$0, -84(%rbp)
.Ltmp19:
	.loc	1 124 5 prologue_end            # test_context.c:124:5
	leaq	func1_thread(%rip), %rcx
	leaq	Func1(%rip), %rdx
	callq	Thread_Create
	.loc	1 125 5                         # test_context.c:125:5
	leaq	func2_thread(%rip), %rcx
	leaq	Func2(%rip), %rdx
	callq	Thread_Create
	.loc	1 127 15                        # test_context.c:127:15
	movl	$-2, func2_ret(%rip)
	.loc	1 129 5                         # test_context.c:129:5
	leaq	.L.str(%rip), %rcx
	leaq	.L.str.1(%rip), %rdx
	movl	$129, %r8d
	callq	printf
.LBB6_1:                                # =>This Inner Loop Header: Depth=1
.Ltmp20:
	.loc	1 132 9                         # test_context.c:132:9
	leaq	main_thread(%rip), %rdi
	leaq	func2_thread(%rip), %rsi
	callq	Thread_Switch
	.loc	1 133 9                         # test_context.c:133:9
	movl	func2_ret(%rip), %ecx
	movq	%rsp, %rax
	movl	%ecx, 32(%rax)
	leaq	.L.str.24(%rip), %rcx
	leaq	.L.str.1(%rip), %rdx
	leaq	.L.str.25(%rip), %r9
	movl	$133, %r8d
	callq	printf
.Ltmp21:
# %bb.2:                                #   in Loop: Header=BB6_1 Depth=1
	.loc	1 134 24                        # test_context.c:134:24
	cmpl	$-1, func2_ret(%rip)
.Ltmp22:
	.loc	1 134 5 is_stmt 0               # test_context.c:134:5
	jne	.LBB6_1
.Ltmp23:
# %bb.3:
	.loc	1 136 5 is_stmt 1               # test_context.c:136:5
	xorl	%eax, %eax
	movaps	-80(%rbp), %xmm6                # 16-byte Reload
	movaps	-64(%rbp), %xmm7                # 16-byte Reload
	movaps	-48(%rbp), %xmm8                # 16-byte Reload
	movaps	-32(%rbp), %xmm9                # 16-byte Reload
	movaps	-16(%rbp), %xmm10               # 16-byte Reload
	movaps	(%rbp), %xmm11                  # 16-byte Reload
	movaps	16(%rbp), %xmm12                # 16-byte Reload
	movaps	32(%rbp), %xmm13                # 16-byte Reload
	movaps	48(%rbp), %xmm14                # 16-byte Reload
	movaps	64(%rbp), %xmm15                # 16-byte Reload
	.loc	1 136 5 epilogue_begin is_stmt 0 # test_context.c:136:5
	addq	$208, %rsp
	popq	%rdi
	popq	%rsi
	popq	%rbp
	retq
.Ltmp24:
.Lfunc_end6:
	.seh_endproc
                                        # -- End function
	.section	.rdata,"dr"
.L.str:                                 # @.str
	.asciz	"print pos at %s:%d\n"

.L.str.1:                               # @.str.1
	.asciz	"D:/ZetaDevelop/ZetaExe/test_context.c"

.L.str.2:                               # @.str.2
	.asciz	"&main_thread = %llu\n"

	.bss
	.globl	main_thread                     # @main_thread
	.p2align	3, 0x0
main_thread:
	.zero	8

	.section	.rdata,"dr"
.L.str.3:                               # @.str.3
	.asciz	"main_thread.stack = %llu\n"

.L.str.4:                               # @.str.4
	.asciz	"func2\n"

	.bss
	.globl	func2_ret                       # @func2_ret
	.p2align	2, 0x0
func2_ret:
	.long	0                               # 0x0

	.globl	func2_thread                    # @func2_thread
	.p2align	3, 0x0
func2_thread:
	.zero	8

	.section	.rdata,"dr"
.L.str.5:                               # @.str.5
	.asciz	"func3\n"

.L.str.6:                               # @.str.6
	.asciz	"rax\t%lld\t%llu\n"

.L.str.7:                               # @.str.7
	.asciz	"rbx\t%lld\t%llu\n"

.L.str.8:                               # @.str.8
	.asciz	"rcx\t%lld\t%llu\n"

.L.str.9:                               # @.str.9
	.asciz	"rdx\t%lld\t%llu\n"

.L.str.10:                              # @.str.10
	.asciz	"rsi\t%lld\t%llu\n"

.L.str.11:                              # @.str.11
	.asciz	"rdi\t%lld\t%llu\n"

.L.str.12:                              # @.str.12
	.asciz	"rbp\t%lld\t%llu\n"

.L.str.13:                              # @.str.13
	.asciz	"rsp\t%lld\t%llu\n"

.L.str.14:                              # @.str.14
	.asciz	"r8\t%lld\t%llu\n"

.L.str.15:                              # @.str.15
	.asciz	"r9\t%lld\t%llu\n"

.L.str.16:                              # @.str.16
	.asciz	"r10\t%lld\t%llu\n"

.L.str.17:                              # @.str.17
	.asciz	"r11\t%lld\t%llu\n"

.L.str.18:                              # @.str.18
	.asciz	"r12\t%lld\t%llu\n"

.L.str.19:                              # @.str.19
	.asciz	"r13\t%lld\t%llu\n"

.L.str.20:                              # @.str.20
	.asciz	"r14\t%lld\t%llu\n"

.L.str.21:                              # @.str.21
	.asciz	"r15\t%lld\t%llu\n"

.L.str.22:                              # @.str.22
	.asciz	"eflags\t%lld\t%llu\n"

.L.str.23:                              # @.str.23
	.asciz	"rip\t%lld\t%llu\n"

	.bss
	.globl	func1_thread                    # @func1_thread
	.p2align	3, 0x0
func1_thread:
	.zero	8

	.section	.rdata,"dr"
.L.str.24:                              # @.str.24
	.asciz	"print var at %s:%d\n    %s = %d\n\n"

.L.str.25:                              # @.str.25
	.asciz	"func2_ret"

	.bss
	.globl	RIP_SAVE                        # @RIP_SAVE
	.p2align	3, 0x0
RIP_SAVE:
	.quad	0                               # 0x0

	.file	4 "C:/Program Files/llvm-mingw-20231003-msvcrt-x86_64/lib/clang/17/include" "stddef.h"
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
	.byte	52                              # DW_TAG_variable
	.byte	0                               # DW_CHILDREN_no
	.byte	3                               # DW_AT_name
	.byte	14                              # DW_FORM_strp
	.byte	73                              # DW_AT_type
	.byte	19                              # DW_FORM_ref4
	.byte	63                              # DW_AT_external
	.byte	25                              # DW_FORM_flag_present
	.byte	58                              # DW_AT_decl_file
	.byte	11                              # DW_FORM_data1
	.byte	59                              # DW_AT_decl_line
	.byte	11                              # DW_FORM_data1
	.byte	2                               # DW_AT_location
	.byte	24                              # DW_FORM_exprloc
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	8                               # Abbreviation Code
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
	.byte	9                               # Abbreviation Code
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
	.byte	10                              # Abbreviation Code
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
	.byte	11                              # Abbreviation Code
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
	.byte	39                              # DW_AT_prototyped
	.byte	25                              # DW_FORM_flag_present
	.byte	63                              # DW_AT_external
	.byte	25                              # DW_FORM_flag_present
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	12                              # Abbreviation Code
	.byte	5                               # DW_TAG_formal_parameter
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
	.byte	13                              # Abbreviation Code
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
	.byte	14                              # Abbreviation Code
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
	.byte	63                              # DW_AT_external
	.byte	25                              # DW_FORM_flag_present
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	15                              # Abbreviation Code
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
	.byte	16                              # Abbreviation Code
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
	.byte	17                              # Abbreviation Code
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
	.byte	18                              # Abbreviation Code
	.byte	24                              # DW_TAG_unspecified_parameters
	.byte	0                               # DW_CHILDREN_no
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	19                              # Abbreviation Code
	.byte	11                              # DW_TAG_lexical_block
	.byte	1                               # DW_CHILDREN_yes
	.byte	17                              # DW_AT_low_pc
	.byte	1                               # DW_FORM_addr
	.byte	18                              # DW_AT_high_pc
	.byte	6                               # DW_FORM_data4
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	20                              # Abbreviation Code
	.byte	46                              # DW_TAG_subprogram
	.byte	0                               # DW_CHILDREN_no
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
	.byte	63                              # DW_AT_external
	.byte	25                              # DW_FORM_flag_present
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	21                              # Abbreviation Code
	.byte	46                              # DW_TAG_subprogram
	.byte	0                               # DW_CHILDREN_no
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
	.byte	22                              # Abbreviation Code
	.byte	15                              # DW_TAG_pointer_type
	.byte	0                               # DW_CHILDREN_no
	.byte	73                              # DW_AT_type
	.byte	19                              # DW_FORM_ref4
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	23                              # Abbreviation Code
	.byte	21                              # DW_TAG_subroutine_type
	.byte	1                               # DW_CHILDREN_yes
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	24                              # Abbreviation Code
	.byte	38                              # DW_TAG_const_type
	.byte	0                               # DW_CHILDREN_no
	.byte	73                              # DW_AT_type
	.byte	19                              # DW_FORM_ref4
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	25                              # Abbreviation Code
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
	.byte	1                               # Abbrev [1] 0xb:0x570 DW_TAG_compile_unit
	.secrel32	.Linfo_string0          # DW_AT_producer
	.short	29                              # DW_AT_language
	.secrel32	.Linfo_string1          # DW_AT_name
	.secrel32	.Lline_table_start0     # DW_AT_stmt_list
	.secrel32	.Linfo_string2          # DW_AT_comp_dir
	.quad	.Lfunc_begin0                   # DW_AT_low_pc
	.long	.Lfunc_end6-.Lfunc_begin0       # DW_AT_high_pc
	.byte	2                               # Abbrev [2] 0x2a:0x11 DW_TAG_variable
	.long	59                              # DW_AT_type
	.byte	1                               # DW_AT_decl_file
	.byte	68                              # DW_AT_decl_line
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
	.byte	68                              # DW_AT_decl_line
	.byte	9                               # DW_AT_location
	.byte	3
	.quad	.L.str.1
	.byte	3                               # Abbrev [3] 0x66:0xc DW_TAG_array_type
	.long	71                              # DW_AT_type
	.byte	4                               # Abbrev [4] 0x6b:0x6 DW_TAG_subrange_type
	.long	78                              # DW_AT_type
	.byte	38                              # DW_AT_count
	.byte	0                               # End Of Children Mark
	.byte	2                               # Abbrev [2] 0x72:0x11 DW_TAG_variable
	.long	131                             # DW_AT_type
	.byte	1                               # DW_AT_decl_file
	.byte	69                              # DW_AT_decl_line
	.byte	9                               # DW_AT_location
	.byte	3
	.quad	.L.str.2
	.byte	3                               # Abbrev [3] 0x83:0xc DW_TAG_array_type
	.long	71                              # DW_AT_type
	.byte	4                               # Abbrev [4] 0x88:0x6 DW_TAG_subrange_type
	.long	78                              # DW_AT_type
	.byte	21                              # DW_AT_count
	.byte	0                               # End Of Children Mark
	.byte	2                               # Abbrev [2] 0x8f:0x11 DW_TAG_variable
	.long	160                             # DW_AT_type
	.byte	1                               # DW_AT_decl_file
	.byte	70                              # DW_AT_decl_line
	.byte	9                               # DW_AT_location
	.byte	3
	.quad	.L.str.3
	.byte	3                               # Abbrev [3] 0xa0:0xc DW_TAG_array_type
	.long	71                              # DW_AT_type
	.byte	4                               # Abbrev [4] 0xa5:0x6 DW_TAG_subrange_type
	.long	78                              # DW_AT_type
	.byte	26                              # DW_AT_count
	.byte	0                               # End Of Children Mark
	.byte	2                               # Abbrev [2] 0xac:0x11 DW_TAG_variable
	.long	189                             # DW_AT_type
	.byte	1                               # DW_AT_decl_file
	.byte	81                              # DW_AT_decl_line
	.byte	9                               # DW_AT_location
	.byte	3
	.quad	.L.str.4
	.byte	3                               # Abbrev [3] 0xbd:0xc DW_TAG_array_type
	.long	71                              # DW_AT_type
	.byte	4                               # Abbrev [4] 0xc2:0x6 DW_TAG_subrange_type
	.long	78                              # DW_AT_type
	.byte	7                               # DW_AT_count
	.byte	0                               # End Of Children Mark
	.byte	2                               # Abbrev [2] 0xc9:0x11 DW_TAG_variable
	.long	189                             # DW_AT_type
	.byte	1                               # DW_AT_decl_file
	.byte	94                              # DW_AT_decl_line
	.byte	9                               # DW_AT_location
	.byte	3
	.quad	.L.str.5
	.byte	2                               # Abbrev [2] 0xda:0x11 DW_TAG_variable
	.long	235                             # DW_AT_type
	.byte	1                               # DW_AT_decl_file
	.byte	97                              # DW_AT_decl_line
	.byte	9                               # DW_AT_location
	.byte	3
	.quad	.L.str.6
	.byte	3                               # Abbrev [3] 0xeb:0xc DW_TAG_array_type
	.long	71                              # DW_AT_type
	.byte	4                               # Abbrev [4] 0xf0:0x6 DW_TAG_subrange_type
	.long	78                              # DW_AT_type
	.byte	15                              # DW_AT_count
	.byte	0                               # End Of Children Mark
	.byte	2                               # Abbrev [2] 0xf7:0x11 DW_TAG_variable
	.long	235                             # DW_AT_type
	.byte	1                               # DW_AT_decl_file
	.byte	98                              # DW_AT_decl_line
	.byte	9                               # DW_AT_location
	.byte	3
	.quad	.L.str.7
	.byte	2                               # Abbrev [2] 0x108:0x11 DW_TAG_variable
	.long	235                             # DW_AT_type
	.byte	1                               # DW_AT_decl_file
	.byte	99                              # DW_AT_decl_line
	.byte	9                               # DW_AT_location
	.byte	3
	.quad	.L.str.8
	.byte	2                               # Abbrev [2] 0x119:0x11 DW_TAG_variable
	.long	235                             # DW_AT_type
	.byte	1                               # DW_AT_decl_file
	.byte	100                             # DW_AT_decl_line
	.byte	9                               # DW_AT_location
	.byte	3
	.quad	.L.str.9
	.byte	2                               # Abbrev [2] 0x12a:0x11 DW_TAG_variable
	.long	235                             # DW_AT_type
	.byte	1                               # DW_AT_decl_file
	.byte	102                             # DW_AT_decl_line
	.byte	9                               # DW_AT_location
	.byte	3
	.quad	.L.str.10
	.byte	2                               # Abbrev [2] 0x13b:0x11 DW_TAG_variable
	.long	235                             # DW_AT_type
	.byte	1                               # DW_AT_decl_file
	.byte	103                             # DW_AT_decl_line
	.byte	9                               # DW_AT_location
	.byte	3
	.quad	.L.str.11
	.byte	2                               # Abbrev [2] 0x14c:0x11 DW_TAG_variable
	.long	235                             # DW_AT_type
	.byte	1                               # DW_AT_decl_file
	.byte	105                             # DW_AT_decl_line
	.byte	9                               # DW_AT_location
	.byte	3
	.quad	.L.str.12
	.byte	2                               # Abbrev [2] 0x15d:0x11 DW_TAG_variable
	.long	235                             # DW_AT_type
	.byte	1                               # DW_AT_decl_file
	.byte	106                             # DW_AT_decl_line
	.byte	9                               # DW_AT_location
	.byte	3
	.quad	.L.str.13
	.byte	2                               # Abbrev [2] 0x16e:0x11 DW_TAG_variable
	.long	383                             # DW_AT_type
	.byte	1                               # DW_AT_decl_file
	.byte	108                             # DW_AT_decl_line
	.byte	9                               # DW_AT_location
	.byte	3
	.quad	.L.str.14
	.byte	3                               # Abbrev [3] 0x17f:0xc DW_TAG_array_type
	.long	71                              # DW_AT_type
	.byte	4                               # Abbrev [4] 0x184:0x6 DW_TAG_subrange_type
	.long	78                              # DW_AT_type
	.byte	14                              # DW_AT_count
	.byte	0                               # End Of Children Mark
	.byte	2                               # Abbrev [2] 0x18b:0x11 DW_TAG_variable
	.long	383                             # DW_AT_type
	.byte	1                               # DW_AT_decl_file
	.byte	109                             # DW_AT_decl_line
	.byte	9                               # DW_AT_location
	.byte	3
	.quad	.L.str.15
	.byte	2                               # Abbrev [2] 0x19c:0x11 DW_TAG_variable
	.long	235                             # DW_AT_type
	.byte	1                               # DW_AT_decl_file
	.byte	110                             # DW_AT_decl_line
	.byte	9                               # DW_AT_location
	.byte	3
	.quad	.L.str.16
	.byte	2                               # Abbrev [2] 0x1ad:0x11 DW_TAG_variable
	.long	235                             # DW_AT_type
	.byte	1                               # DW_AT_decl_file
	.byte	111                             # DW_AT_decl_line
	.byte	9                               # DW_AT_location
	.byte	3
	.quad	.L.str.17
	.byte	2                               # Abbrev [2] 0x1be:0x11 DW_TAG_variable
	.long	235                             # DW_AT_type
	.byte	1                               # DW_AT_decl_file
	.byte	113                             # DW_AT_decl_line
	.byte	9                               # DW_AT_location
	.byte	3
	.quad	.L.str.18
	.byte	2                               # Abbrev [2] 0x1cf:0x11 DW_TAG_variable
	.long	235                             # DW_AT_type
	.byte	1                               # DW_AT_decl_file
	.byte	114                             # DW_AT_decl_line
	.byte	9                               # DW_AT_location
	.byte	3
	.quad	.L.str.19
	.byte	2                               # Abbrev [2] 0x1e0:0x11 DW_TAG_variable
	.long	235                             # DW_AT_type
	.byte	1                               # DW_AT_decl_file
	.byte	115                             # DW_AT_decl_line
	.byte	9                               # DW_AT_location
	.byte	3
	.quad	.L.str.20
	.byte	2                               # Abbrev [2] 0x1f1:0x11 DW_TAG_variable
	.long	235                             # DW_AT_type
	.byte	1                               # DW_AT_decl_file
	.byte	116                             # DW_AT_decl_line
	.byte	9                               # DW_AT_location
	.byte	3
	.quad	.L.str.21
	.byte	2                               # Abbrev [2] 0x202:0x11 DW_TAG_variable
	.long	531                             # DW_AT_type
	.byte	1                               # DW_AT_decl_file
	.byte	118                             # DW_AT_decl_line
	.byte	9                               # DW_AT_location
	.byte	3
	.quad	.L.str.22
	.byte	3                               # Abbrev [3] 0x213:0xc DW_TAG_array_type
	.long	71                              # DW_AT_type
	.byte	4                               # Abbrev [4] 0x218:0x6 DW_TAG_subrange_type
	.long	78                              # DW_AT_type
	.byte	18                              # DW_AT_count
	.byte	0                               # End Of Children Mark
	.byte	2                               # Abbrev [2] 0x21f:0x11 DW_TAG_variable
	.long	235                             # DW_AT_type
	.byte	1                               # DW_AT_decl_file
	.byte	120                             # DW_AT_decl_line
	.byte	9                               # DW_AT_location
	.byte	3
	.quad	.L.str.23
	.byte	2                               # Abbrev [2] 0x230:0x11 DW_TAG_variable
	.long	577                             # DW_AT_type
	.byte	1                               # DW_AT_decl_file
	.byte	133                             # DW_AT_decl_line
	.byte	9                               # DW_AT_location
	.byte	3
	.quad	.L.str.24
	.byte	3                               # Abbrev [3] 0x241:0xc DW_TAG_array_type
	.long	71                              # DW_AT_type
	.byte	4                               # Abbrev [4] 0x246:0x6 DW_TAG_subrange_type
	.long	78                              # DW_AT_type
	.byte	33                              # DW_AT_count
	.byte	0                               # End Of Children Mark
	.byte	2                               # Abbrev [2] 0x24d:0x11 DW_TAG_variable
	.long	606                             # DW_AT_type
	.byte	1                               # DW_AT_decl_file
	.byte	133                             # DW_AT_decl_line
	.byte	9                               # DW_AT_location
	.byte	3
	.quad	.L.str.25
	.byte	3                               # Abbrev [3] 0x25e:0xc DW_TAG_array_type
	.long	71                              # DW_AT_type
	.byte	4                               # Abbrev [4] 0x263:0x6 DW_TAG_subrange_type
	.long	78                              # DW_AT_type
	.byte	10                              # DW_AT_count
	.byte	0                               # End Of Children Mark
	.byte	7                               # Abbrev [7] 0x26a:0x15 DW_TAG_variable
	.secrel32	.Linfo_string5          # DW_AT_name
	.long	639                             # DW_AT_type
                                        # DW_AT_external
	.byte	1                               # DW_AT_decl_file
	.byte	30                              # DW_AT_decl_line
	.byte	9                               # DW_AT_location
	.byte	3
	.quad	RIP_SAVE
	.byte	5                               # Abbrev [5] 0x27f:0x7 DW_TAG_base_type
	.secrel32	.Linfo_string6          # DW_AT_name
	.byte	7                               # DW_AT_encoding
	.byte	8                               # DW_AT_byte_size
	.byte	7                               # Abbrev [7] 0x286:0x15 DW_TAG_variable
	.secrel32	.Linfo_string7          # DW_AT_name
	.long	667                             # DW_AT_type
                                        # DW_AT_external
	.byte	1                               # DW_AT_decl_file
	.byte	42                              # DW_AT_decl_line
	.byte	9                               # DW_AT_location
	.byte	3
	.quad	main_thread
	.byte	8                               # Abbrev [8] 0x29b:0xb DW_TAG_typedef
	.long	678                             # DW_AT_type
	.secrel32	.Linfo_string10         # DW_AT_name
	.byte	1                               # DW_AT_decl_file
	.byte	36                              # DW_AT_decl_line
	.byte	9                               # Abbrev [9] 0x2a6:0x15 DW_TAG_structure_type
	.secrel32	.Linfo_string10         # DW_AT_name
	.byte	8                               # DW_AT_byte_size
	.byte	1                               # DW_AT_decl_file
	.byte	36                              # DW_AT_decl_line
	.byte	10                              # Abbrev [10] 0x2ae:0xc DW_TAG_member
	.secrel32	.Linfo_string8          # DW_AT_name
	.long	699                             # DW_AT_type
	.byte	1                               # DW_AT_decl_file
	.byte	36                              # DW_AT_decl_line
	.byte	0                               # DW_AT_data_member_location
	.byte	0                               # End Of Children Mark
	.byte	8                               # Abbrev [8] 0x2bb:0xb DW_TAG_typedef
	.long	639                             # DW_AT_type
	.secrel32	.Linfo_string9          # DW_AT_name
	.byte	1                               # DW_AT_decl_file
	.byte	34                              # DW_AT_decl_line
	.byte	7                               # Abbrev [7] 0x2c6:0x15 DW_TAG_variable
	.secrel32	.Linfo_string11         # DW_AT_name
	.long	667                             # DW_AT_type
                                        # DW_AT_external
	.byte	1                               # DW_AT_decl_file
	.byte	43                              # DW_AT_decl_line
	.byte	9                               # DW_AT_location
	.byte	3
	.quad	func1_thread
	.byte	7                               # Abbrev [7] 0x2db:0x15 DW_TAG_variable
	.secrel32	.Linfo_string12         # DW_AT_name
	.long	667                             # DW_AT_type
                                        # DW_AT_external
	.byte	1                               # DW_AT_decl_file
	.byte	44                              # DW_AT_decl_line
	.byte	9                               # DW_AT_location
	.byte	3
	.quad	func2_thread
	.byte	7                               # Abbrev [7] 0x2f0:0x15 DW_TAG_variable
	.secrel32	.Linfo_string13         # DW_AT_name
	.long	773                             # DW_AT_type
                                        # DW_AT_external
	.byte	1                               # DW_AT_decl_file
	.byte	78                              # DW_AT_decl_line
	.byte	9                               # DW_AT_location
	.byte	3
	.quad	func2_ret
	.byte	5                               # Abbrev [5] 0x305:0x7 DW_TAG_base_type
	.secrel32	.Linfo_string14         # DW_AT_name
	.byte	5                               # DW_AT_encoding
	.byte	4                               # DW_AT_byte_size
	.byte	8                               # Abbrev [8] 0x30c:0xb DW_TAG_typedef
	.long	791                             # DW_AT_type
	.secrel32	.Linfo_string16         # DW_AT_name
	.byte	2                               # DW_AT_decl_file
	.byte	62                              # DW_AT_decl_line
	.byte	5                               # Abbrev [5] 0x317:0x7 DW_TAG_base_type
	.secrel32	.Linfo_string15         # DW_AT_name
	.byte	5                               # DW_AT_encoding
	.byte	8                               # DW_AT_byte_size
	.byte	11                              # Abbrev [11] 0x31e:0x40 DW_TAG_subprogram
	.quad	.Lfunc_begin0                   # DW_AT_low_pc
	.long	.Lfunc_end0-.Lfunc_begin0       # DW_AT_high_pc
	.byte	1                               # DW_AT_frame_base
	.byte	87
	.secrel32	.Linfo_string17         # DW_AT_name
	.byte	1                               # DW_AT_decl_file
	.byte	54                              # DW_AT_decl_line
                                        # DW_AT_prototyped
                                        # DW_AT_external
	.byte	12                              # Abbrev [12] 0x333:0xe DW_TAG_formal_parameter
	.byte	2                               # DW_AT_location
	.byte	145
	.byte	56
	.secrel32	.Linfo_string24         # DW_AT_name
	.byte	1                               # DW_AT_decl_file
	.byte	54                              # DW_AT_decl_line
	.long	1103                            # DW_AT_type
	.byte	12                              # Abbrev [12] 0x341:0xe DW_TAG_formal_parameter
	.byte	2                               # DW_AT_location
	.byte	145
	.byte	48
	.secrel32	.Linfo_string25         # DW_AT_name
	.byte	1                               # DW_AT_decl_file
	.byte	54                              # DW_AT_decl_line
	.long	1108                            # DW_AT_type
	.byte	13                              # Abbrev [13] 0x34f:0xe DW_TAG_variable
	.byte	2                               # DW_AT_location
	.byte	145
	.byte	40
	.secrel32	.Linfo_string26         # DW_AT_name
	.byte	1                               # DW_AT_decl_file
	.byte	55                              # DW_AT_decl_line
	.long	1116                            # DW_AT_type
	.byte	0                               # End Of Children Mark
	.byte	14                              # Abbrev [14] 0x35e:0x24 DW_TAG_subprogram
	.quad	.Lfunc_begin1                   # DW_AT_low_pc
	.long	.Lfunc_end1-.Lfunc_begin1       # DW_AT_high_pc
	.byte	1                               # DW_AT_frame_base
	.byte	87
	.secrel32	.Linfo_string18         # DW_AT_name
	.byte	1                               # DW_AT_decl_file
	.byte	67                              # DW_AT_decl_line
                                        # DW_AT_external
	.byte	13                              # Abbrev [13] 0x373:0xe DW_TAG_variable
	.byte	2                               # DW_AT_location
	.byte	145
	.byte	32
	.secrel32	.Linfo_string8          # DW_AT_name
	.byte	1                               # DW_AT_decl_file
	.byte	71                              # DW_AT_decl_line
	.long	699                             # DW_AT_type
	.byte	0                               # End Of Children Mark
	.byte	15                              # Abbrev [15] 0x382:0x49 DW_TAG_subprogram
	.quad	.Lfunc_begin2                   # DW_AT_low_pc
	.long	.Lfunc_end2-.Lfunc_begin2       # DW_AT_high_pc
	.byte	1                               # DW_AT_frame_base
	.byte	87
	.secrel32	.Linfo_string19         # DW_AT_name
	.byte	3                               # DW_AT_decl_file
	.short	379                             # DW_AT_decl_line
                                        # DW_AT_prototyped
	.long	773                             # DW_AT_type
	.byte	16                              # Abbrev [16] 0x39c:0xf DW_TAG_formal_parameter
	.byte	2                               # DW_AT_location
	.byte	145
	.byte	48
	.secrel32	.Linfo_string28         # DW_AT_name
	.byte	3                               # DW_AT_decl_file
	.short	379                             # DW_AT_decl_line
	.long	1132                            # DW_AT_type
	.byte	17                              # Abbrev [17] 0x3ab:0xf DW_TAG_variable
	.byte	2                               # DW_AT_location
	.byte	145
	.byte	44
	.secrel32	.Linfo_string29         # DW_AT_name
	.byte	3                               # DW_AT_decl_file
	.short	381                             # DW_AT_decl_line
	.long	773                             # DW_AT_type
	.byte	17                              # Abbrev [17] 0x3ba:0xf DW_TAG_variable
	.byte	2                               # DW_AT_location
	.byte	145
	.byte	32
	.secrel32	.Linfo_string30         # DW_AT_name
	.byte	3                               # DW_AT_decl_file
	.short	382                             # DW_AT_decl_line
	.long	1142                            # DW_AT_type
	.byte	18                              # Abbrev [18] 0x3c9:0x1 DW_TAG_unspecified_parameters
	.byte	0                               # End Of Children Mark
	.byte	14                              # Abbrev [14] 0x3cb:0x32 DW_TAG_subprogram
	.quad	.Lfunc_begin3                   # DW_AT_low_pc
	.long	.Lfunc_end3-.Lfunc_begin3       # DW_AT_high_pc
	.byte	1                               # DW_AT_frame_base
	.byte	87
	.secrel32	.Linfo_string20         # DW_AT_name
	.byte	1                               # DW_AT_decl_file
	.byte	80                              # DW_AT_decl_line
                                        # DW_AT_external
	.byte	19                              # Abbrev [19] 0x3e0:0x1c DW_TAG_lexical_block
	.quad	.Ltmp6                          # DW_AT_low_pc
	.long	.Ltmp11-.Ltmp6                  # DW_AT_high_pc
	.byte	13                              # Abbrev [13] 0x3ed:0xe DW_TAG_variable
	.byte	2                               # DW_AT_location
	.byte	145
	.byte	44
	.secrel32	.Linfo_string32         # DW_AT_name
	.byte	1                               # DW_AT_decl_file
	.byte	83                              # DW_AT_decl_line
	.long	773                             # DW_AT_type
	.byte	0                               # End Of Children Mark
	.byte	0                               # End Of Children Mark
	.byte	20                              # Abbrev [20] 0x3fd:0x15 DW_TAG_subprogram
	.quad	.Lfunc_begin4                   # DW_AT_low_pc
	.long	.Lfunc_end4-.Lfunc_begin4       # DW_AT_high_pc
	.byte	1                               # DW_AT_frame_base
	.byte	87
	.secrel32	.Linfo_string21         # DW_AT_name
	.byte	1                               # DW_AT_decl_file
	.byte	94                              # DW_AT_decl_line
                                        # DW_AT_external
	.byte	11                              # Abbrev [11] 0x412:0x24 DW_TAG_subprogram
	.quad	.Lfunc_begin5                   # DW_AT_low_pc
	.long	.Lfunc_end5-.Lfunc_begin5       # DW_AT_high_pc
	.byte	1                               # DW_AT_frame_base
	.byte	87
	.secrel32	.Linfo_string22         # DW_AT_name
	.byte	1                               # DW_AT_decl_file
	.byte	96                              # DW_AT_decl_line
                                        # DW_AT_prototyped
                                        # DW_AT_external
	.byte	12                              # Abbrev [12] 0x427:0xe DW_TAG_formal_parameter
	.byte	2                               # DW_AT_location
	.byte	145
	.byte	32
	.secrel32	.Linfo_string33         # DW_AT_name
	.byte	1                               # DW_AT_decl_file
	.byte	96                              # DW_AT_decl_line
	.long	1156                            # DW_AT_type
	.byte	0                               # End Of Children Mark
	.byte	21                              # Abbrev [21] 0x436:0x19 DW_TAG_subprogram
	.quad	.Lfunc_begin6                   # DW_AT_low_pc
	.long	.Lfunc_end6-.Lfunc_begin6       # DW_AT_high_pc
	.byte	1                               # DW_AT_frame_base
	.byte	86
	.secrel32	.Linfo_string23         # DW_AT_name
	.byte	1                               # DW_AT_decl_file
	.byte	123                             # DW_AT_decl_line
	.long	773                             # DW_AT_type
                                        # DW_AT_external
	.byte	22                              # Abbrev [22] 0x44f:0x5 DW_TAG_pointer_type
	.long	667                             # DW_AT_type
	.byte	22                              # Abbrev [22] 0x454:0x5 DW_TAG_pointer_type
	.long	1113                            # DW_AT_type
	.byte	23                              # Abbrev [23] 0x459:0x3 DW_TAG_subroutine_type
	.byte	18                              # Abbrev [18] 0x45a:0x1 DW_TAG_unspecified_parameters
	.byte	0                               # End Of Children Mark
	.byte	24                              # Abbrev [24] 0x45c:0x5 DW_TAG_const_type
	.long	1121                            # DW_AT_type
	.byte	8                               # Abbrev [8] 0x461:0xb DW_TAG_typedef
	.long	639                             # DW_AT_type
	.secrel32	.Linfo_string27         # DW_AT_name
	.byte	4                               # DW_AT_decl_file
	.byte	46                              # DW_AT_decl_line
	.byte	22                              # Abbrev [22] 0x46c:0x5 DW_TAG_pointer_type
	.long	1137                            # DW_AT_type
	.byte	24                              # Abbrev [24] 0x471:0x5 DW_TAG_const_type
	.long	71                              # DW_AT_type
	.byte	25                              # Abbrev [25] 0x476:0x9 DW_TAG_typedef
	.long	1151                            # DW_AT_type
	.secrel32	.Linfo_string31         # DW_AT_name
	.byte	22                              # Abbrev [22] 0x47f:0x5 DW_TAG_pointer_type
	.long	71                              # DW_AT_type
	.byte	22                              # Abbrev [22] 0x484:0x5 DW_TAG_pointer_type
	.long	1161                            # DW_AT_type
	.byte	24                              # Abbrev [24] 0x489:0x5 DW_TAG_const_type
	.long	1166                            # DW_AT_type
	.byte	8                               # Abbrev [8] 0x48e:0xb DW_TAG_typedef
	.long	1177                            # DW_AT_type
	.secrel32	.Linfo_string52         # DW_AT_name
	.byte	1                               # DW_AT_decl_file
	.byte	3                               # DW_AT_decl_line
	.byte	9                               # Abbrev [9] 0x499:0xe1 DW_TAG_structure_type
	.secrel32	.Linfo_string52         # DW_AT_name
	.byte	144                             # DW_AT_byte_size
	.byte	1                               # DW_AT_decl_file
	.byte	3                               # DW_AT_decl_line
	.byte	10                              # Abbrev [10] 0x4a1:0xc DW_TAG_member
	.secrel32	.Linfo_string34         # DW_AT_name
	.long	639                             # DW_AT_type
	.byte	1                               # DW_AT_decl_file
	.byte	4                               # DW_AT_decl_line
	.byte	0                               # DW_AT_data_member_location
	.byte	10                              # Abbrev [10] 0x4ad:0xc DW_TAG_member
	.secrel32	.Linfo_string35         # DW_AT_name
	.long	639                             # DW_AT_type
	.byte	1                               # DW_AT_decl_file
	.byte	5                               # DW_AT_decl_line
	.byte	8                               # DW_AT_data_member_location
	.byte	10                              # Abbrev [10] 0x4b9:0xc DW_TAG_member
	.secrel32	.Linfo_string36         # DW_AT_name
	.long	639                             # DW_AT_type
	.byte	1                               # DW_AT_decl_file
	.byte	6                               # DW_AT_decl_line
	.byte	16                              # DW_AT_data_member_location
	.byte	10                              # Abbrev [10] 0x4c5:0xc DW_TAG_member
	.secrel32	.Linfo_string37         # DW_AT_name
	.long	639                             # DW_AT_type
	.byte	1                               # DW_AT_decl_file
	.byte	7                               # DW_AT_decl_line
	.byte	24                              # DW_AT_data_member_location
	.byte	10                              # Abbrev [10] 0x4d1:0xc DW_TAG_member
	.secrel32	.Linfo_string38         # DW_AT_name
	.long	639                             # DW_AT_type
	.byte	1                               # DW_AT_decl_file
	.byte	9                               # DW_AT_decl_line
	.byte	32                              # DW_AT_data_member_location
	.byte	10                              # Abbrev [10] 0x4dd:0xc DW_TAG_member
	.secrel32	.Linfo_string39         # DW_AT_name
	.long	639                             # DW_AT_type
	.byte	1                               # DW_AT_decl_file
	.byte	10                              # DW_AT_decl_line
	.byte	40                              # DW_AT_data_member_location
	.byte	10                              # Abbrev [10] 0x4e9:0xc DW_TAG_member
	.secrel32	.Linfo_string40         # DW_AT_name
	.long	639                             # DW_AT_type
	.byte	1                               # DW_AT_decl_file
	.byte	12                              # DW_AT_decl_line
	.byte	48                              # DW_AT_data_member_location
	.byte	10                              # Abbrev [10] 0x4f5:0xc DW_TAG_member
	.secrel32	.Linfo_string41         # DW_AT_name
	.long	639                             # DW_AT_type
	.byte	1                               # DW_AT_decl_file
	.byte	13                              # DW_AT_decl_line
	.byte	56                              # DW_AT_data_member_location
	.byte	10                              # Abbrev [10] 0x501:0xc DW_TAG_member
	.secrel32	.Linfo_string42         # DW_AT_name
	.long	639                             # DW_AT_type
	.byte	1                               # DW_AT_decl_file
	.byte	15                              # DW_AT_decl_line
	.byte	64                              # DW_AT_data_member_location
	.byte	10                              # Abbrev [10] 0x50d:0xc DW_TAG_member
	.secrel32	.Linfo_string43         # DW_AT_name
	.long	639                             # DW_AT_type
	.byte	1                               # DW_AT_decl_file
	.byte	16                              # DW_AT_decl_line
	.byte	72                              # DW_AT_data_member_location
	.byte	10                              # Abbrev [10] 0x519:0xc DW_TAG_member
	.secrel32	.Linfo_string44         # DW_AT_name
	.long	639                             # DW_AT_type
	.byte	1                               # DW_AT_decl_file
	.byte	17                              # DW_AT_decl_line
	.byte	80                              # DW_AT_data_member_location
	.byte	10                              # Abbrev [10] 0x525:0xc DW_TAG_member
	.secrel32	.Linfo_string45         # DW_AT_name
	.long	639                             # DW_AT_type
	.byte	1                               # DW_AT_decl_file
	.byte	18                              # DW_AT_decl_line
	.byte	88                              # DW_AT_data_member_location
	.byte	10                              # Abbrev [10] 0x531:0xc DW_TAG_member
	.secrel32	.Linfo_string46         # DW_AT_name
	.long	639                             # DW_AT_type
	.byte	1                               # DW_AT_decl_file
	.byte	20                              # DW_AT_decl_line
	.byte	96                              # DW_AT_data_member_location
	.byte	10                              # Abbrev [10] 0x53d:0xc DW_TAG_member
	.secrel32	.Linfo_string47         # DW_AT_name
	.long	639                             # DW_AT_type
	.byte	1                               # DW_AT_decl_file
	.byte	21                              # DW_AT_decl_line
	.byte	104                             # DW_AT_data_member_location
	.byte	10                              # Abbrev [10] 0x549:0xc DW_TAG_member
	.secrel32	.Linfo_string48         # DW_AT_name
	.long	639                             # DW_AT_type
	.byte	1                               # DW_AT_decl_file
	.byte	22                              # DW_AT_decl_line
	.byte	112                             # DW_AT_data_member_location
	.byte	10                              # Abbrev [10] 0x555:0xc DW_TAG_member
	.secrel32	.Linfo_string49         # DW_AT_name
	.long	639                             # DW_AT_type
	.byte	1                               # DW_AT_decl_file
	.byte	23                              # DW_AT_decl_line
	.byte	120                             # DW_AT_data_member_location
	.byte	10                              # Abbrev [10] 0x561:0xc DW_TAG_member
	.secrel32	.Linfo_string50         # DW_AT_name
	.long	639                             # DW_AT_type
	.byte	1                               # DW_AT_decl_file
	.byte	25                              # DW_AT_decl_line
	.byte	128                             # DW_AT_data_member_location
	.byte	10                              # Abbrev [10] 0x56d:0xc DW_TAG_member
	.secrel32	.Linfo_string51         # DW_AT_name
	.long	639                             # DW_AT_type
	.byte	1                               # DW_AT_decl_file
	.byte	27                              # DW_AT_decl_line
	.byte	136                             # DW_AT_data_member_location
	.byte	0                               # End Of Children Mark
	.byte	0                               # End Of Children Mark
.Ldebug_info_end0:
	.section	.debug_str,"dr"
.Linfo_string:
.Linfo_string0:
	.asciz	"clang version 17.0.2 (https://github.com/llvm/llvm-project.git b2417f51dbbd7435eb3aaf203de24de6754da50e)" # string offset=0
.Linfo_string1:
	.asciz	"D:/ZetaDevelop/ZetaExe/test_context.c" # string offset=105
.Linfo_string2:
	.asciz	"D:/ZetaDevelop/ZetaExe"        # string offset=143
.Linfo_string3:
	.asciz	"char"                          # string offset=166
.Linfo_string4:
	.asciz	"__ARRAY_SIZE_TYPE__"           # string offset=171
.Linfo_string5:
	.asciz	"RIP_SAVE"                      # string offset=191
.Linfo_string6:
	.asciz	"unsigned long long"            # string offset=200
.Linfo_string7:
	.asciz	"main_thread"                   # string offset=219
.Linfo_string8:
	.asciz	"stack"                         # string offset=231
.Linfo_string9:
	.asciz	"RX"                            # string offset=237
.Linfo_string10:
	.asciz	"Thread"                        # string offset=240
.Linfo_string11:
	.asciz	"func1_thread"                  # string offset=247
.Linfo_string12:
	.asciz	"func2_thread"                  # string offset=260
.Linfo_string13:
	.asciz	"func2_ret"                     # string offset=273
.Linfo_string14:
	.asciz	"int"                           # string offset=283
.Linfo_string15:
	.asciz	"long long"                     # string offset=287
.Linfo_string16:
	.asciz	"intptr_t"                      # string offset=297
.Linfo_string17:
	.asciz	"Thread_Create"                 # string offset=306
.Linfo_string18:
	.asciz	"Func1"                         # string offset=320
.Linfo_string19:
	.asciz	"printf"                        # string offset=326
.Linfo_string20:
	.asciz	"Func2"                         # string offset=333
.Linfo_string21:
	.asciz	"Func3"                         # string offset=339
.Linfo_string22:
	.asciz	"PrintPCB"                      # string offset=345
.Linfo_string23:
	.asciz	"main"                          # string offset=354
.Linfo_string24:
	.asciz	"thread"                        # string offset=359
.Linfo_string25:
	.asciz	"func"                          # string offset=366
.Linfo_string26:
	.asciz	"stack_size"                    # string offset=371
.Linfo_string27:
	.asciz	"size_t"                        # string offset=382
.Linfo_string28:
	.asciz	"__format"                      # string offset=389
.Linfo_string29:
	.asciz	"__retval"                      # string offset=398
.Linfo_string30:
	.asciz	"__local_argv"                  # string offset=407
.Linfo_string31:
	.asciz	"__builtin_va_list"             # string offset=420
.Linfo_string32:
	.asciz	"i"                             # string offset=438
.Linfo_string33:
	.asciz	"pcb"                           # string offset=440
.Linfo_string34:
	.asciz	"rax"                           # string offset=444
.Linfo_string35:
	.asciz	"rbx"                           # string offset=448
.Linfo_string36:
	.asciz	"rcx"                           # string offset=452
.Linfo_string37:
	.asciz	"rdx"                           # string offset=456
.Linfo_string38:
	.asciz	"rsi"                           # string offset=460
.Linfo_string39:
	.asciz	"rdi"                           # string offset=464
.Linfo_string40:
	.asciz	"rbp"                           # string offset=468
.Linfo_string41:
	.asciz	"rsp"                           # string offset=472
.Linfo_string42:
	.asciz	"r8"                            # string offset=476
.Linfo_string43:
	.asciz	"r9"                            # string offset=479
.Linfo_string44:
	.asciz	"r10"                           # string offset=482
.Linfo_string45:
	.asciz	"r11"                           # string offset=486
.Linfo_string46:
	.asciz	"r12"                           # string offset=490
.Linfo_string47:
	.asciz	"r13"                           # string offset=494
.Linfo_string48:
	.asciz	"r14"                           # string offset=498
.Linfo_string49:
	.asciz	"r15"                           # string offset=502
.Linfo_string50:
	.asciz	"eflags"                        # string offset=506
.Linfo_string51:
	.asciz	"rip"                           # string offset=513
.Linfo_string52:
	.asciz	"PCB"                           # string offset=517
	.addrsig
	.addrsig_sym Thread_Create
	.addrsig_sym Thread_Create_
	.addrsig_sym malloc
	.addrsig_sym Func1
	.addrsig_sym printf
	.addrsig_sym exit
	.addrsig_sym Func2
	.addrsig_sym Thread_Switch
	.addrsig_sym __mingw_vfprintf
	.addrsig_sym main_thread
	.addrsig_sym func2_ret
	.addrsig_sym func2_thread
	.addrsig_sym func1_thread
	.section	.debug_line,"dr"
.Lsection_line:
.Lline_table_start0:
