; ModuleID = 'D:/ZetaDevelop/Zeta/Debugger.c'
source_filename = "D:/ZetaDevelop/Zeta/Debugger.c"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc19.38.33133"

@zeta_debugger_assert_stage = dso_local local_unnamed_addr global i8 0, align 1
@zeta_debugger_assert_callback_context = dso_local local_unnamed_addr global ptr null, align 8
@zeta_debugger_assert_callback = dso_local local_unnamed_addr global ptr null, align 8

!llvm.module.flags = !{!0, !1, !2, !3}
!llvm.ident = !{!4}

!0 = !{i32 1, !"wchar_size", i32 2}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"uwtable", i32 2}
!3 = !{i32 1, !"MaxTLSAlign", i32 65536}
!4 = !{!"clang version 18.1.4"}
