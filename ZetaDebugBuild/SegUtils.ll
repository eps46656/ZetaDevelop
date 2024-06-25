; ModuleID = 'D:/ZetaDevelop/Zeta/SegUtils.c'
source_filename = "D:/ZetaDevelop/Zeta/SegUtils.c"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc19.38.33133"

%struct.Zeta_CircularVector_Cursor = type { ptr, i64, ptr }
%struct.Zeta_CircularVector = type { ptr, i64, i64, i64, i64, i64 }

$sprintf = comdat any

$vsprintf = comdat any

$_snprintf = comdat any

$_vsnprintf = comdat any

$printf = comdat any

$_vsprintf_l = comdat any

$_vsnprintf_l = comdat any

$__local_stdio_printf_options = comdat any

$_vfprintf_l = comdat any

$"??_C@_09DDPAKPII@SegShoveL?$AA@" = comdat any

$"??_C@_0BP@DOHEAJCL@D?3?1ZetaDevelop?1Zeta?1SegUtils?4c?$AA@" = comdat any

$"??_C@_0BC@CENDOPBP@?$CF48s?3?$CF?94llu?7?$CF?924s?$AA@" = comdat any

$"??_C@_0BD@LBCLNPNL@l_cv?5?$CB?$DN?5?$CI?$CIvoid?$CK?$CJ0?$CJ?$AA@" = comdat any

$"??_C@_0M@BJGCKIIG@?7assert?7?$CFs?6?$AA@" = comdat any

$"??_C@_0BD@DGBEPCEH@r_cv?5?$CB?$DN?5?$CI?$CIvoid?$CK?$CJ0?$CJ?$AA@" = comdat any

$"??_C@_0CJ@EGMKBOBE@l_cv?9?$DOsize?5?$CL?5shove_cnt?5?$DM?$DN?5l_cv?9?$DO@" = comdat any

$"??_C@_0CC@OIMDNCHI@shove_cnt?5?$DM?$DN?5r_cv?9?$DOsize?5?$CL?5ins_cn@" = comdat any

$"??_C@_0BF@LNJADDKF@width?5?$DN?$DN?5r_cv?9?$DOwidth?$AA@" = comdat any

$"??_C@_09OHLBJAFH@SegShoveR?$AA@" = comdat any

$"??_C@_0CJ@CMKBNDAF@r_cv?9?$DOsize?5?$CL?5shove_cnt?5?$DM?$DN?5l_cv?9?$DO@" = comdat any

$"??_C@_0CC@HAOIMMCE@shove_cnt?5?$DM?$DN?5l_cv?9?$DOsize?5?$CL?5ins_cn@" = comdat any

@"??_C@_09DDPAKPII@SegShoveL?$AA@" = linkonce_odr dso_local unnamed_addr constant [10 x i8] c"SegShoveL\00", comdat, align 1, !dbg !0
@"??_C@_0BP@DOHEAJCL@D?3?1ZetaDevelop?1Zeta?1SegUtils?4c?$AA@" = linkonce_odr dso_local unnamed_addr constant [31 x i8] c"D:/ZetaDevelop/Zeta/SegUtils.c\00", comdat, align 1, !dbg !8
@"??_C@_0BC@CENDOPBP@?$CF48s?3?$CF?94llu?7?$CF?924s?$AA@" = linkonce_odr dso_local unnamed_addr constant [18 x i8] c"%48s:%-4llu\09%-24s\00", comdat, align 1, !dbg !13
@"??_C@_0BD@LBCLNPNL@l_cv?5?$CB?$DN?5?$CI?$CIvoid?$CK?$CJ0?$CJ?$AA@" = linkonce_odr dso_local unnamed_addr constant [19 x i8] c"l_cv != ((void*)0)\00", comdat, align 1, !dbg !18
@"??_C@_0M@BJGCKIIG@?7assert?7?$CFs?6?$AA@" = linkonce_odr dso_local unnamed_addr constant [12 x i8] c"\09assert\09%s\0A\00", comdat, align 1, !dbg !23
@zeta_debugger_assert_stage = external dso_local global i8, align 1
@zeta_debugger_assert_callback = external dso_local global ptr, align 8
@zeta_debugger_assert_callback_context = external dso_local global ptr, align 8
@"??_C@_0BD@DGBEPCEH@r_cv?5?$CB?$DN?5?$CI?$CIvoid?$CK?$CJ0?$CJ?$AA@" = linkonce_odr dso_local unnamed_addr constant [19 x i8] c"r_cv != ((void*)0)\00", comdat, align 1, !dbg !28
@"??_C@_0CJ@EGMKBOBE@l_cv?9?$DOsize?5?$CL?5shove_cnt?5?$DM?$DN?5l_cv?9?$DO@" = linkonce_odr dso_local unnamed_addr constant [41 x i8] c"l_cv->size + shove_cnt <= l_cv->capacity\00", comdat, align 1, !dbg !30
@"??_C@_0CC@OIMDNCHI@shove_cnt?5?$DM?$DN?5r_cv?9?$DOsize?5?$CL?5ins_cn@" = linkonce_odr dso_local unnamed_addr constant [34 x i8] c"shove_cnt <= r_cv->size + ins_cnt\00", comdat, align 1, !dbg !35
@"??_C@_0BF@LNJADDKF@width?5?$DN?$DN?5r_cv?9?$DOwidth?$AA@" = linkonce_odr dso_local unnamed_addr constant [21 x i8] c"width == r_cv->width\00", comdat, align 1, !dbg !40
@"??_C@_09OHLBJAFH@SegShoveR?$AA@" = linkonce_odr dso_local unnamed_addr constant [10 x i8] c"SegShoveR\00", comdat, align 1, !dbg !45
@"??_C@_0CJ@CMKBNDAF@r_cv?9?$DOsize?5?$CL?5shove_cnt?5?$DM?$DN?5l_cv?9?$DO@" = linkonce_odr dso_local unnamed_addr constant [41 x i8] c"r_cv->size + shove_cnt <= l_cv->capacity\00", comdat, align 1, !dbg !47
@"??_C@_0CC@HAOIMMCE@shove_cnt?5?$DM?$DN?5l_cv?9?$DOsize?5?$CL?5ins_cn@" = linkonce_odr dso_local unnamed_addr constant [34 x i8] c"shove_cnt <= l_cv->size + ins_cnt\00", comdat, align 1, !dbg !49
@__local_stdio_printf_options._OptionsStorage = internal global i64 0, align 8, !dbg !51

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr dso_local i32 @sprintf(ptr noundef %0, ptr noundef %1, ...) #0 comdat !dbg !73 {
  %3 = alloca ptr, align 8
  %4 = alloca ptr, align 8
  %5 = alloca i32, align 4
  %6 = alloca ptr, align 8
  store ptr %1, ptr %3, align 8
  call void @llvm.dbg.declare(metadata ptr %3, metadata !83, metadata !DIExpression()), !dbg !84
  store ptr %0, ptr %4, align 8
  call void @llvm.dbg.declare(metadata ptr %4, metadata !85, metadata !DIExpression()), !dbg !86
  call void @llvm.dbg.declare(metadata ptr %5, metadata !87, metadata !DIExpression()), !dbg !88
  call void @llvm.dbg.declare(metadata ptr %6, metadata !89, metadata !DIExpression()), !dbg !92
  call void @llvm.va_start(ptr %6), !dbg !93
  %7 = load ptr, ptr %6, align 8, !dbg !94
  %8 = load ptr, ptr %3, align 8, !dbg !94
  %9 = load ptr, ptr %4, align 8, !dbg !94
  %10 = call i32 @_vsprintf_l(ptr noundef %9, ptr noundef %8, ptr noundef null, ptr noundef %7), !dbg !94
  store i32 %10, ptr %5, align 4, !dbg !94
  call void @llvm.va_end(ptr %6), !dbg !95
  %11 = load i32, ptr %5, align 4, !dbg !96
  ret i32 %11, !dbg !96
}

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr dso_local i32 @vsprintf(ptr noundef %0, ptr noundef %1, ptr noundef %2) #0 comdat !dbg !97 {
  %4 = alloca ptr, align 8
  %5 = alloca ptr, align 8
  %6 = alloca ptr, align 8
  store ptr %2, ptr %4, align 8
  call void @llvm.dbg.declare(metadata ptr %4, metadata !100, metadata !DIExpression()), !dbg !101
  store ptr %1, ptr %5, align 8
  call void @llvm.dbg.declare(metadata ptr %5, metadata !102, metadata !DIExpression()), !dbg !103
  store ptr %0, ptr %6, align 8
  call void @llvm.dbg.declare(metadata ptr %6, metadata !104, metadata !DIExpression()), !dbg !105
  %7 = load ptr, ptr %4, align 8, !dbg !106
  %8 = load ptr, ptr %5, align 8, !dbg !106
  %9 = load ptr, ptr %6, align 8, !dbg !106
  %10 = call i32 @_vsnprintf_l(ptr noundef %9, i64 noundef -1, ptr noundef %8, ptr noundef null, ptr noundef %7), !dbg !106
  ret i32 %10, !dbg !106
}

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr dso_local i32 @_snprintf(ptr noundef %0, i64 noundef %1, ptr noundef %2, ...) #0 comdat !dbg !107 {
  %4 = alloca ptr, align 8
  %5 = alloca i64, align 8
  %6 = alloca ptr, align 8
  %7 = alloca i32, align 4
  %8 = alloca ptr, align 8
  store ptr %2, ptr %4, align 8
  call void @llvm.dbg.declare(metadata ptr %4, metadata !111, metadata !DIExpression()), !dbg !112
  store i64 %1, ptr %5, align 8
  call void @llvm.dbg.declare(metadata ptr %5, metadata !113, metadata !DIExpression()), !dbg !114
  store ptr %0, ptr %6, align 8
  call void @llvm.dbg.declare(metadata ptr %6, metadata !115, metadata !DIExpression()), !dbg !116
  call void @llvm.dbg.declare(metadata ptr %7, metadata !117, metadata !DIExpression()), !dbg !118
  call void @llvm.dbg.declare(metadata ptr %8, metadata !119, metadata !DIExpression()), !dbg !120
  call void @llvm.va_start(ptr %8), !dbg !121
  %9 = load ptr, ptr %8, align 8, !dbg !122
  %10 = load ptr, ptr %4, align 8, !dbg !122
  %11 = load i64, ptr %5, align 8, !dbg !122
  %12 = load ptr, ptr %6, align 8, !dbg !122
  %13 = call i32 @_vsnprintf(ptr noundef %12, i64 noundef %11, ptr noundef %10, ptr noundef %9), !dbg !122
  store i32 %13, ptr %7, align 4, !dbg !122
  call void @llvm.va_end(ptr %8), !dbg !123
  %14 = load i32, ptr %7, align 4, !dbg !124
  ret i32 %14, !dbg !124
}

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr dso_local i32 @_vsnprintf(ptr noundef %0, i64 noundef %1, ptr noundef %2, ptr noundef %3) #0 comdat !dbg !125 {
  %5 = alloca ptr, align 8
  %6 = alloca ptr, align 8
  %7 = alloca i64, align 8
  %8 = alloca ptr, align 8
  store ptr %3, ptr %5, align 8
  call void @llvm.dbg.declare(metadata ptr %5, metadata !128, metadata !DIExpression()), !dbg !129
  store ptr %2, ptr %6, align 8
  call void @llvm.dbg.declare(metadata ptr %6, metadata !130, metadata !DIExpression()), !dbg !131
  store i64 %1, ptr %7, align 8
  call void @llvm.dbg.declare(metadata ptr %7, metadata !132, metadata !DIExpression()), !dbg !133
  store ptr %0, ptr %8, align 8
  call void @llvm.dbg.declare(metadata ptr %8, metadata !134, metadata !DIExpression()), !dbg !135
  %9 = load ptr, ptr %5, align 8, !dbg !136
  %10 = load ptr, ptr %6, align 8, !dbg !136
  %11 = load i64, ptr %7, align 8, !dbg !136
  %12 = load ptr, ptr %8, align 8, !dbg !136
  %13 = call i32 @_vsnprintf_l(ptr noundef %12, i64 noundef %11, ptr noundef %10, ptr noundef null, ptr noundef %9), !dbg !136
  ret i32 %13, !dbg !136
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @SegShoveL(ptr noundef %0, ptr noundef %1, i64 noundef %2, i64 noundef %3, i64 noundef %4) #0 !dbg !137 {
  %6 = alloca i64, align 8
  %7 = alloca i64, align 8
  %8 = alloca i64, align 8
  %9 = alloca ptr, align 8
  %10 = alloca ptr, align 8
  %11 = alloca i64, align 8
  %12 = alloca i64, align 8
  %13 = alloca i64, align 8
  %14 = alloca i64, align 8
  %15 = alloca i64, align 8
  %16 = alloca i64, align 8
  %17 = alloca i64, align 8
  %18 = alloca i64, align 8
  %19 = alloca i64, align 8
  %20 = alloca i64, align 8
  %21 = alloca i64, align 8
  %22 = alloca i64, align 8
  %23 = alloca %struct.Zeta_CircularVector_Cursor, align 8
  %24 = alloca %struct.Zeta_CircularVector_Cursor, align 8
  %25 = alloca i64, align 8
  %26 = alloca i64, align 8
  store i64 %4, ptr %6, align 8
  call void @llvm.dbg.declare(metadata ptr %6, metadata !151, metadata !DIExpression()), !dbg !152
  store i64 %3, ptr %7, align 8
  call void @llvm.dbg.declare(metadata ptr %7, metadata !153, metadata !DIExpression()), !dbg !152
  store i64 %2, ptr %8, align 8
  call void @llvm.dbg.declare(metadata ptr %8, metadata !154, metadata !DIExpression()), !dbg !152
  store ptr %1, ptr %9, align 8
  call void @llvm.dbg.declare(metadata ptr %9, metadata !155, metadata !DIExpression()), !dbg !156
  store ptr %0, ptr %10, align 8
  call void @llvm.dbg.declare(metadata ptr %10, metadata !157, metadata !DIExpression()), !dbg !156
  %27 = load ptr, ptr %10, align 8, !dbg !158
  %28 = icmp ne ptr %27, null, !dbg !158
  br i1 %28, label %29, label %30, !dbg !158

29:                                               ; preds = %5
  br label %46, !dbg !159

30:                                               ; preds = %5
  %31 = call i32 (ptr, ...) @printf(ptr noundef @"??_C@_0BC@CENDOPBP@?$CF48s?3?$CF?94llu?7?$CF?924s?$AA@", ptr noundef @"??_C@_0BP@DOHEAJCL@D?3?1ZetaDevelop?1Zeta?1SegUtils?4c?$AA@", i64 noundef 8, ptr noundef @"??_C@_09DDPAKPII@SegShoveL?$AA@"), !dbg !162
  %32 = call i32 (ptr, ...) @printf(ptr noundef @"??_C@_0M@BJGCKIIG@?7assert?7?$CFs?6?$AA@", ptr noundef @"??_C@_0BD@LBCLNPNL@l_cv?5?$CB?$DN?5?$CI?$CIvoid?$CK?$CJ0?$CJ?$AA@"), !dbg !162
  %33 = call ptr @__acrt_iob_func(i32 noundef 1), !dbg !164
  %34 = call i32 @fflush(ptr noundef %33), !dbg !164
  %35 = load i8, ptr @zeta_debugger_assert_stage, align 1, !dbg !162
  %36 = trunc i8 %35 to i1, !dbg !162
  br i1 %36, label %45, label %37, !dbg !162

37:                                               ; preds = %30
  store i8 1, ptr @zeta_debugger_assert_stage, align 1, !dbg !167
  %38 = load ptr, ptr @zeta_debugger_assert_callback, align 8, !dbg !167
  %39 = icmp ne ptr %38, null, !dbg !167
  br i1 %39, label %40, label %44, !dbg !167

40:                                               ; preds = %37
  %41 = load ptr, ptr @zeta_debugger_assert_callback, align 8, !dbg !170
  %42 = load ptr, ptr @zeta_debugger_assert_callback_context, align 8, !dbg !170
  %43 = call i32 %41(ptr noundef %42), !dbg !170
  br label %44, !dbg !170

44:                                               ; preds = %40, %37
  br label %45, !dbg !167

45:                                               ; preds = %44, %30
  call void @exit(i32 noundef 0) #5, !dbg !162
  unreachable, !dbg !162

46:                                               ; preds = %29
  %47 = load ptr, ptr %9, align 8, !dbg !173
  %48 = icmp ne ptr %47, null, !dbg !173
  br i1 %48, label %49, label %50, !dbg !173

49:                                               ; preds = %46
  br label %66, !dbg !174

50:                                               ; preds = %46
  %51 = call i32 (ptr, ...) @printf(ptr noundef @"??_C@_0BC@CENDOPBP@?$CF48s?3?$CF?94llu?7?$CF?924s?$AA@", ptr noundef @"??_C@_0BP@DOHEAJCL@D?3?1ZetaDevelop?1Zeta?1SegUtils?4c?$AA@", i64 noundef 9, ptr noundef @"??_C@_09DDPAKPII@SegShoveL?$AA@"), !dbg !177
  %52 = call i32 (ptr, ...) @printf(ptr noundef @"??_C@_0M@BJGCKIIG@?7assert?7?$CFs?6?$AA@", ptr noundef @"??_C@_0BD@DGBEPCEH@r_cv?5?$CB?$DN?5?$CI?$CIvoid?$CK?$CJ0?$CJ?$AA@"), !dbg !177
  %53 = call ptr @__acrt_iob_func(i32 noundef 1), !dbg !179
  %54 = call i32 @fflush(ptr noundef %53), !dbg !179
  %55 = load i8, ptr @zeta_debugger_assert_stage, align 1, !dbg !177
  %56 = trunc i8 %55 to i1, !dbg !177
  br i1 %56, label %65, label %57, !dbg !177

57:                                               ; preds = %50
  store i8 1, ptr @zeta_debugger_assert_stage, align 1, !dbg !182
  %58 = load ptr, ptr @zeta_debugger_assert_callback, align 8, !dbg !182
  %59 = icmp ne ptr %58, null, !dbg !182
  br i1 %59, label %60, label %64, !dbg !182

60:                                               ; preds = %57
  %61 = load ptr, ptr @zeta_debugger_assert_callback, align 8, !dbg !185
  %62 = load ptr, ptr @zeta_debugger_assert_callback_context, align 8, !dbg !185
  %63 = call i32 %61(ptr noundef %62), !dbg !185
  br label %64, !dbg !185

64:                                               ; preds = %60, %57
  br label %65, !dbg !182

65:                                               ; preds = %64, %50
  call void @exit(i32 noundef 0) #5, !dbg !177
  unreachable, !dbg !177

66:                                               ; preds = %49
  %67 = load ptr, ptr %10, align 8, !dbg !188
  %68 = getelementptr inbounds %struct.Zeta_CircularVector, ptr %67, i32 0, i32 4, !dbg !188
  %69 = load i64, ptr %68, align 8, !dbg !188
  %70 = load i64, ptr %6, align 8, !dbg !188
  %71 = add i64 %69, %70, !dbg !188
  %72 = load ptr, ptr %10, align 8, !dbg !188
  %73 = getelementptr inbounds %struct.Zeta_CircularVector, ptr %72, i32 0, i32 5, !dbg !188
  %74 = load i64, ptr %73, align 8, !dbg !188
  %75 = icmp ule i64 %71, %74, !dbg !188
  br i1 %75, label %76, label %77, !dbg !188

76:                                               ; preds = %66
  br label %93, !dbg !189

77:                                               ; preds = %66
  %78 = call i32 (ptr, ...) @printf(ptr noundef @"??_C@_0BC@CENDOPBP@?$CF48s?3?$CF?94llu?7?$CF?924s?$AA@", ptr noundef @"??_C@_0BP@DOHEAJCL@D?3?1ZetaDevelop?1Zeta?1SegUtils?4c?$AA@", i64 noundef 11, ptr noundef @"??_C@_09DDPAKPII@SegShoveL?$AA@"), !dbg !192
  %79 = call i32 (ptr, ...) @printf(ptr noundef @"??_C@_0M@BJGCKIIG@?7assert?7?$CFs?6?$AA@", ptr noundef @"??_C@_0CJ@EGMKBOBE@l_cv?9?$DOsize?5?$CL?5shove_cnt?5?$DM?$DN?5l_cv?9?$DO@"), !dbg !192
  %80 = call ptr @__acrt_iob_func(i32 noundef 1), !dbg !194
  %81 = call i32 @fflush(ptr noundef %80), !dbg !194
  %82 = load i8, ptr @zeta_debugger_assert_stage, align 1, !dbg !192
  %83 = trunc i8 %82 to i1, !dbg !192
  br i1 %83, label %92, label %84, !dbg !192

84:                                               ; preds = %77
  store i8 1, ptr @zeta_debugger_assert_stage, align 1, !dbg !197
  %85 = load ptr, ptr @zeta_debugger_assert_callback, align 8, !dbg !197
  %86 = icmp ne ptr %85, null, !dbg !197
  br i1 %86, label %87, label %91, !dbg !197

87:                                               ; preds = %84
  %88 = load ptr, ptr @zeta_debugger_assert_callback, align 8, !dbg !200
  %89 = load ptr, ptr @zeta_debugger_assert_callback_context, align 8, !dbg !200
  %90 = call i32 %88(ptr noundef %89), !dbg !200
  br label %91, !dbg !200

91:                                               ; preds = %87, %84
  br label %92, !dbg !197

92:                                               ; preds = %91, %77
  call void @exit(i32 noundef 0) #5, !dbg !192
  unreachable, !dbg !192

93:                                               ; preds = %76
  %94 = load i64, ptr %6, align 8, !dbg !203
  %95 = load ptr, ptr %9, align 8, !dbg !203
  %96 = getelementptr inbounds %struct.Zeta_CircularVector, ptr %95, i32 0, i32 4, !dbg !203
  %97 = load i64, ptr %96, align 8, !dbg !203
  %98 = load i64, ptr %7, align 8, !dbg !203
  %99 = add i64 %97, %98, !dbg !203
  %100 = icmp ule i64 %94, %99, !dbg !203
  br i1 %100, label %101, label %102, !dbg !203

101:                                              ; preds = %93
  br label %118, !dbg !204

102:                                              ; preds = %93
  %103 = call i32 (ptr, ...) @printf(ptr noundef @"??_C@_0BC@CENDOPBP@?$CF48s?3?$CF?94llu?7?$CF?924s?$AA@", ptr noundef @"??_C@_0BP@DOHEAJCL@D?3?1ZetaDevelop?1Zeta?1SegUtils?4c?$AA@", i64 noundef 12, ptr noundef @"??_C@_09DDPAKPII@SegShoveL?$AA@"), !dbg !207
  %104 = call i32 (ptr, ...) @printf(ptr noundef @"??_C@_0M@BJGCKIIG@?7assert?7?$CFs?6?$AA@", ptr noundef @"??_C@_0CC@OIMDNCHI@shove_cnt?5?$DM?$DN?5r_cv?9?$DOsize?5?$CL?5ins_cn@"), !dbg !207
  %105 = call ptr @__acrt_iob_func(i32 noundef 1), !dbg !209
  %106 = call i32 @fflush(ptr noundef %105), !dbg !209
  %107 = load i8, ptr @zeta_debugger_assert_stage, align 1, !dbg !207
  %108 = trunc i8 %107 to i1, !dbg !207
  br i1 %108, label %117, label %109, !dbg !207

109:                                              ; preds = %102
  store i8 1, ptr @zeta_debugger_assert_stage, align 1, !dbg !212
  %110 = load ptr, ptr @zeta_debugger_assert_callback, align 8, !dbg !212
  %111 = icmp ne ptr %110, null, !dbg !212
  br i1 %111, label %112, label %116, !dbg !212

112:                                              ; preds = %109
  %113 = load ptr, ptr @zeta_debugger_assert_callback, align 8, !dbg !215
  %114 = load ptr, ptr @zeta_debugger_assert_callback_context, align 8, !dbg !215
  %115 = call i32 %113(ptr noundef %114), !dbg !215
  br label %116, !dbg !215

116:                                              ; preds = %112, %109
  br label %117, !dbg !212

117:                                              ; preds = %116, %102
  call void @exit(i32 noundef 0) #5, !dbg !207
  unreachable, !dbg !207

118:                                              ; preds = %101
  call void @llvm.dbg.declare(metadata ptr %11, metadata !218, metadata !DIExpression()), !dbg !219
  %119 = load ptr, ptr %10, align 8, !dbg !219
  %120 = getelementptr inbounds %struct.Zeta_CircularVector, ptr %119, i32 0, i32 1, !dbg !219
  %121 = load i64, ptr %120, align 8, !dbg !219
  store i64 %121, ptr %11, align 8, !dbg !219
  %122 = load i64, ptr %11, align 8, !dbg !220
  %123 = load ptr, ptr %9, align 8, !dbg !220
  %124 = getelementptr inbounds %struct.Zeta_CircularVector, ptr %123, i32 0, i32 1, !dbg !220
  %125 = load i64, ptr %124, align 8, !dbg !220
  %126 = icmp eq i64 %122, %125, !dbg !220
  br i1 %126, label %127, label %128, !dbg !220

127:                                              ; preds = %118
  br label %144, !dbg !221

128:                                              ; preds = %118
  %129 = call i32 (ptr, ...) @printf(ptr noundef @"??_C@_0BC@CENDOPBP@?$CF48s?3?$CF?94llu?7?$CF?924s?$AA@", ptr noundef @"??_C@_0BP@DOHEAJCL@D?3?1ZetaDevelop?1Zeta?1SegUtils?4c?$AA@", i64 noundef 15, ptr noundef @"??_C@_09DDPAKPII@SegShoveL?$AA@"), !dbg !224
  %130 = call i32 (ptr, ...) @printf(ptr noundef @"??_C@_0M@BJGCKIIG@?7assert?7?$CFs?6?$AA@", ptr noundef @"??_C@_0BF@LNJADDKF@width?5?$DN?$DN?5r_cv?9?$DOwidth?$AA@"), !dbg !224
  %131 = call ptr @__acrt_iob_func(i32 noundef 1), !dbg !226
  %132 = call i32 @fflush(ptr noundef %131), !dbg !226
  %133 = load i8, ptr @zeta_debugger_assert_stage, align 1, !dbg !224
  %134 = trunc i8 %133 to i1, !dbg !224
  br i1 %134, label %143, label %135, !dbg !224

135:                                              ; preds = %128
  store i8 1, ptr @zeta_debugger_assert_stage, align 1, !dbg !229
  %136 = load ptr, ptr @zeta_debugger_assert_callback, align 8, !dbg !229
  %137 = icmp ne ptr %136, null, !dbg !229
  br i1 %137, label %138, label %142, !dbg !229

138:                                              ; preds = %135
  %139 = load ptr, ptr @zeta_debugger_assert_callback, align 8, !dbg !232
  %140 = load ptr, ptr @zeta_debugger_assert_callback_context, align 8, !dbg !232
  %141 = call i32 %139(ptr noundef %140), !dbg !232
  br label %142, !dbg !232

142:                                              ; preds = %138, %135
  br label %143, !dbg !229

143:                                              ; preds = %142, %128
  call void @exit(i32 noundef 0) #5, !dbg !224
  unreachable, !dbg !224

144:                                              ; preds = %127
  call void @llvm.dbg.declare(metadata ptr %12, metadata !235, metadata !DIExpression()), !dbg !236
  call void @llvm.dbg.declare(metadata ptr %13, metadata !237, metadata !DIExpression()), !dbg !239
  %145 = load i64, ptr %8, align 8, !dbg !239
  store i64 %145, ptr %13, align 8, !dbg !239
  call void @llvm.dbg.declare(metadata ptr %14, metadata !240, metadata !DIExpression()), !dbg !239
  %146 = load i64, ptr %6, align 8, !dbg !239
  store i64 %146, ptr %14, align 8, !dbg !239
  %147 = load i64, ptr %13, align 8, !dbg !239
  %148 = load i64, ptr %14, align 8, !dbg !239
  %149 = icmp ult i64 %147, %148, !dbg !239
  br i1 %149, label %150, label %152, !dbg !239

150:                                              ; preds = %144
  %151 = load i64, ptr %13, align 8, !dbg !239
  br label %154, !dbg !239

152:                                              ; preds = %144
  %153 = load i64, ptr %14, align 8, !dbg !239
  br label %154, !dbg !239

154:                                              ; preds = %152, %150
  %155 = phi i64 [ %151, %150 ], [ %153, %152 ], !dbg !239
  store i64 %155, ptr %15, align 8, !dbg !239
  %156 = load i64, ptr %15, align 8, !dbg !239
  store i64 %156, ptr %12, align 8, !dbg !236
  call void @llvm.dbg.declare(metadata ptr %16, metadata !241, metadata !DIExpression()), !dbg !242
  call void @llvm.dbg.declare(metadata ptr %17, metadata !243, metadata !DIExpression()), !dbg !245
  %157 = load i64, ptr %7, align 8, !dbg !245
  store i64 %157, ptr %17, align 8, !dbg !245
  call void @llvm.dbg.declare(metadata ptr %18, metadata !246, metadata !DIExpression()), !dbg !245
  %158 = load i64, ptr %6, align 8, !dbg !245
  %159 = load i64, ptr %12, align 8, !dbg !245
  %160 = sub i64 %158, %159, !dbg !245
  store i64 %160, ptr %18, align 8, !dbg !245
  %161 = load i64, ptr %17, align 8, !dbg !245
  %162 = load i64, ptr %18, align 8, !dbg !245
  %163 = icmp ult i64 %161, %162, !dbg !245
  br i1 %163, label %164, label %166, !dbg !245

164:                                              ; preds = %154
  %165 = load i64, ptr %17, align 8, !dbg !245
  br label %168, !dbg !245

166:                                              ; preds = %154
  %167 = load i64, ptr %18, align 8, !dbg !245
  br label %168, !dbg !245

168:                                              ; preds = %166, %164
  %169 = phi i64 [ %165, %164 ], [ %167, %166 ], !dbg !245
  store i64 %169, ptr %19, align 8, !dbg !245
  %170 = load i64, ptr %19, align 8, !dbg !245
  store i64 %170, ptr %16, align 8, !dbg !242
  call void @llvm.dbg.declare(metadata ptr %20, metadata !247, metadata !DIExpression()), !dbg !248
  %171 = load i64, ptr %6, align 8, !dbg !248
  %172 = load i64, ptr %12, align 8, !dbg !248
  %173 = sub i64 %171, %172, !dbg !248
  %174 = load i64, ptr %16, align 8, !dbg !248
  %175 = sub i64 %173, %174, !dbg !248
  store i64 %175, ptr %20, align 8, !dbg !248
  call void @llvm.dbg.declare(metadata ptr %21, metadata !249, metadata !DIExpression()), !dbg !250
  %176 = load ptr, ptr %10, align 8, !dbg !250
  %177 = getelementptr inbounds %struct.Zeta_CircularVector, ptr %176, i32 0, i32 4, !dbg !250
  %178 = load i64, ptr %177, align 8, !dbg !250
  store i64 %178, ptr %21, align 8, !dbg !250
  call void @llvm.dbg.declare(metadata ptr %22, metadata !251, metadata !DIExpression()), !dbg !252
  store i64 0, ptr %22, align 8, !dbg !252
  call void @llvm.dbg.declare(metadata ptr %23, metadata !253, metadata !DIExpression()), !dbg !260
  call void @llvm.dbg.declare(metadata ptr %24, metadata !261, metadata !DIExpression()), !dbg !262
  %179 = load ptr, ptr %10, align 8, !dbg !263
  call void @Zeta_CircularVector_GetRBCursor(ptr noundef %179, ptr noundef %23), !dbg !263
  %180 = load i64, ptr %6, align 8, !dbg !264
  %181 = load ptr, ptr %10, align 8, !dbg !264
  %182 = call ptr @Zeta_CircularVector_Insert(ptr noundef %181, ptr noundef %23, i64 noundef %180), !dbg !264
  call void @llvm.dbg.declare(metadata ptr %25, metadata !265, metadata !DIExpression()), !dbg !267
  %183 = load i64, ptr %12, align 8, !dbg !267
  store i64 %183, ptr %25, align 8, !dbg !267
  br label %184, !dbg !267

184:                                              ; preds = %196, %168
  %185 = load i64, ptr %25, align 8, !dbg !267
  %186 = add i64 %185, -1, !dbg !267
  store i64 %186, ptr %25, align 8, !dbg !267
  %187 = icmp ult i64 0, %185, !dbg !267
  br i1 %187, label %188, label %201, !dbg !267

188:                                              ; preds = %184
  %189 = load i64, ptr %11, align 8, !dbg !268
  %190 = load i64, ptr %22, align 8, !dbg !268
  %191 = load ptr, ptr %9, align 8, !dbg !268
  %192 = call ptr @Zeta_CircularVector_Access(ptr noundef %191, ptr noundef null, ptr noundef null, i64 noundef %190), !dbg !268
  %193 = load i64, ptr %21, align 8, !dbg !268
  %194 = load ptr, ptr %10, align 8, !dbg !268
  %195 = call ptr @Zeta_CircularVector_Access(ptr noundef %194, ptr noundef null, ptr noundef null, i64 noundef %193), !dbg !268
  call void @Zeta_MemCopy(ptr noundef %195, ptr noundef %192, i64 noundef %189), !dbg !268
  br label %196, !dbg !271

196:                                              ; preds = %188
  %197 = load i64, ptr %21, align 8, !dbg !272
  %198 = add i64 %197, 1, !dbg !272
  store i64 %198, ptr %21, align 8, !dbg !272
  %199 = load i64, ptr %22, align 8, !dbg !272
  %200 = add i64 %199, 1, !dbg !272
  store i64 %200, ptr %22, align 8, !dbg !272
  br label %184, !dbg !272, !llvm.loop !273

201:                                              ; preds = %184
  %202 = load i64, ptr %16, align 8, !dbg !276
  %203 = load i64, ptr %21, align 8, !dbg !276
  %204 = add i64 %203, %202, !dbg !276
  store i64 %204, ptr %21, align 8, !dbg !276
  call void @llvm.dbg.declare(metadata ptr %26, metadata !277, metadata !DIExpression()), !dbg !279
  %205 = load i64, ptr %20, align 8, !dbg !279
  store i64 %205, ptr %26, align 8, !dbg !279
  br label %206, !dbg !279

206:                                              ; preds = %218, %201
  %207 = load i64, ptr %26, align 8, !dbg !279
  %208 = add i64 %207, -1, !dbg !279
  store i64 %208, ptr %26, align 8, !dbg !279
  %209 = icmp ult i64 0, %207, !dbg !279
  br i1 %209, label %210, label %223, !dbg !279

210:                                              ; preds = %206
  %211 = load i64, ptr %11, align 8, !dbg !280
  %212 = load i64, ptr %22, align 8, !dbg !280
  %213 = load ptr, ptr %9, align 8, !dbg !280
  %214 = call ptr @Zeta_CircularVector_Access(ptr noundef %213, ptr noundef null, ptr noundef null, i64 noundef %212), !dbg !280
  %215 = load i64, ptr %21, align 8, !dbg !280
  %216 = load ptr, ptr %10, align 8, !dbg !280
  %217 = call ptr @Zeta_CircularVector_Access(ptr noundef %216, ptr noundef null, ptr noundef null, i64 noundef %215), !dbg !280
  call void @Zeta_MemCopy(ptr noundef %217, ptr noundef %214, i64 noundef %211), !dbg !280
  br label %218, !dbg !283

218:                                              ; preds = %210
  %219 = load i64, ptr %21, align 8, !dbg !284
  %220 = add i64 %219, 1, !dbg !284
  store i64 %220, ptr %21, align 8, !dbg !284
  %221 = load i64, ptr %22, align 8, !dbg !284
  %222 = add i64 %221, 1, !dbg !284
  store i64 %222, ptr %22, align 8, !dbg !284
  br label %206, !dbg !284, !llvm.loop !285

223:                                              ; preds = %206
  %224 = load ptr, ptr %9, align 8, !dbg !287
  %225 = call ptr @Zeta_CircularVector_PeekL(ptr noundef %224, ptr noundef %24, ptr noundef null), !dbg !287
  %226 = load i64, ptr %12, align 8, !dbg !288
  %227 = load i64, ptr %20, align 8, !dbg !288
  %228 = add i64 %226, %227, !dbg !288
  %229 = load ptr, ptr %9, align 8, !dbg !288
  call void @Zeta_CircularVector_Erase(ptr noundef %229, ptr noundef %24, i64 noundef %228), !dbg !288
  %230 = load i64, ptr %8, align 8, !dbg !289
  %231 = load i64, ptr %12, align 8, !dbg !289
  %232 = sub i64 %230, %231, !dbg !289
  %233 = load ptr, ptr %9, align 8, !dbg !289
  call void @Zeta_CircularVector_Cursor_AdvanceR(ptr noundef %233, ptr noundef %24, i64 noundef %232), !dbg !289
  %234 = load i64, ptr %7, align 8, !dbg !290
  %235 = load i64, ptr %16, align 8, !dbg !290
  %236 = sub i64 %234, %235, !dbg !290
  %237 = load ptr, ptr %9, align 8, !dbg !290
  %238 = call ptr @Zeta_CircularVector_Insert(ptr noundef %237, ptr noundef %24, i64 noundef %236), !dbg !290
  ret void, !dbg !291
}

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr dso_local i32 @printf(ptr noundef %0, ...) #0 comdat !dbg !292 {
  %2 = alloca ptr, align 8
  %3 = alloca i32, align 4
  %4 = alloca ptr, align 8
  store ptr %0, ptr %2, align 8
  call void @llvm.dbg.declare(metadata ptr %2, metadata !295, metadata !DIExpression()), !dbg !296
  call void @llvm.dbg.declare(metadata ptr %3, metadata !297, metadata !DIExpression()), !dbg !298
  call void @llvm.dbg.declare(metadata ptr %4, metadata !299, metadata !DIExpression()), !dbg !300
  call void @llvm.va_start(ptr %4), !dbg !301
  %5 = load ptr, ptr %4, align 8, !dbg !302
  %6 = load ptr, ptr %2, align 8, !dbg !302
  %7 = call ptr @__acrt_iob_func(i32 noundef 1), !dbg !302
  %8 = call i32 @_vfprintf_l(ptr noundef %7, ptr noundef %6, ptr noundef null, ptr noundef %5), !dbg !302
  store i32 %8, ptr %3, align 4, !dbg !302
  call void @llvm.va_end(ptr %4), !dbg !303
  %9 = load i32, ptr %3, align 4, !dbg !304
  ret i32 %9, !dbg !304
}

declare dso_local i32 @fflush(ptr noundef) #2

declare dso_local ptr @__acrt_iob_func(i32 noundef) #2

; Function Attrs: noreturn
declare dso_local void @exit(i32 noundef) #3

declare dso_local void @Zeta_CircularVector_GetRBCursor(ptr noundef, ptr noundef) #2

declare dso_local ptr @Zeta_CircularVector_Insert(ptr noundef, ptr noundef, i64 noundef) #2

declare dso_local void @Zeta_MemCopy(ptr noundef, ptr noundef, i64 noundef) #2

declare dso_local ptr @Zeta_CircularVector_Access(ptr noundef, ptr noundef, ptr noundef, i64 noundef) #2

declare dso_local ptr @Zeta_CircularVector_PeekL(ptr noundef, ptr noundef, ptr noundef) #2

declare dso_local void @Zeta_CircularVector_Erase(ptr noundef, ptr noundef, i64 noundef) #2

declare dso_local void @Zeta_CircularVector_Cursor_AdvanceR(ptr noundef, ptr noundef, i64 noundef) #2

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @SegShoveR(ptr noundef %0, ptr noundef %1, i64 noundef %2, i64 noundef %3, i64 noundef %4) #0 !dbg !305 {
  %6 = alloca i64, align 8
  %7 = alloca i64, align 8
  %8 = alloca i64, align 8
  %9 = alloca ptr, align 8
  %10 = alloca ptr, align 8
  %11 = alloca i64, align 8
  %12 = alloca i64, align 8
  %13 = alloca i64, align 8
  %14 = alloca i64, align 8
  %15 = alloca i64, align 8
  %16 = alloca i64, align 8
  %17 = alloca i64, align 8
  %18 = alloca i64, align 8
  %19 = alloca i64, align 8
  %20 = alloca i64, align 8
  %21 = alloca i64, align 8
  %22 = alloca i64, align 8
  %23 = alloca %struct.Zeta_CircularVector_Cursor, align 8
  %24 = alloca %struct.Zeta_CircularVector_Cursor, align 8
  %25 = alloca i64, align 8
  %26 = alloca i64, align 8
  store i64 %4, ptr %6, align 8
  call void @llvm.dbg.declare(metadata ptr %6, metadata !306, metadata !DIExpression()), !dbg !307
  store i64 %3, ptr %7, align 8
  call void @llvm.dbg.declare(metadata ptr %7, metadata !308, metadata !DIExpression()), !dbg !307
  store i64 %2, ptr %8, align 8
  call void @llvm.dbg.declare(metadata ptr %8, metadata !309, metadata !DIExpression()), !dbg !307
  store ptr %1, ptr %9, align 8
  call void @llvm.dbg.declare(metadata ptr %9, metadata !310, metadata !DIExpression()), !dbg !311
  store ptr %0, ptr %10, align 8
  call void @llvm.dbg.declare(metadata ptr %10, metadata !312, metadata !DIExpression()), !dbg !311
  %27 = load ptr, ptr %10, align 8, !dbg !313
  %28 = icmp ne ptr %27, null, !dbg !313
  br i1 %28, label %29, label %30, !dbg !313

29:                                               ; preds = %5
  br label %46, !dbg !314

30:                                               ; preds = %5
  %31 = call i32 (ptr, ...) @printf(ptr noundef @"??_C@_0BC@CENDOPBP@?$CF48s?3?$CF?94llu?7?$CF?924s?$AA@", ptr noundef @"??_C@_0BP@DOHEAJCL@D?3?1ZetaDevelop?1Zeta?1SegUtils?4c?$AA@", i64 noundef 54, ptr noundef @"??_C@_09OHLBJAFH@SegShoveR?$AA@"), !dbg !317
  %32 = call i32 (ptr, ...) @printf(ptr noundef @"??_C@_0M@BJGCKIIG@?7assert?7?$CFs?6?$AA@", ptr noundef @"??_C@_0BD@LBCLNPNL@l_cv?5?$CB?$DN?5?$CI?$CIvoid?$CK?$CJ0?$CJ?$AA@"), !dbg !317
  %33 = call ptr @__acrt_iob_func(i32 noundef 1), !dbg !319
  %34 = call i32 @fflush(ptr noundef %33), !dbg !319
  %35 = load i8, ptr @zeta_debugger_assert_stage, align 1, !dbg !317
  %36 = trunc i8 %35 to i1, !dbg !317
  br i1 %36, label %45, label %37, !dbg !317

37:                                               ; preds = %30
  store i8 1, ptr @zeta_debugger_assert_stage, align 1, !dbg !322
  %38 = load ptr, ptr @zeta_debugger_assert_callback, align 8, !dbg !322
  %39 = icmp ne ptr %38, null, !dbg !322
  br i1 %39, label %40, label %44, !dbg !322

40:                                               ; preds = %37
  %41 = load ptr, ptr @zeta_debugger_assert_callback, align 8, !dbg !325
  %42 = load ptr, ptr @zeta_debugger_assert_callback_context, align 8, !dbg !325
  %43 = call i32 %41(ptr noundef %42), !dbg !325
  br label %44, !dbg !325

44:                                               ; preds = %40, %37
  br label %45, !dbg !322

45:                                               ; preds = %44, %30
  call void @exit(i32 noundef 0) #5, !dbg !317
  unreachable, !dbg !317

46:                                               ; preds = %29
  %47 = load ptr, ptr %9, align 8, !dbg !328
  %48 = icmp ne ptr %47, null, !dbg !328
  br i1 %48, label %49, label %50, !dbg !328

49:                                               ; preds = %46
  br label %66, !dbg !329

50:                                               ; preds = %46
  %51 = call i32 (ptr, ...) @printf(ptr noundef @"??_C@_0BC@CENDOPBP@?$CF48s?3?$CF?94llu?7?$CF?924s?$AA@", ptr noundef @"??_C@_0BP@DOHEAJCL@D?3?1ZetaDevelop?1Zeta?1SegUtils?4c?$AA@", i64 noundef 55, ptr noundef @"??_C@_09OHLBJAFH@SegShoveR?$AA@"), !dbg !332
  %52 = call i32 (ptr, ...) @printf(ptr noundef @"??_C@_0M@BJGCKIIG@?7assert?7?$CFs?6?$AA@", ptr noundef @"??_C@_0BD@DGBEPCEH@r_cv?5?$CB?$DN?5?$CI?$CIvoid?$CK?$CJ0?$CJ?$AA@"), !dbg !332
  %53 = call ptr @__acrt_iob_func(i32 noundef 1), !dbg !334
  %54 = call i32 @fflush(ptr noundef %53), !dbg !334
  %55 = load i8, ptr @zeta_debugger_assert_stage, align 1, !dbg !332
  %56 = trunc i8 %55 to i1, !dbg !332
  br i1 %56, label %65, label %57, !dbg !332

57:                                               ; preds = %50
  store i8 1, ptr @zeta_debugger_assert_stage, align 1, !dbg !337
  %58 = load ptr, ptr @zeta_debugger_assert_callback, align 8, !dbg !337
  %59 = icmp ne ptr %58, null, !dbg !337
  br i1 %59, label %60, label %64, !dbg !337

60:                                               ; preds = %57
  %61 = load ptr, ptr @zeta_debugger_assert_callback, align 8, !dbg !340
  %62 = load ptr, ptr @zeta_debugger_assert_callback_context, align 8, !dbg !340
  %63 = call i32 %61(ptr noundef %62), !dbg !340
  br label %64, !dbg !340

64:                                               ; preds = %60, %57
  br label %65, !dbg !337

65:                                               ; preds = %64, %50
  call void @exit(i32 noundef 0) #5, !dbg !332
  unreachable, !dbg !332

66:                                               ; preds = %49
  %67 = load ptr, ptr %9, align 8, !dbg !343
  %68 = getelementptr inbounds %struct.Zeta_CircularVector, ptr %67, i32 0, i32 4, !dbg !343
  %69 = load i64, ptr %68, align 8, !dbg !343
  %70 = load i64, ptr %6, align 8, !dbg !343
  %71 = add i64 %69, %70, !dbg !343
  %72 = load ptr, ptr %10, align 8, !dbg !343
  %73 = getelementptr inbounds %struct.Zeta_CircularVector, ptr %72, i32 0, i32 5, !dbg !343
  %74 = load i64, ptr %73, align 8, !dbg !343
  %75 = icmp ule i64 %71, %74, !dbg !343
  br i1 %75, label %76, label %77, !dbg !343

76:                                               ; preds = %66
  br label %93, !dbg !344

77:                                               ; preds = %66
  %78 = call i32 (ptr, ...) @printf(ptr noundef @"??_C@_0BC@CENDOPBP@?$CF48s?3?$CF?94llu?7?$CF?924s?$AA@", ptr noundef @"??_C@_0BP@DOHEAJCL@D?3?1ZetaDevelop?1Zeta?1SegUtils?4c?$AA@", i64 noundef 57, ptr noundef @"??_C@_09OHLBJAFH@SegShoveR?$AA@"), !dbg !347
  %79 = call i32 (ptr, ...) @printf(ptr noundef @"??_C@_0M@BJGCKIIG@?7assert?7?$CFs?6?$AA@", ptr noundef @"??_C@_0CJ@CMKBNDAF@r_cv?9?$DOsize?5?$CL?5shove_cnt?5?$DM?$DN?5l_cv?9?$DO@"), !dbg !347
  %80 = call ptr @__acrt_iob_func(i32 noundef 1), !dbg !349
  %81 = call i32 @fflush(ptr noundef %80), !dbg !349
  %82 = load i8, ptr @zeta_debugger_assert_stage, align 1, !dbg !347
  %83 = trunc i8 %82 to i1, !dbg !347
  br i1 %83, label %92, label %84, !dbg !347

84:                                               ; preds = %77
  store i8 1, ptr @zeta_debugger_assert_stage, align 1, !dbg !352
  %85 = load ptr, ptr @zeta_debugger_assert_callback, align 8, !dbg !352
  %86 = icmp ne ptr %85, null, !dbg !352
  br i1 %86, label %87, label %91, !dbg !352

87:                                               ; preds = %84
  %88 = load ptr, ptr @zeta_debugger_assert_callback, align 8, !dbg !355
  %89 = load ptr, ptr @zeta_debugger_assert_callback_context, align 8, !dbg !355
  %90 = call i32 %88(ptr noundef %89), !dbg !355
  br label %91, !dbg !355

91:                                               ; preds = %87, %84
  br label %92, !dbg !352

92:                                               ; preds = %91, %77
  call void @exit(i32 noundef 0) #5, !dbg !347
  unreachable, !dbg !347

93:                                               ; preds = %76
  %94 = load i64, ptr %6, align 8, !dbg !358
  %95 = load ptr, ptr %10, align 8, !dbg !358
  %96 = getelementptr inbounds %struct.Zeta_CircularVector, ptr %95, i32 0, i32 4, !dbg !358
  %97 = load i64, ptr %96, align 8, !dbg !358
  %98 = load i64, ptr %7, align 8, !dbg !358
  %99 = add i64 %97, %98, !dbg !358
  %100 = icmp ule i64 %94, %99, !dbg !358
  br i1 %100, label %101, label %102, !dbg !358

101:                                              ; preds = %93
  br label %118, !dbg !359

102:                                              ; preds = %93
  %103 = call i32 (ptr, ...) @printf(ptr noundef @"??_C@_0BC@CENDOPBP@?$CF48s?3?$CF?94llu?7?$CF?924s?$AA@", ptr noundef @"??_C@_0BP@DOHEAJCL@D?3?1ZetaDevelop?1Zeta?1SegUtils?4c?$AA@", i64 noundef 58, ptr noundef @"??_C@_09OHLBJAFH@SegShoveR?$AA@"), !dbg !362
  %104 = call i32 (ptr, ...) @printf(ptr noundef @"??_C@_0M@BJGCKIIG@?7assert?7?$CFs?6?$AA@", ptr noundef @"??_C@_0CC@HAOIMMCE@shove_cnt?5?$DM?$DN?5l_cv?9?$DOsize?5?$CL?5ins_cn@"), !dbg !362
  %105 = call ptr @__acrt_iob_func(i32 noundef 1), !dbg !364
  %106 = call i32 @fflush(ptr noundef %105), !dbg !364
  %107 = load i8, ptr @zeta_debugger_assert_stage, align 1, !dbg !362
  %108 = trunc i8 %107 to i1, !dbg !362
  br i1 %108, label %117, label %109, !dbg !362

109:                                              ; preds = %102
  store i8 1, ptr @zeta_debugger_assert_stage, align 1, !dbg !367
  %110 = load ptr, ptr @zeta_debugger_assert_callback, align 8, !dbg !367
  %111 = icmp ne ptr %110, null, !dbg !367
  br i1 %111, label %112, label %116, !dbg !367

112:                                              ; preds = %109
  %113 = load ptr, ptr @zeta_debugger_assert_callback, align 8, !dbg !370
  %114 = load ptr, ptr @zeta_debugger_assert_callback_context, align 8, !dbg !370
  %115 = call i32 %113(ptr noundef %114), !dbg !370
  br label %116, !dbg !370

116:                                              ; preds = %112, %109
  br label %117, !dbg !367

117:                                              ; preds = %116, %102
  call void @exit(i32 noundef 0) #5, !dbg !362
  unreachable, !dbg !362

118:                                              ; preds = %101
  call void @llvm.dbg.declare(metadata ptr %11, metadata !373, metadata !DIExpression()), !dbg !374
  %119 = load ptr, ptr %10, align 8, !dbg !374
  %120 = getelementptr inbounds %struct.Zeta_CircularVector, ptr %119, i32 0, i32 1, !dbg !374
  %121 = load i64, ptr %120, align 8, !dbg !374
  store i64 %121, ptr %11, align 8, !dbg !374
  %122 = load i64, ptr %11, align 8, !dbg !375
  %123 = load ptr, ptr %9, align 8, !dbg !375
  %124 = getelementptr inbounds %struct.Zeta_CircularVector, ptr %123, i32 0, i32 1, !dbg !375
  %125 = load i64, ptr %124, align 8, !dbg !375
  %126 = icmp eq i64 %122, %125, !dbg !375
  br i1 %126, label %127, label %128, !dbg !375

127:                                              ; preds = %118
  br label %144, !dbg !376

128:                                              ; preds = %118
  %129 = call i32 (ptr, ...) @printf(ptr noundef @"??_C@_0BC@CENDOPBP@?$CF48s?3?$CF?94llu?7?$CF?924s?$AA@", ptr noundef @"??_C@_0BP@DOHEAJCL@D?3?1ZetaDevelop?1Zeta?1SegUtils?4c?$AA@", i64 noundef 61, ptr noundef @"??_C@_09OHLBJAFH@SegShoveR?$AA@"), !dbg !379
  %130 = call i32 (ptr, ...) @printf(ptr noundef @"??_C@_0M@BJGCKIIG@?7assert?7?$CFs?6?$AA@", ptr noundef @"??_C@_0BF@LNJADDKF@width?5?$DN?$DN?5r_cv?9?$DOwidth?$AA@"), !dbg !379
  %131 = call ptr @__acrt_iob_func(i32 noundef 1), !dbg !381
  %132 = call i32 @fflush(ptr noundef %131), !dbg !381
  %133 = load i8, ptr @zeta_debugger_assert_stage, align 1, !dbg !379
  %134 = trunc i8 %133 to i1, !dbg !379
  br i1 %134, label %143, label %135, !dbg !379

135:                                              ; preds = %128
  store i8 1, ptr @zeta_debugger_assert_stage, align 1, !dbg !384
  %136 = load ptr, ptr @zeta_debugger_assert_callback, align 8, !dbg !384
  %137 = icmp ne ptr %136, null, !dbg !384
  br i1 %137, label %138, label %142, !dbg !384

138:                                              ; preds = %135
  %139 = load ptr, ptr @zeta_debugger_assert_callback, align 8, !dbg !387
  %140 = load ptr, ptr @zeta_debugger_assert_callback_context, align 8, !dbg !387
  %141 = call i32 %139(ptr noundef %140), !dbg !387
  br label %142, !dbg !387

142:                                              ; preds = %138, %135
  br label %143, !dbg !384

143:                                              ; preds = %142, %128
  call void @exit(i32 noundef 0) #5, !dbg !379
  unreachable, !dbg !379

144:                                              ; preds = %127
  call void @llvm.dbg.declare(metadata ptr %12, metadata !390, metadata !DIExpression()), !dbg !391
  call void @llvm.dbg.declare(metadata ptr %13, metadata !392, metadata !DIExpression()), !dbg !394
  %145 = load i64, ptr %8, align 8, !dbg !394
  store i64 %145, ptr %13, align 8, !dbg !394
  call void @llvm.dbg.declare(metadata ptr %14, metadata !395, metadata !DIExpression()), !dbg !394
  %146 = load i64, ptr %6, align 8, !dbg !394
  store i64 %146, ptr %14, align 8, !dbg !394
  %147 = load i64, ptr %13, align 8, !dbg !394
  %148 = load i64, ptr %14, align 8, !dbg !394
  %149 = icmp ult i64 %147, %148, !dbg !394
  br i1 %149, label %150, label %152, !dbg !394

150:                                              ; preds = %144
  %151 = load i64, ptr %13, align 8, !dbg !394
  br label %154, !dbg !394

152:                                              ; preds = %144
  %153 = load i64, ptr %14, align 8, !dbg !394
  br label %154, !dbg !394

154:                                              ; preds = %152, %150
  %155 = phi i64 [ %151, %150 ], [ %153, %152 ], !dbg !394
  store i64 %155, ptr %15, align 8, !dbg !394
  %156 = load i64, ptr %15, align 8, !dbg !394
  store i64 %156, ptr %12, align 8, !dbg !391
  call void @llvm.dbg.declare(metadata ptr %16, metadata !396, metadata !DIExpression()), !dbg !397
  call void @llvm.dbg.declare(metadata ptr %17, metadata !398, metadata !DIExpression()), !dbg !400
  %157 = load i64, ptr %7, align 8, !dbg !400
  store i64 %157, ptr %17, align 8, !dbg !400
  call void @llvm.dbg.declare(metadata ptr %18, metadata !401, metadata !DIExpression()), !dbg !400
  %158 = load i64, ptr %6, align 8, !dbg !400
  %159 = load i64, ptr %12, align 8, !dbg !400
  %160 = sub i64 %158, %159, !dbg !400
  store i64 %160, ptr %18, align 8, !dbg !400
  %161 = load i64, ptr %17, align 8, !dbg !400
  %162 = load i64, ptr %18, align 8, !dbg !400
  %163 = icmp ult i64 %161, %162, !dbg !400
  br i1 %163, label %164, label %166, !dbg !400

164:                                              ; preds = %154
  %165 = load i64, ptr %17, align 8, !dbg !400
  br label %168, !dbg !400

166:                                              ; preds = %154
  %167 = load i64, ptr %18, align 8, !dbg !400
  br label %168, !dbg !400

168:                                              ; preds = %166, %164
  %169 = phi i64 [ %165, %164 ], [ %167, %166 ], !dbg !400
  store i64 %169, ptr %19, align 8, !dbg !400
  %170 = load i64, ptr %19, align 8, !dbg !400
  store i64 %170, ptr %16, align 8, !dbg !397
  call void @llvm.dbg.declare(metadata ptr %20, metadata !402, metadata !DIExpression()), !dbg !403
  %171 = load i64, ptr %6, align 8, !dbg !403
  %172 = load i64, ptr %12, align 8, !dbg !403
  %173 = sub i64 %171, %172, !dbg !403
  %174 = load i64, ptr %16, align 8, !dbg !403
  %175 = sub i64 %173, %174, !dbg !403
  store i64 %175, ptr %20, align 8, !dbg !403
  call void @llvm.dbg.declare(metadata ptr %21, metadata !404, metadata !DIExpression()), !dbg !405
  %176 = load ptr, ptr %10, align 8, !dbg !405
  %177 = getelementptr inbounds %struct.Zeta_CircularVector, ptr %176, i32 0, i32 4, !dbg !405
  %178 = load i64, ptr %177, align 8, !dbg !405
  %179 = sub i64 %178, 1, !dbg !405
  store i64 %179, ptr %21, align 8, !dbg !405
  call void @llvm.dbg.declare(metadata ptr %22, metadata !406, metadata !DIExpression()), !dbg !407
  %180 = load i64, ptr %6, align 8, !dbg !407
  %181 = sub i64 %180, 1, !dbg !407
  store i64 %181, ptr %22, align 8, !dbg !407
  call void @llvm.dbg.declare(metadata ptr %23, metadata !408, metadata !DIExpression()), !dbg !409
  call void @llvm.dbg.declare(metadata ptr %24, metadata !410, metadata !DIExpression()), !dbg !411
  %182 = load ptr, ptr %9, align 8, !dbg !412
  %183 = call ptr @Zeta_CircularVector_PeekL(ptr noundef %182, ptr noundef %24, ptr noundef null), !dbg !412
  %184 = load i64, ptr %6, align 8, !dbg !413
  %185 = load ptr, ptr %9, align 8, !dbg !413
  %186 = call ptr @Zeta_CircularVector_Insert(ptr noundef %185, ptr noundef %24, i64 noundef %184), !dbg !413
  call void @llvm.dbg.declare(metadata ptr %25, metadata !414, metadata !DIExpression()), !dbg !416
  %187 = load i64, ptr %12, align 8, !dbg !416
  store i64 %187, ptr %25, align 8, !dbg !416
  br label %188, !dbg !416

188:                                              ; preds = %200, %168
  %189 = load i64, ptr %25, align 8, !dbg !416
  %190 = add i64 %189, -1, !dbg !416
  store i64 %190, ptr %25, align 8, !dbg !416
  %191 = icmp ult i64 0, %189, !dbg !416
  br i1 %191, label %192, label %205, !dbg !416

192:                                              ; preds = %188
  %193 = load i64, ptr %11, align 8, !dbg !417
  %194 = load i64, ptr %21, align 8, !dbg !417
  %195 = load ptr, ptr %10, align 8, !dbg !417
  %196 = call ptr @Zeta_CircularVector_Access(ptr noundef %195, ptr noundef null, ptr noundef null, i64 noundef %194), !dbg !417
  %197 = load i64, ptr %22, align 8, !dbg !417
  %198 = load ptr, ptr %9, align 8, !dbg !417
  %199 = call ptr @Zeta_CircularVector_Access(ptr noundef %198, ptr noundef null, ptr noundef null, i64 noundef %197), !dbg !417
  call void @Zeta_MemCopy(ptr noundef %199, ptr noundef %196, i64 noundef %193), !dbg !417
  br label %200, !dbg !420

200:                                              ; preds = %192
  %201 = load i64, ptr %21, align 8, !dbg !421
  %202 = add i64 %201, -1, !dbg !421
  store i64 %202, ptr %21, align 8, !dbg !421
  %203 = load i64, ptr %22, align 8, !dbg !421
  %204 = add i64 %203, -1, !dbg !421
  store i64 %204, ptr %22, align 8, !dbg !421
  br label %188, !dbg !421, !llvm.loop !422

205:                                              ; preds = %188
  %206 = load i64, ptr %16, align 8, !dbg !424
  %207 = load i64, ptr %22, align 8, !dbg !424
  %208 = sub i64 %207, %206, !dbg !424
  store i64 %208, ptr %22, align 8, !dbg !424
  call void @llvm.dbg.declare(metadata ptr %26, metadata !425, metadata !DIExpression()), !dbg !427
  %209 = load i64, ptr %20, align 8, !dbg !427
  store i64 %209, ptr %26, align 8, !dbg !427
  br label %210, !dbg !427

210:                                              ; preds = %222, %205
  %211 = load i64, ptr %26, align 8, !dbg !427
  %212 = add i64 %211, -1, !dbg !427
  store i64 %212, ptr %26, align 8, !dbg !427
  %213 = icmp ult i64 0, %211, !dbg !427
  br i1 %213, label %214, label %227, !dbg !427

214:                                              ; preds = %210
  %215 = load i64, ptr %11, align 8, !dbg !428
  %216 = load i64, ptr %21, align 8, !dbg !428
  %217 = load ptr, ptr %10, align 8, !dbg !428
  %218 = call ptr @Zeta_CircularVector_Access(ptr noundef %217, ptr noundef null, ptr noundef null, i64 noundef %216), !dbg !428
  %219 = load i64, ptr %22, align 8, !dbg !428
  %220 = load ptr, ptr %9, align 8, !dbg !428
  %221 = call ptr @Zeta_CircularVector_Access(ptr noundef %220, ptr noundef null, ptr noundef null, i64 noundef %219), !dbg !428
  call void @Zeta_MemCopy(ptr noundef %221, ptr noundef %218, i64 noundef %215), !dbg !428
  br label %222, !dbg !431

222:                                              ; preds = %214
  %223 = load i64, ptr %21, align 8, !dbg !432
  %224 = add i64 %223, -1, !dbg !432
  store i64 %224, ptr %21, align 8, !dbg !432
  %225 = load i64, ptr %22, align 8, !dbg !432
  %226 = add i64 %225, -1, !dbg !432
  store i64 %226, ptr %22, align 8, !dbg !432
  br label %210, !dbg !432, !llvm.loop !433

227:                                              ; preds = %210
  %228 = load ptr, ptr %10, align 8, !dbg !435
  %229 = getelementptr inbounds %struct.Zeta_CircularVector, ptr %228, i32 0, i32 4, !dbg !435
  %230 = load i64, ptr %229, align 8, !dbg !435
  %231 = load i64, ptr %12, align 8, !dbg !435
  %232 = sub i64 %230, %231, !dbg !435
  %233 = load i64, ptr %20, align 8, !dbg !435
  %234 = sub i64 %232, %233, !dbg !435
  %235 = load ptr, ptr %10, align 8, !dbg !435
  %236 = call ptr @Zeta_CircularVector_Access(ptr noundef %235, ptr noundef %23, ptr noundef null, i64 noundef %234), !dbg !435
  %237 = load i64, ptr %12, align 8, !dbg !436
  %238 = load i64, ptr %20, align 8, !dbg !436
  %239 = add i64 %237, %238, !dbg !436
  %240 = load ptr, ptr %10, align 8, !dbg !436
  call void @Zeta_CircularVector_Erase(ptr noundef %240, ptr noundef %23, i64 noundef %239), !dbg !436
  %241 = load i64, ptr %8, align 8, !dbg !437
  %242 = load i64, ptr %12, align 8, !dbg !437
  %243 = sub i64 %241, %242, !dbg !437
  %244 = load ptr, ptr %10, align 8, !dbg !437
  call void @Zeta_CircularVector_Cursor_AdvanceL(ptr noundef %244, ptr noundef %23, i64 noundef %243), !dbg !437
  %245 = load i64, ptr %7, align 8, !dbg !438
  %246 = load i64, ptr %16, align 8, !dbg !438
  %247 = sub i64 %245, %246, !dbg !438
  %248 = load ptr, ptr %10, align 8, !dbg !438
  %249 = call ptr @Zeta_CircularVector_Insert(ptr noundef %248, ptr noundef %23, i64 noundef %247), !dbg !438
  ret void, !dbg !439
}

declare dso_local void @Zeta_CircularVector_Cursor_AdvanceL(ptr noundef, ptr noundef, i64 noundef) #2

; Function Attrs: nocallback nofree nosync nounwind willreturn
declare void @llvm.va_start(ptr) #4

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr dso_local i32 @_vsprintf_l(ptr noundef %0, ptr noundef %1, ptr noundef %2, ptr noundef %3) #0 comdat !dbg !440 {
  %5 = alloca ptr, align 8
  %6 = alloca ptr, align 8
  %7 = alloca ptr, align 8
  %8 = alloca ptr, align 8
  store ptr %3, ptr %5, align 8
  call void @llvm.dbg.declare(metadata ptr %5, metadata !456, metadata !DIExpression()), !dbg !457
  store ptr %2, ptr %6, align 8
  call void @llvm.dbg.declare(metadata ptr %6, metadata !458, metadata !DIExpression()), !dbg !459
  store ptr %1, ptr %7, align 8
  call void @llvm.dbg.declare(metadata ptr %7, metadata !460, metadata !DIExpression()), !dbg !461
  store ptr %0, ptr %8, align 8
  call void @llvm.dbg.declare(metadata ptr %8, metadata !462, metadata !DIExpression()), !dbg !463
  %9 = load ptr, ptr %5, align 8, !dbg !464
  %10 = load ptr, ptr %6, align 8, !dbg !464
  %11 = load ptr, ptr %7, align 8, !dbg !464
  %12 = load ptr, ptr %8, align 8, !dbg !464
  %13 = call i32 @_vsnprintf_l(ptr noundef %12, i64 noundef -1, ptr noundef %11, ptr noundef %10, ptr noundef %9), !dbg !464
  ret i32 %13, !dbg !464
}

; Function Attrs: nocallback nofree nosync nounwind willreturn
declare void @llvm.va_end(ptr) #4

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr dso_local i32 @_vsnprintf_l(ptr noundef %0, i64 noundef %1, ptr noundef %2, ptr noundef %3, ptr noundef %4) #0 comdat !dbg !465 {
  %6 = alloca ptr, align 8
  %7 = alloca ptr, align 8
  %8 = alloca ptr, align 8
  %9 = alloca i64, align 8
  %10 = alloca ptr, align 8
  %11 = alloca i32, align 4
  store ptr %4, ptr %6, align 8
  call void @llvm.dbg.declare(metadata ptr %6, metadata !468, metadata !DIExpression()), !dbg !469
  store ptr %3, ptr %7, align 8
  call void @llvm.dbg.declare(metadata ptr %7, metadata !470, metadata !DIExpression()), !dbg !471
  store ptr %2, ptr %8, align 8
  call void @llvm.dbg.declare(metadata ptr %8, metadata !472, metadata !DIExpression()), !dbg !473
  store i64 %1, ptr %9, align 8
  call void @llvm.dbg.declare(metadata ptr %9, metadata !474, metadata !DIExpression()), !dbg !475
  store ptr %0, ptr %10, align 8
  call void @llvm.dbg.declare(metadata ptr %10, metadata !476, metadata !DIExpression()), !dbg !477
  call void @llvm.dbg.declare(metadata ptr %11, metadata !478, metadata !DIExpression()), !dbg !480
  %12 = load ptr, ptr %6, align 8, !dbg !480
  %13 = load ptr, ptr %7, align 8, !dbg !480
  %14 = load ptr, ptr %8, align 8, !dbg !480
  %15 = load i64, ptr %9, align 8, !dbg !480
  %16 = load ptr, ptr %10, align 8, !dbg !480
  %17 = call ptr @__local_stdio_printf_options(), !dbg !480
  %18 = load i64, ptr %17, align 8, !dbg !480
  %19 = or i64 %18, 1, !dbg !480
  %20 = call i32 @__stdio_common_vsprintf(i64 noundef %19, ptr noundef %16, i64 noundef %15, ptr noundef %14, ptr noundef %13, ptr noundef %12), !dbg !480
  store i32 %20, ptr %11, align 4, !dbg !480
  %21 = load i32, ptr %11, align 4, !dbg !481
  %22 = icmp slt i32 %21, 0, !dbg !481
  br i1 %22, label %23, label %24, !dbg !481

23:                                               ; preds = %5
  br label %26, !dbg !481

24:                                               ; preds = %5
  %25 = load i32, ptr %11, align 4, !dbg !481
  br label %26, !dbg !481

26:                                               ; preds = %24, %23
  %27 = phi i32 [ -1, %23 ], [ %25, %24 ], !dbg !481
  ret i32 %27, !dbg !481
}

declare dso_local i32 @__stdio_common_vsprintf(i64 noundef, ptr noundef, i64 noundef, ptr noundef, ptr noundef, ptr noundef) #2

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr dso_local ptr @__local_stdio_printf_options() #0 comdat !dbg !53 {
  ret ptr @__local_stdio_printf_options._OptionsStorage, !dbg !482
}

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr dso_local i32 @_vfprintf_l(ptr noundef %0, ptr noundef %1, ptr noundef %2, ptr noundef %3) #0 comdat !dbg !483 {
  %5 = alloca ptr, align 8
  %6 = alloca ptr, align 8
  %7 = alloca ptr, align 8
  %8 = alloca ptr, align 8
  store ptr %3, ptr %5, align 8
  call void @llvm.dbg.declare(metadata ptr %5, metadata !493, metadata !DIExpression()), !dbg !494
  store ptr %2, ptr %6, align 8
  call void @llvm.dbg.declare(metadata ptr %6, metadata !495, metadata !DIExpression()), !dbg !496
  store ptr %1, ptr %7, align 8
  call void @llvm.dbg.declare(metadata ptr %7, metadata !497, metadata !DIExpression()), !dbg !498
  store ptr %0, ptr %8, align 8
  call void @llvm.dbg.declare(metadata ptr %8, metadata !499, metadata !DIExpression()), !dbg !500
  %9 = load ptr, ptr %5, align 8, !dbg !501
  %10 = load ptr, ptr %6, align 8, !dbg !501
  %11 = load ptr, ptr %7, align 8, !dbg !501
  %12 = load ptr, ptr %8, align 8, !dbg !501
  %13 = call ptr @__local_stdio_printf_options(), !dbg !501
  %14 = load i64, ptr %13, align 8, !dbg !501
  %15 = call i32 @__stdio_common_vfprintf(i64 noundef %14, ptr noundef %12, ptr noundef %11, ptr noundef %10, ptr noundef %9), !dbg !501
  ret i32 %15, !dbg !501
}

declare dso_local i32 @__stdio_common_vfprintf(i64 noundef, ptr noundef, ptr noundef, ptr noundef, ptr noundef) #2

attributes #0 = { noinline nounwind optnone uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }
attributes #2 = { "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #3 = { noreturn "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #4 = { nocallback nofree nosync nounwind willreturn }
attributes #5 = { noreturn }

!llvm.dbg.cu = !{!59}
!llvm.module.flags = !{!66, !67, !68, !69, !70, !71}
!llvm.ident = !{!72}

!0 = !DIGlobalVariableExpression(var: !1, expr: !DIExpression())
!1 = distinct !DIGlobalVariable(scope: null, file: !2, line: 8, type: !3, isLocal: true, isDefinition: true)
!2 = !DIFile(filename: "D:/ZetaDevelop/Zeta/SegUtils.c", directory: "", checksumkind: CSK_MD5, checksum: "67c0c08dbabed6f68e09af18f36c7400")
!3 = !DICompositeType(tag: DW_TAG_array_type, baseType: !4, size: 80, elements: !6)
!4 = !DIDerivedType(tag: DW_TAG_const_type, baseType: !5)
!5 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!6 = !{!7}
!7 = !DISubrange(count: 10)
!8 = !DIGlobalVariableExpression(var: !9, expr: !DIExpression())
!9 = distinct !DIGlobalVariable(scope: null, file: !2, line: 8, type: !10, isLocal: true, isDefinition: true)
!10 = !DICompositeType(tag: DW_TAG_array_type, baseType: !5, size: 248, elements: !11)
!11 = !{!12}
!12 = !DISubrange(count: 31)
!13 = !DIGlobalVariableExpression(var: !14, expr: !DIExpression())
!14 = distinct !DIGlobalVariable(scope: null, file: !2, line: 8, type: !15, isLocal: true, isDefinition: true)
!15 = !DICompositeType(tag: DW_TAG_array_type, baseType: !5, size: 144, elements: !16)
!16 = !{!17}
!17 = !DISubrange(count: 18)
!18 = !DIGlobalVariableExpression(var: !19, expr: !DIExpression())
!19 = distinct !DIGlobalVariable(scope: null, file: !2, line: 8, type: !20, isLocal: true, isDefinition: true)
!20 = !DICompositeType(tag: DW_TAG_array_type, baseType: !5, size: 152, elements: !21)
!21 = !{!22}
!22 = !DISubrange(count: 19)
!23 = !DIGlobalVariableExpression(var: !24, expr: !DIExpression())
!24 = distinct !DIGlobalVariable(scope: null, file: !2, line: 8, type: !25, isLocal: true, isDefinition: true)
!25 = !DICompositeType(tag: DW_TAG_array_type, baseType: !5, size: 96, elements: !26)
!26 = !{!27}
!27 = !DISubrange(count: 12)
!28 = !DIGlobalVariableExpression(var: !29, expr: !DIExpression())
!29 = distinct !DIGlobalVariable(scope: null, file: !2, line: 9, type: !20, isLocal: true, isDefinition: true)
!30 = !DIGlobalVariableExpression(var: !31, expr: !DIExpression())
!31 = distinct !DIGlobalVariable(scope: null, file: !2, line: 11, type: !32, isLocal: true, isDefinition: true)
!32 = !DICompositeType(tag: DW_TAG_array_type, baseType: !5, size: 328, elements: !33)
!33 = !{!34}
!34 = !DISubrange(count: 41)
!35 = !DIGlobalVariableExpression(var: !36, expr: !DIExpression())
!36 = distinct !DIGlobalVariable(scope: null, file: !2, line: 12, type: !37, isLocal: true, isDefinition: true)
!37 = !DICompositeType(tag: DW_TAG_array_type, baseType: !5, size: 272, elements: !38)
!38 = !{!39}
!39 = !DISubrange(count: 34)
!40 = !DIGlobalVariableExpression(var: !41, expr: !DIExpression())
!41 = distinct !DIGlobalVariable(scope: null, file: !2, line: 15, type: !42, isLocal: true, isDefinition: true)
!42 = !DICompositeType(tag: DW_TAG_array_type, baseType: !5, size: 168, elements: !43)
!43 = !{!44}
!44 = !DISubrange(count: 21)
!45 = !DIGlobalVariableExpression(var: !46, expr: !DIExpression())
!46 = distinct !DIGlobalVariable(scope: null, file: !2, line: 54, type: !3, isLocal: true, isDefinition: true)
!47 = !DIGlobalVariableExpression(var: !48, expr: !DIExpression())
!48 = distinct !DIGlobalVariable(scope: null, file: !2, line: 57, type: !32, isLocal: true, isDefinition: true)
!49 = !DIGlobalVariableExpression(var: !50, expr: !DIExpression())
!50 = distinct !DIGlobalVariable(scope: null, file: !2, line: 58, type: !37, isLocal: true, isDefinition: true)
!51 = !DIGlobalVariableExpression(var: !52, expr: !DIExpression())
!52 = distinct !DIGlobalVariable(name: "_OptionsStorage", scope: !53, file: !54, line: 91, type: !58, isLocal: true, isDefinition: true)
!53 = distinct !DISubprogram(name: "__local_stdio_printf_options", scope: !54, file: !54, line: 89, type: !55, scopeLine: 90, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !59)
!54 = !DIFile(filename: "C:\\Program Files (x86)\\Windows Kits\\10\\Include\\10.0.22621.0\\ucrt\\corecrt_stdio_config.h", directory: "", checksumkind: CSK_MD5, checksum: "dacf907bda504afb0b64f53a242bdae6")
!55 = !DISubroutineType(types: !56)
!56 = !{!57}
!57 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !58, size: 64)
!58 = !DIBasicType(name: "unsigned long long", size: 64, encoding: DW_ATE_unsigned)
!59 = distinct !DICompileUnit(language: DW_LANG_C11, file: !60, producer: "clang version 18.1.4", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, retainedTypes: !61, globals: !65, splitDebugInlining: false, nameTableKind: None)
!60 = !DIFile(filename: "D:/ZetaDevelop/Zeta\\SegUtils.c", directory: "D:\\ZetaDevelop", checksumkind: CSK_MD5, checksum: "67c0c08dbabed6f68e09af18f36c7400")
!61 = !{!62, !58, !63}
!62 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: null, size: 64)
!63 = !DIDerivedType(tag: DW_TAG_typedef, name: "size_t", file: !64, line: 193, baseType: !58)
!64 = !DIFile(filename: "C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\VC\\Tools\\MSVC\\14.38.33130\\include\\vcruntime.h", directory: "", checksumkind: CSK_MD5, checksum: "39da3a8c8438e40538f3964bd55ef6b8")
!65 = !{!0, !8, !13, !18, !23, !28, !30, !35, !40, !45, !47, !49, !51}
!66 = !{i32 2, !"CodeView", i32 1}
!67 = !{i32 2, !"Debug Info Version", i32 3}
!68 = !{i32 1, !"wchar_size", i32 2}
!69 = !{i32 8, !"PIC Level", i32 2}
!70 = !{i32 7, !"uwtable", i32 2}
!71 = !{i32 1, !"MaxTLSAlign", i32 65536}
!72 = !{!"clang version 18.1.4"}
!73 = distinct !DISubprogram(name: "sprintf", scope: !74, file: !74, line: 1764, type: !75, scopeLine: 1771, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !59, retainedNodes: !82)
!74 = !DIFile(filename: "C:\\Program Files (x86)\\Windows Kits\\10\\Include\\10.0.22621.0\\ucrt\\stdio.h", directory: "", checksumkind: CSK_MD5, checksum: "c1a1fbc43e7d45f0ea4ae539ddcffb19")
!75 = !DISubroutineType(types: !76)
!76 = !{!77, !78, !80, null}
!77 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!78 = !DIDerivedType(tag: DW_TAG_const_type, baseType: !79)
!79 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !5, size: 64)
!80 = !DIDerivedType(tag: DW_TAG_const_type, baseType: !81)
!81 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !4, size: 64)
!82 = !{}
!83 = !DILocalVariable(name: "_Format", arg: 2, scope: !73, file: !74, line: 1766, type: !80)
!84 = !DILocation(line: 1766, scope: !73)
!85 = !DILocalVariable(name: "_Buffer", arg: 1, scope: !73, file: !74, line: 1765, type: !78)
!86 = !DILocation(line: 1765, scope: !73)
!87 = !DILocalVariable(name: "_Result", scope: !73, file: !74, line: 1772, type: !77)
!88 = !DILocation(line: 1772, scope: !73)
!89 = !DILocalVariable(name: "_ArgList", scope: !73, file: !74, line: 1773, type: !90)
!90 = !DIDerivedType(tag: DW_TAG_typedef, name: "va_list", file: !91, line: 72, baseType: !79)
!91 = !DIFile(filename: "C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\VC\\Tools\\MSVC\\14.38.33130\\include\\vadefs.h", directory: "", checksumkind: CSK_MD5, checksum: "a4b8f96637d0704c82f39ecb6bde2ab4")
!92 = !DILocation(line: 1773, scope: !73)
!93 = !DILocation(line: 1774, scope: !73)
!94 = !DILocation(line: 1776, scope: !73)
!95 = !DILocation(line: 1778, scope: !73)
!96 = !DILocation(line: 1779, scope: !73)
!97 = distinct !DISubprogram(name: "vsprintf", scope: !74, file: !74, line: 1465, type: !98, scopeLine: 1473, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !59, retainedNodes: !82)
!98 = !DISubroutineType(types: !99)
!99 = !{!77, !78, !80, !90}
!100 = !DILocalVariable(name: "_ArgList", arg: 3, scope: !97, file: !74, line: 1468, type: !90)
!101 = !DILocation(line: 1468, scope: !97)
!102 = !DILocalVariable(name: "_Format", arg: 2, scope: !97, file: !74, line: 1467, type: !80)
!103 = !DILocation(line: 1467, scope: !97)
!104 = !DILocalVariable(name: "_Buffer", arg: 1, scope: !97, file: !74, line: 1466, type: !78)
!105 = !DILocation(line: 1466, scope: !97)
!106 = !DILocation(line: 1474, scope: !97)
!107 = distinct !DISubprogram(name: "_snprintf", scope: !74, file: !74, line: 1939, type: !108, scopeLine: 1947, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !59, retainedNodes: !82)
!108 = !DISubroutineType(types: !109)
!109 = !{!77, !78, !110, !80, null}
!110 = !DIDerivedType(tag: DW_TAG_const_type, baseType: !63)
!111 = !DILocalVariable(name: "_Format", arg: 3, scope: !107, file: !74, line: 1942, type: !80)
!112 = !DILocation(line: 1942, scope: !107)
!113 = !DILocalVariable(name: "_BufferCount", arg: 2, scope: !107, file: !74, line: 1941, type: !110)
!114 = !DILocation(line: 1941, scope: !107)
!115 = !DILocalVariable(name: "_Buffer", arg: 1, scope: !107, file: !74, line: 1940, type: !78)
!116 = !DILocation(line: 1940, scope: !107)
!117 = !DILocalVariable(name: "_Result", scope: !107, file: !74, line: 1948, type: !77)
!118 = !DILocation(line: 1948, scope: !107)
!119 = !DILocalVariable(name: "_ArgList", scope: !107, file: !74, line: 1949, type: !90)
!120 = !DILocation(line: 1949, scope: !107)
!121 = !DILocation(line: 1950, scope: !107)
!122 = !DILocation(line: 1951, scope: !107)
!123 = !DILocation(line: 1952, scope: !107)
!124 = !DILocation(line: 1953, scope: !107)
!125 = distinct !DISubprogram(name: "_vsnprintf", scope: !74, file: !74, line: 1402, type: !126, scopeLine: 1411, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !59, retainedNodes: !82)
!126 = !DISubroutineType(types: !127)
!127 = !{!77, !78, !110, !80, !90}
!128 = !DILocalVariable(name: "_ArgList", arg: 4, scope: !125, file: !74, line: 1406, type: !90)
!129 = !DILocation(line: 1406, scope: !125)
!130 = !DILocalVariable(name: "_Format", arg: 3, scope: !125, file: !74, line: 1405, type: !80)
!131 = !DILocation(line: 1405, scope: !125)
!132 = !DILocalVariable(name: "_BufferCount", arg: 2, scope: !125, file: !74, line: 1404, type: !110)
!133 = !DILocation(line: 1404, scope: !125)
!134 = !DILocalVariable(name: "_Buffer", arg: 1, scope: !125, file: !74, line: 1403, type: !78)
!135 = !DILocation(line: 1403, scope: !125)
!136 = !DILocation(line: 1412, scope: !125)
!137 = distinct !DISubprogram(name: "SegShoveL", scope: !2, file: !2, line: 6, type: !138, scopeLine: 7, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !59, retainedNodes: !82)
!138 = !DISubroutineType(types: !139)
!139 = !{null, !140, !140, !63, !63, !63}
!140 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !141, size: 64)
!141 = !DIDerivedType(tag: DW_TAG_typedef, name: "Zeta_CircularVector", file: !142, line: 10, baseType: !143)
!142 = !DIFile(filename: "D:/ZetaDevelop/Zeta\\CircularVector.h", directory: "", checksumkind: CSK_MD5, checksum: "2ac38c200004d62d287611362e956ca2")
!143 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "Zeta_CircularVector", file: !142, line: 13, size: 384, elements: !144)
!144 = !{!145, !146, !147, !148, !149, !150}
!145 = !DIDerivedType(tag: DW_TAG_member, name: "data", scope: !143, file: !142, line: 14, baseType: !62, size: 64)
!146 = !DIDerivedType(tag: DW_TAG_member, name: "width", scope: !143, file: !142, line: 15, baseType: !63, size: 64, offset: 64)
!147 = !DIDerivedType(tag: DW_TAG_member, name: "stride", scope: !143, file: !142, line: 16, baseType: !63, size: 64, offset: 128)
!148 = !DIDerivedType(tag: DW_TAG_member, name: "offset", scope: !143, file: !142, line: 17, baseType: !63, size: 64, offset: 192)
!149 = !DIDerivedType(tag: DW_TAG_member, name: "size", scope: !143, file: !142, line: 18, baseType: !63, size: 64, offset: 256)
!150 = !DIDerivedType(tag: DW_TAG_member, name: "capacity", scope: !143, file: !142, line: 19, baseType: !63, size: 64, offset: 320)
!151 = !DILocalVariable(name: "shove_cnt", arg: 5, scope: !137, file: !2, line: 7, type: !63)
!152 = !DILocation(line: 7, scope: !137)
!153 = !DILocalVariable(name: "ins_cnt", arg: 4, scope: !137, file: !2, line: 7, type: !63)
!154 = !DILocalVariable(name: "rl_cnt", arg: 3, scope: !137, file: !2, line: 7, type: !63)
!155 = !DILocalVariable(name: "r_cv", arg: 2, scope: !137, file: !2, line: 6, type: !140)
!156 = !DILocation(line: 6, scope: !137)
!157 = !DILocalVariable(name: "l_cv", arg: 1, scope: !137, file: !2, line: 6, type: !140)
!158 = !DILocation(line: 8, scope: !137)
!159 = !DILocation(line: 8, scope: !160)
!160 = distinct !DILexicalBlock(scope: !161, file: !2, line: 8)
!161 = distinct !DILexicalBlock(scope: !137, file: !2, line: 8)
!162 = !DILocation(line: 8, scope: !163)
!163 = distinct !DILexicalBlock(scope: !161, file: !2, line: 8)
!164 = !DILocation(line: 8, scope: !165)
!165 = distinct !DILexicalBlock(scope: !166, file: !2, line: 8)
!166 = distinct !DILexicalBlock(scope: !163, file: !2, line: 8)
!167 = !DILocation(line: 8, scope: !168)
!168 = distinct !DILexicalBlock(scope: !169, file: !2, line: 8)
!169 = distinct !DILexicalBlock(scope: !163, file: !2, line: 8)
!170 = !DILocation(line: 8, scope: !171)
!171 = distinct !DILexicalBlock(scope: !172, file: !2, line: 8)
!172 = distinct !DILexicalBlock(scope: !168, file: !2, line: 8)
!173 = !DILocation(line: 9, scope: !137)
!174 = !DILocation(line: 9, scope: !175)
!175 = distinct !DILexicalBlock(scope: !176, file: !2, line: 9)
!176 = distinct !DILexicalBlock(scope: !137, file: !2, line: 9)
!177 = !DILocation(line: 9, scope: !178)
!178 = distinct !DILexicalBlock(scope: !176, file: !2, line: 9)
!179 = !DILocation(line: 9, scope: !180)
!180 = distinct !DILexicalBlock(scope: !181, file: !2, line: 9)
!181 = distinct !DILexicalBlock(scope: !178, file: !2, line: 9)
!182 = !DILocation(line: 9, scope: !183)
!183 = distinct !DILexicalBlock(scope: !184, file: !2, line: 9)
!184 = distinct !DILexicalBlock(scope: !178, file: !2, line: 9)
!185 = !DILocation(line: 9, scope: !186)
!186 = distinct !DILexicalBlock(scope: !187, file: !2, line: 9)
!187 = distinct !DILexicalBlock(scope: !183, file: !2, line: 9)
!188 = !DILocation(line: 11, scope: !137)
!189 = !DILocation(line: 11, scope: !190)
!190 = distinct !DILexicalBlock(scope: !191, file: !2, line: 11)
!191 = distinct !DILexicalBlock(scope: !137, file: !2, line: 11)
!192 = !DILocation(line: 11, scope: !193)
!193 = distinct !DILexicalBlock(scope: !191, file: !2, line: 11)
!194 = !DILocation(line: 11, scope: !195)
!195 = distinct !DILexicalBlock(scope: !196, file: !2, line: 11)
!196 = distinct !DILexicalBlock(scope: !193, file: !2, line: 11)
!197 = !DILocation(line: 11, scope: !198)
!198 = distinct !DILexicalBlock(scope: !199, file: !2, line: 11)
!199 = distinct !DILexicalBlock(scope: !193, file: !2, line: 11)
!200 = !DILocation(line: 11, scope: !201)
!201 = distinct !DILexicalBlock(scope: !202, file: !2, line: 11)
!202 = distinct !DILexicalBlock(scope: !198, file: !2, line: 11)
!203 = !DILocation(line: 12, scope: !137)
!204 = !DILocation(line: 12, scope: !205)
!205 = distinct !DILexicalBlock(scope: !206, file: !2, line: 12)
!206 = distinct !DILexicalBlock(scope: !137, file: !2, line: 12)
!207 = !DILocation(line: 12, scope: !208)
!208 = distinct !DILexicalBlock(scope: !206, file: !2, line: 12)
!209 = !DILocation(line: 12, scope: !210)
!210 = distinct !DILexicalBlock(scope: !211, file: !2, line: 12)
!211 = distinct !DILexicalBlock(scope: !208, file: !2, line: 12)
!212 = !DILocation(line: 12, scope: !213)
!213 = distinct !DILexicalBlock(scope: !214, file: !2, line: 12)
!214 = distinct !DILexicalBlock(scope: !208, file: !2, line: 12)
!215 = !DILocation(line: 12, scope: !216)
!216 = distinct !DILexicalBlock(scope: !217, file: !2, line: 12)
!217 = distinct !DILexicalBlock(scope: !213, file: !2, line: 12)
!218 = !DILocalVariable(name: "width", scope: !137, file: !2, line: 14, type: !63)
!219 = !DILocation(line: 14, scope: !137)
!220 = !DILocation(line: 15, scope: !137)
!221 = !DILocation(line: 15, scope: !222)
!222 = distinct !DILexicalBlock(scope: !223, file: !2, line: 15)
!223 = distinct !DILexicalBlock(scope: !137, file: !2, line: 15)
!224 = !DILocation(line: 15, scope: !225)
!225 = distinct !DILexicalBlock(scope: !223, file: !2, line: 15)
!226 = !DILocation(line: 15, scope: !227)
!227 = distinct !DILexicalBlock(scope: !228, file: !2, line: 15)
!228 = distinct !DILexicalBlock(scope: !225, file: !2, line: 15)
!229 = !DILocation(line: 15, scope: !230)
!230 = distinct !DILexicalBlock(scope: !231, file: !2, line: 15)
!231 = distinct !DILexicalBlock(scope: !225, file: !2, line: 15)
!232 = !DILocation(line: 15, scope: !233)
!233 = distinct !DILexicalBlock(scope: !234, file: !2, line: 15)
!234 = distinct !DILexicalBlock(scope: !230, file: !2, line: 15)
!235 = !DILocalVariable(name: "cnt_a", scope: !137, file: !2, line: 17, type: !63)
!236 = !DILocation(line: 17, scope: !137)
!237 = !DILocalVariable(name: "ZETA_tmp_0", scope: !238, file: !2, line: 17, type: !63)
!238 = distinct !DILexicalBlock(scope: !137, file: !2, line: 17)
!239 = !DILocation(line: 17, scope: !238)
!240 = !DILocalVariable(name: "ZETA_tmp_1", scope: !238, file: !2, line: 17, type: !63)
!241 = !DILocalVariable(name: "cnt_b", scope: !137, file: !2, line: 18, type: !63)
!242 = !DILocation(line: 18, scope: !137)
!243 = !DILocalVariable(name: "ZETA_tmp_2", scope: !244, file: !2, line: 18, type: !63)
!244 = distinct !DILexicalBlock(scope: !137, file: !2, line: 18)
!245 = !DILocation(line: 18, scope: !244)
!246 = !DILocalVariable(name: "ZETA_tmp_3", scope: !244, file: !2, line: 18, type: !63)
!247 = !DILocalVariable(name: "cnt_c", scope: !137, file: !2, line: 19, type: !63)
!248 = !DILocation(line: 19, scope: !137)
!249 = !DILocalVariable(name: "l_i", scope: !137, file: !2, line: 21, type: !63)
!250 = !DILocation(line: 21, scope: !137)
!251 = !DILocalVariable(name: "r_i", scope: !137, file: !2, line: 22, type: !63)
!252 = !DILocation(line: 22, scope: !137)
!253 = !DILocalVariable(name: "l_cv_cursor", scope: !137, file: !2, line: 24, type: !254)
!254 = !DIDerivedType(tag: DW_TAG_typedef, name: "Zeta_CircularVector_Cursor", file: !142, line: 11, baseType: !255)
!255 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "Zeta_CircularVector_Cursor", file: !142, line: 22, size: 192, elements: !256)
!256 = !{!257, !258, !259}
!257 = !DIDerivedType(tag: DW_TAG_member, name: "cv", scope: !255, file: !142, line: 23, baseType: !140, size: 64)
!258 = !DIDerivedType(tag: DW_TAG_member, name: "idx", scope: !255, file: !142, line: 24, baseType: !63, size: 64, offset: 64)
!259 = !DIDerivedType(tag: DW_TAG_member, name: "ele", scope: !255, file: !142, line: 25, baseType: !62, size: 64, offset: 128)
!260 = !DILocation(line: 24, scope: !137)
!261 = !DILocalVariable(name: "r_cv_cursor", scope: !137, file: !2, line: 25, type: !254)
!262 = !DILocation(line: 25, scope: !137)
!263 = !DILocation(line: 27, scope: !137)
!264 = !DILocation(line: 29, scope: !137)
!265 = !DILocalVariable(name: "i", scope: !266, file: !2, line: 31, type: !63)
!266 = distinct !DILexicalBlock(scope: !137, file: !2, line: 31)
!267 = !DILocation(line: 31, scope: !266)
!268 = !DILocation(line: 32, scope: !269)
!269 = distinct !DILexicalBlock(scope: !270, file: !2, line: 31)
!270 = distinct !DILexicalBlock(scope: !266, file: !2, line: 31)
!271 = !DILocation(line: 34, scope: !269)
!272 = !DILocation(line: 31, scope: !270)
!273 = distinct !{!273, !267, !274, !275}
!274 = !DILocation(line: 34, scope: !266)
!275 = !{!"llvm.loop.mustprogress"}
!276 = !DILocation(line: 36, scope: !137)
!277 = !DILocalVariable(name: "i", scope: !278, file: !2, line: 38, type: !63)
!278 = distinct !DILexicalBlock(scope: !137, file: !2, line: 38)
!279 = !DILocation(line: 38, scope: !278)
!280 = !DILocation(line: 39, scope: !281)
!281 = distinct !DILexicalBlock(scope: !282, file: !2, line: 38)
!282 = distinct !DILexicalBlock(scope: !278, file: !2, line: 38)
!283 = !DILocation(line: 41, scope: !281)
!284 = !DILocation(line: 38, scope: !282)
!285 = distinct !{!285, !279, !286, !275}
!286 = !DILocation(line: 41, scope: !278)
!287 = !DILocation(line: 43, scope: !137)
!288 = !DILocation(line: 45, scope: !137)
!289 = !DILocation(line: 47, scope: !137)
!290 = !DILocation(line: 49, scope: !137)
!291 = !DILocation(line: 50, scope: !137)
!292 = distinct !DISubprogram(name: "printf", scope: !74, file: !74, line: 950, type: !293, scopeLine: 956, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !59, retainedNodes: !82)
!293 = !DISubroutineType(types: !294)
!294 = !{!77, !80, null}
!295 = !DILocalVariable(name: "_Format", arg: 1, scope: !292, file: !74, line: 951, type: !80)
!296 = !DILocation(line: 951, scope: !292)
!297 = !DILocalVariable(name: "_Result", scope: !292, file: !74, line: 957, type: !77)
!298 = !DILocation(line: 957, scope: !292)
!299 = !DILocalVariable(name: "_ArgList", scope: !292, file: !74, line: 958, type: !90)
!300 = !DILocation(line: 958, scope: !292)
!301 = !DILocation(line: 959, scope: !292)
!302 = !DILocation(line: 960, scope: !292)
!303 = !DILocation(line: 961, scope: !292)
!304 = !DILocation(line: 962, scope: !292)
!305 = distinct !DISubprogram(name: "SegShoveR", scope: !2, file: !2, line: 52, type: !138, scopeLine: 53, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !59, retainedNodes: !82)
!306 = !DILocalVariable(name: "shove_cnt", arg: 5, scope: !305, file: !2, line: 53, type: !63)
!307 = !DILocation(line: 53, scope: !305)
!308 = !DILocalVariable(name: "ins_cnt", arg: 4, scope: !305, file: !2, line: 53, type: !63)
!309 = !DILocalVariable(name: "lr_cnt", arg: 3, scope: !305, file: !2, line: 53, type: !63)
!310 = !DILocalVariable(name: "r_cv", arg: 2, scope: !305, file: !2, line: 52, type: !140)
!311 = !DILocation(line: 52, scope: !305)
!312 = !DILocalVariable(name: "l_cv", arg: 1, scope: !305, file: !2, line: 52, type: !140)
!313 = !DILocation(line: 54, scope: !305)
!314 = !DILocation(line: 54, scope: !315)
!315 = distinct !DILexicalBlock(scope: !316, file: !2, line: 54)
!316 = distinct !DILexicalBlock(scope: !305, file: !2, line: 54)
!317 = !DILocation(line: 54, scope: !318)
!318 = distinct !DILexicalBlock(scope: !316, file: !2, line: 54)
!319 = !DILocation(line: 54, scope: !320)
!320 = distinct !DILexicalBlock(scope: !321, file: !2, line: 54)
!321 = distinct !DILexicalBlock(scope: !318, file: !2, line: 54)
!322 = !DILocation(line: 54, scope: !323)
!323 = distinct !DILexicalBlock(scope: !324, file: !2, line: 54)
!324 = distinct !DILexicalBlock(scope: !318, file: !2, line: 54)
!325 = !DILocation(line: 54, scope: !326)
!326 = distinct !DILexicalBlock(scope: !327, file: !2, line: 54)
!327 = distinct !DILexicalBlock(scope: !323, file: !2, line: 54)
!328 = !DILocation(line: 55, scope: !305)
!329 = !DILocation(line: 55, scope: !330)
!330 = distinct !DILexicalBlock(scope: !331, file: !2, line: 55)
!331 = distinct !DILexicalBlock(scope: !305, file: !2, line: 55)
!332 = !DILocation(line: 55, scope: !333)
!333 = distinct !DILexicalBlock(scope: !331, file: !2, line: 55)
!334 = !DILocation(line: 55, scope: !335)
!335 = distinct !DILexicalBlock(scope: !336, file: !2, line: 55)
!336 = distinct !DILexicalBlock(scope: !333, file: !2, line: 55)
!337 = !DILocation(line: 55, scope: !338)
!338 = distinct !DILexicalBlock(scope: !339, file: !2, line: 55)
!339 = distinct !DILexicalBlock(scope: !333, file: !2, line: 55)
!340 = !DILocation(line: 55, scope: !341)
!341 = distinct !DILexicalBlock(scope: !342, file: !2, line: 55)
!342 = distinct !DILexicalBlock(scope: !338, file: !2, line: 55)
!343 = !DILocation(line: 57, scope: !305)
!344 = !DILocation(line: 57, scope: !345)
!345 = distinct !DILexicalBlock(scope: !346, file: !2, line: 57)
!346 = distinct !DILexicalBlock(scope: !305, file: !2, line: 57)
!347 = !DILocation(line: 57, scope: !348)
!348 = distinct !DILexicalBlock(scope: !346, file: !2, line: 57)
!349 = !DILocation(line: 57, scope: !350)
!350 = distinct !DILexicalBlock(scope: !351, file: !2, line: 57)
!351 = distinct !DILexicalBlock(scope: !348, file: !2, line: 57)
!352 = !DILocation(line: 57, scope: !353)
!353 = distinct !DILexicalBlock(scope: !354, file: !2, line: 57)
!354 = distinct !DILexicalBlock(scope: !348, file: !2, line: 57)
!355 = !DILocation(line: 57, scope: !356)
!356 = distinct !DILexicalBlock(scope: !357, file: !2, line: 57)
!357 = distinct !DILexicalBlock(scope: !353, file: !2, line: 57)
!358 = !DILocation(line: 58, scope: !305)
!359 = !DILocation(line: 58, scope: !360)
!360 = distinct !DILexicalBlock(scope: !361, file: !2, line: 58)
!361 = distinct !DILexicalBlock(scope: !305, file: !2, line: 58)
!362 = !DILocation(line: 58, scope: !363)
!363 = distinct !DILexicalBlock(scope: !361, file: !2, line: 58)
!364 = !DILocation(line: 58, scope: !365)
!365 = distinct !DILexicalBlock(scope: !366, file: !2, line: 58)
!366 = distinct !DILexicalBlock(scope: !363, file: !2, line: 58)
!367 = !DILocation(line: 58, scope: !368)
!368 = distinct !DILexicalBlock(scope: !369, file: !2, line: 58)
!369 = distinct !DILexicalBlock(scope: !363, file: !2, line: 58)
!370 = !DILocation(line: 58, scope: !371)
!371 = distinct !DILexicalBlock(scope: !372, file: !2, line: 58)
!372 = distinct !DILexicalBlock(scope: !368, file: !2, line: 58)
!373 = !DILocalVariable(name: "width", scope: !305, file: !2, line: 60, type: !63)
!374 = !DILocation(line: 60, scope: !305)
!375 = !DILocation(line: 61, scope: !305)
!376 = !DILocation(line: 61, scope: !377)
!377 = distinct !DILexicalBlock(scope: !378, file: !2, line: 61)
!378 = distinct !DILexicalBlock(scope: !305, file: !2, line: 61)
!379 = !DILocation(line: 61, scope: !380)
!380 = distinct !DILexicalBlock(scope: !378, file: !2, line: 61)
!381 = !DILocation(line: 61, scope: !382)
!382 = distinct !DILexicalBlock(scope: !383, file: !2, line: 61)
!383 = distinct !DILexicalBlock(scope: !380, file: !2, line: 61)
!384 = !DILocation(line: 61, scope: !385)
!385 = distinct !DILexicalBlock(scope: !386, file: !2, line: 61)
!386 = distinct !DILexicalBlock(scope: !380, file: !2, line: 61)
!387 = !DILocation(line: 61, scope: !388)
!388 = distinct !DILexicalBlock(scope: !389, file: !2, line: 61)
!389 = distinct !DILexicalBlock(scope: !385, file: !2, line: 61)
!390 = !DILocalVariable(name: "cnt_a", scope: !305, file: !2, line: 63, type: !63)
!391 = !DILocation(line: 63, scope: !305)
!392 = !DILocalVariable(name: "ZETA_tmp_4", scope: !393, file: !2, line: 63, type: !63)
!393 = distinct !DILexicalBlock(scope: !305, file: !2, line: 63)
!394 = !DILocation(line: 63, scope: !393)
!395 = !DILocalVariable(name: "ZETA_tmp_5", scope: !393, file: !2, line: 63, type: !63)
!396 = !DILocalVariable(name: "cnt_b", scope: !305, file: !2, line: 64, type: !63)
!397 = !DILocation(line: 64, scope: !305)
!398 = !DILocalVariable(name: "ZETA_tmp_6", scope: !399, file: !2, line: 64, type: !63)
!399 = distinct !DILexicalBlock(scope: !305, file: !2, line: 64)
!400 = !DILocation(line: 64, scope: !399)
!401 = !DILocalVariable(name: "ZETA_tmp_7", scope: !399, file: !2, line: 64, type: !63)
!402 = !DILocalVariable(name: "cnt_c", scope: !305, file: !2, line: 65, type: !63)
!403 = !DILocation(line: 65, scope: !305)
!404 = !DILocalVariable(name: "l_i", scope: !305, file: !2, line: 67, type: !63)
!405 = !DILocation(line: 67, scope: !305)
!406 = !DILocalVariable(name: "r_i", scope: !305, file: !2, line: 68, type: !63)
!407 = !DILocation(line: 68, scope: !305)
!408 = !DILocalVariable(name: "l_cv_cursor", scope: !305, file: !2, line: 70, type: !254)
!409 = !DILocation(line: 70, scope: !305)
!410 = !DILocalVariable(name: "r_cv_cursor", scope: !305, file: !2, line: 71, type: !254)
!411 = !DILocation(line: 71, scope: !305)
!412 = !DILocation(line: 73, scope: !305)
!413 = !DILocation(line: 75, scope: !305)
!414 = !DILocalVariable(name: "i", scope: !415, file: !2, line: 77, type: !63)
!415 = distinct !DILexicalBlock(scope: !305, file: !2, line: 77)
!416 = !DILocation(line: 77, scope: !415)
!417 = !DILocation(line: 78, scope: !418)
!418 = distinct !DILexicalBlock(scope: !419, file: !2, line: 77)
!419 = distinct !DILexicalBlock(scope: !415, file: !2, line: 77)
!420 = !DILocation(line: 80, scope: !418)
!421 = !DILocation(line: 77, scope: !419)
!422 = distinct !{!422, !416, !423, !275}
!423 = !DILocation(line: 80, scope: !415)
!424 = !DILocation(line: 82, scope: !305)
!425 = !DILocalVariable(name: "i", scope: !426, file: !2, line: 84, type: !63)
!426 = distinct !DILexicalBlock(scope: !305, file: !2, line: 84)
!427 = !DILocation(line: 84, scope: !426)
!428 = !DILocation(line: 85, scope: !429)
!429 = distinct !DILexicalBlock(scope: !430, file: !2, line: 84)
!430 = distinct !DILexicalBlock(scope: !426, file: !2, line: 84)
!431 = !DILocation(line: 87, scope: !429)
!432 = !DILocation(line: 84, scope: !430)
!433 = distinct !{!433, !427, !434, !275}
!434 = !DILocation(line: 87, scope: !426)
!435 = !DILocation(line: 89, scope: !305)
!436 = !DILocation(line: 92, scope: !305)
!437 = !DILocation(line: 94, scope: !305)
!438 = !DILocation(line: 96, scope: !305)
!439 = !DILocation(line: 97, scope: !305)
!440 = distinct !DISubprogram(name: "_vsprintf_l", scope: !74, file: !74, line: 1449, type: !441, scopeLine: 1458, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !59, retainedNodes: !82)
!441 = !DISubroutineType(types: !442)
!442 = !{!77, !78, !80, !443, !90}
!443 = !DIDerivedType(tag: DW_TAG_const_type, baseType: !444)
!444 = !DIDerivedType(tag: DW_TAG_typedef, name: "_locale_t", file: !445, line: 623, baseType: !446)
!445 = !DIFile(filename: "C:\\Program Files (x86)\\Windows Kits\\10\\Include\\10.0.22621.0\\ucrt\\corecrt.h", directory: "", checksumkind: CSK_MD5, checksum: "4ce81db8e96f94c79f8dce86dd46b97f")
!446 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !447, size: 64)
!447 = !DIDerivedType(tag: DW_TAG_typedef, name: "__crt_locale_pointers", file: !445, line: 621, baseType: !448)
!448 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "__crt_locale_pointers", file: !445, line: 617, size: 128, elements: !449)
!449 = !{!450, !453}
!450 = !DIDerivedType(tag: DW_TAG_member, name: "locinfo", scope: !448, file: !445, line: 619, baseType: !451, size: 64)
!451 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !452, size: 64)
!452 = !DICompositeType(tag: DW_TAG_structure_type, name: "__crt_locale_data", file: !445, line: 619, flags: DIFlagFwdDecl)
!453 = !DIDerivedType(tag: DW_TAG_member, name: "mbcinfo", scope: !448, file: !445, line: 620, baseType: !454, size: 64, offset: 64)
!454 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !455, size: 64)
!455 = !DICompositeType(tag: DW_TAG_structure_type, name: "__crt_multibyte_data", file: !445, line: 620, flags: DIFlagFwdDecl)
!456 = !DILocalVariable(name: "_ArgList", arg: 4, scope: !440, file: !74, line: 1453, type: !90)
!457 = !DILocation(line: 1453, scope: !440)
!458 = !DILocalVariable(name: "_Locale", arg: 3, scope: !440, file: !74, line: 1452, type: !443)
!459 = !DILocation(line: 1452, scope: !440)
!460 = !DILocalVariable(name: "_Format", arg: 2, scope: !440, file: !74, line: 1451, type: !80)
!461 = !DILocation(line: 1451, scope: !440)
!462 = !DILocalVariable(name: "_Buffer", arg: 1, scope: !440, file: !74, line: 1450, type: !78)
!463 = !DILocation(line: 1450, scope: !440)
!464 = !DILocation(line: 1459, scope: !440)
!465 = distinct !DISubprogram(name: "_vsnprintf_l", scope: !74, file: !74, line: 1381, type: !466, scopeLine: 1391, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !59, retainedNodes: !82)
!466 = !DISubroutineType(types: !467)
!467 = !{!77, !78, !110, !80, !443, !90}
!468 = !DILocalVariable(name: "_ArgList", arg: 5, scope: !465, file: !74, line: 1386, type: !90)
!469 = !DILocation(line: 1386, scope: !465)
!470 = !DILocalVariable(name: "_Locale", arg: 4, scope: !465, file: !74, line: 1385, type: !443)
!471 = !DILocation(line: 1385, scope: !465)
!472 = !DILocalVariable(name: "_Format", arg: 3, scope: !465, file: !74, line: 1384, type: !80)
!473 = !DILocation(line: 1384, scope: !465)
!474 = !DILocalVariable(name: "_BufferCount", arg: 2, scope: !465, file: !74, line: 1383, type: !110)
!475 = !DILocation(line: 1383, scope: !465)
!476 = !DILocalVariable(name: "_Buffer", arg: 1, scope: !465, file: !74, line: 1382, type: !78)
!477 = !DILocation(line: 1382, scope: !465)
!478 = !DILocalVariable(name: "_Result", scope: !465, file: !74, line: 1392, type: !479)
!479 = !DIDerivedType(tag: DW_TAG_const_type, baseType: !77)
!480 = !DILocation(line: 1392, scope: !465)
!481 = !DILocation(line: 1396, scope: !465)
!482 = !DILocation(line: 92, scope: !53)
!483 = distinct !DISubprogram(name: "_vfprintf_l", scope: !74, file: !74, line: 635, type: !484, scopeLine: 644, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !59, retainedNodes: !82)
!484 = !DISubroutineType(types: !485)
!485 = !{!77, !486, !80, !443, !90}
!486 = !DIDerivedType(tag: DW_TAG_const_type, baseType: !487)
!487 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !488, size: 64)
!488 = !DIDerivedType(tag: DW_TAG_typedef, name: "FILE", file: !489, line: 31, baseType: !490)
!489 = !DIFile(filename: "C:\\Program Files (x86)\\Windows Kits\\10\\Include\\10.0.22621.0\\ucrt\\corecrt_wstdio.h", directory: "", checksumkind: CSK_MD5, checksum: "bf50373b435d0afd0235dd3e05c4a277")
!490 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "_iobuf", file: !489, line: 28, size: 64, elements: !491)
!491 = !{!492}
!492 = !DIDerivedType(tag: DW_TAG_member, name: "_Placeholder", scope: !490, file: !489, line: 30, baseType: !62, size: 64)
!493 = !DILocalVariable(name: "_ArgList", arg: 4, scope: !483, file: !74, line: 639, type: !90)
!494 = !DILocation(line: 639, scope: !483)
!495 = !DILocalVariable(name: "_Locale", arg: 3, scope: !483, file: !74, line: 638, type: !443)
!496 = !DILocation(line: 638, scope: !483)
!497 = !DILocalVariable(name: "_Format", arg: 2, scope: !483, file: !74, line: 637, type: !80)
!498 = !DILocation(line: 637, scope: !483)
!499 = !DILocalVariable(name: "_Stream", arg: 1, scope: !483, file: !74, line: 636, type: !486)
!500 = !DILocation(line: 636, scope: !483)
!501 = !DILocation(line: 645, scope: !483)
