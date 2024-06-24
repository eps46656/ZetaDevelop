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

$"??_C@_0BA@LFLILLAE@?$CF48s?3?$CF?94d?7?$CF?924s?$AA@" = comdat any

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
@"??_C@_0BA@LFLILLAE@?$CF48s?3?$CF?94d?7?$CF?924s?$AA@" = linkonce_odr dso_local unnamed_addr constant [16 x i8] c"%48s:%-4d\09%-24s\00", comdat, align 1, !dbg !13
@"??_C@_0BD@LBCLNPNL@l_cv?5?$CB?$DN?5?$CI?$CIvoid?$CK?$CJ0?$CJ?$AA@" = linkonce_odr dso_local unnamed_addr constant [19 x i8] c"l_cv != ((void*)0)\00", comdat, align 1, !dbg !18
@"??_C@_0M@BJGCKIIG@?7assert?7?$CFs?6?$AA@" = linkonce_odr dso_local unnamed_addr constant [12 x i8] c"\09assert\09%s\0A\00", comdat, align 1, !dbg !23
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
  br label %35, !dbg !159

30:                                               ; preds = %5
  %31 = call i32 (ptr, ...) @printf(ptr noundef @"??_C@_0BA@LFLILLAE@?$CF48s?3?$CF?94d?7?$CF?924s?$AA@", ptr noundef @"??_C@_0BP@DOHEAJCL@D?3?1ZetaDevelop?1Zeta?1SegUtils?4c?$AA@", i32 noundef 7, ptr noundef @"??_C@_09DDPAKPII@SegShoveL?$AA@"), !dbg !162
  %32 = call i32 (ptr, ...) @printf(ptr noundef @"??_C@_0M@BJGCKIIG@?7assert?7?$CFs?6?$AA@", ptr noundef @"??_C@_0BD@LBCLNPNL@l_cv?5?$CB?$DN?5?$CI?$CIvoid?$CK?$CJ0?$CJ?$AA@"), !dbg !162
  %33 = call ptr @__acrt_iob_func(i32 noundef 1), !dbg !162
  %34 = call i32 @fflush(ptr noundef %33), !dbg !162
  call void @exit(i32 noundef 0) #5, !dbg !162
  unreachable, !dbg !162

35:                                               ; preds = %29
  %36 = load ptr, ptr %9, align 8, !dbg !164
  %37 = icmp ne ptr %36, null, !dbg !164
  br i1 %37, label %38, label %39, !dbg !164

38:                                               ; preds = %35
  br label %44, !dbg !165

39:                                               ; preds = %35
  %40 = call i32 (ptr, ...) @printf(ptr noundef @"??_C@_0BA@LFLILLAE@?$CF48s?3?$CF?94d?7?$CF?924s?$AA@", ptr noundef @"??_C@_0BP@DOHEAJCL@D?3?1ZetaDevelop?1Zeta?1SegUtils?4c?$AA@", i32 noundef 8, ptr noundef @"??_C@_09DDPAKPII@SegShoveL?$AA@"), !dbg !168
  %41 = call i32 (ptr, ...) @printf(ptr noundef @"??_C@_0M@BJGCKIIG@?7assert?7?$CFs?6?$AA@", ptr noundef @"??_C@_0BD@DGBEPCEH@r_cv?5?$CB?$DN?5?$CI?$CIvoid?$CK?$CJ0?$CJ?$AA@"), !dbg !168
  %42 = call ptr @__acrt_iob_func(i32 noundef 1), !dbg !168
  %43 = call i32 @fflush(ptr noundef %42), !dbg !168
  call void @exit(i32 noundef 0) #5, !dbg !168
  unreachable, !dbg !168

44:                                               ; preds = %38
  %45 = load ptr, ptr %10, align 8, !dbg !170
  %46 = getelementptr inbounds %struct.Zeta_CircularVector, ptr %45, i32 0, i32 4, !dbg !170
  %47 = load i64, ptr %46, align 8, !dbg !170
  %48 = load i64, ptr %6, align 8, !dbg !170
  %49 = add i64 %47, %48, !dbg !170
  %50 = load ptr, ptr %10, align 8, !dbg !170
  %51 = getelementptr inbounds %struct.Zeta_CircularVector, ptr %50, i32 0, i32 5, !dbg !170
  %52 = load i64, ptr %51, align 8, !dbg !170
  %53 = icmp ule i64 %49, %52, !dbg !170
  br i1 %53, label %54, label %55, !dbg !170

54:                                               ; preds = %44
  br label %60, !dbg !171

55:                                               ; preds = %44
  %56 = call i32 (ptr, ...) @printf(ptr noundef @"??_C@_0BA@LFLILLAE@?$CF48s?3?$CF?94d?7?$CF?924s?$AA@", ptr noundef @"??_C@_0BP@DOHEAJCL@D?3?1ZetaDevelop?1Zeta?1SegUtils?4c?$AA@", i32 noundef 10, ptr noundef @"??_C@_09DDPAKPII@SegShoveL?$AA@"), !dbg !174
  %57 = call i32 (ptr, ...) @printf(ptr noundef @"??_C@_0M@BJGCKIIG@?7assert?7?$CFs?6?$AA@", ptr noundef @"??_C@_0CJ@EGMKBOBE@l_cv?9?$DOsize?5?$CL?5shove_cnt?5?$DM?$DN?5l_cv?9?$DO@"), !dbg !174
  %58 = call ptr @__acrt_iob_func(i32 noundef 1), !dbg !174
  %59 = call i32 @fflush(ptr noundef %58), !dbg !174
  call void @exit(i32 noundef 0) #5, !dbg !174
  unreachable, !dbg !174

60:                                               ; preds = %54
  %61 = load i64, ptr %6, align 8, !dbg !176
  %62 = load ptr, ptr %9, align 8, !dbg !176
  %63 = getelementptr inbounds %struct.Zeta_CircularVector, ptr %62, i32 0, i32 4, !dbg !176
  %64 = load i64, ptr %63, align 8, !dbg !176
  %65 = load i64, ptr %7, align 8, !dbg !176
  %66 = add i64 %64, %65, !dbg !176
  %67 = icmp ule i64 %61, %66, !dbg !176
  br i1 %67, label %68, label %69, !dbg !176

68:                                               ; preds = %60
  br label %74, !dbg !177

69:                                               ; preds = %60
  %70 = call i32 (ptr, ...) @printf(ptr noundef @"??_C@_0BA@LFLILLAE@?$CF48s?3?$CF?94d?7?$CF?924s?$AA@", ptr noundef @"??_C@_0BP@DOHEAJCL@D?3?1ZetaDevelop?1Zeta?1SegUtils?4c?$AA@", i32 noundef 11, ptr noundef @"??_C@_09DDPAKPII@SegShoveL?$AA@"), !dbg !180
  %71 = call i32 (ptr, ...) @printf(ptr noundef @"??_C@_0M@BJGCKIIG@?7assert?7?$CFs?6?$AA@", ptr noundef @"??_C@_0CC@OIMDNCHI@shove_cnt?5?$DM?$DN?5r_cv?9?$DOsize?5?$CL?5ins_cn@"), !dbg !180
  %72 = call ptr @__acrt_iob_func(i32 noundef 1), !dbg !180
  %73 = call i32 @fflush(ptr noundef %72), !dbg !180
  call void @exit(i32 noundef 0) #5, !dbg !180
  unreachable, !dbg !180

74:                                               ; preds = %68
  call void @llvm.dbg.declare(metadata ptr %11, metadata !182, metadata !DIExpression()), !dbg !183
  %75 = load ptr, ptr %10, align 8, !dbg !183
  %76 = getelementptr inbounds %struct.Zeta_CircularVector, ptr %75, i32 0, i32 1, !dbg !183
  %77 = load i64, ptr %76, align 8, !dbg !183
  store i64 %77, ptr %11, align 8, !dbg !183
  %78 = load i64, ptr %11, align 8, !dbg !184
  %79 = load ptr, ptr %9, align 8, !dbg !184
  %80 = getelementptr inbounds %struct.Zeta_CircularVector, ptr %79, i32 0, i32 1, !dbg !184
  %81 = load i64, ptr %80, align 8, !dbg !184
  %82 = icmp eq i64 %78, %81, !dbg !184
  br i1 %82, label %83, label %84, !dbg !184

83:                                               ; preds = %74
  br label %89, !dbg !185

84:                                               ; preds = %74
  %85 = call i32 (ptr, ...) @printf(ptr noundef @"??_C@_0BA@LFLILLAE@?$CF48s?3?$CF?94d?7?$CF?924s?$AA@", ptr noundef @"??_C@_0BP@DOHEAJCL@D?3?1ZetaDevelop?1Zeta?1SegUtils?4c?$AA@", i32 noundef 14, ptr noundef @"??_C@_09DDPAKPII@SegShoveL?$AA@"), !dbg !188
  %86 = call i32 (ptr, ...) @printf(ptr noundef @"??_C@_0M@BJGCKIIG@?7assert?7?$CFs?6?$AA@", ptr noundef @"??_C@_0BF@LNJADDKF@width?5?$DN?$DN?5r_cv?9?$DOwidth?$AA@"), !dbg !188
  %87 = call ptr @__acrt_iob_func(i32 noundef 1), !dbg !188
  %88 = call i32 @fflush(ptr noundef %87), !dbg !188
  call void @exit(i32 noundef 0) #5, !dbg !188
  unreachable, !dbg !188

89:                                               ; preds = %83
  call void @llvm.dbg.declare(metadata ptr %12, metadata !190, metadata !DIExpression()), !dbg !191
  call void @llvm.dbg.declare(metadata ptr %13, metadata !192, metadata !DIExpression()), !dbg !194
  %90 = load i64, ptr %8, align 8, !dbg !194
  store i64 %90, ptr %13, align 8, !dbg !194
  call void @llvm.dbg.declare(metadata ptr %14, metadata !195, metadata !DIExpression()), !dbg !194
  %91 = load i64, ptr %6, align 8, !dbg !194
  store i64 %91, ptr %14, align 8, !dbg !194
  %92 = load i64, ptr %13, align 8, !dbg !194
  %93 = load i64, ptr %14, align 8, !dbg !194
  %94 = icmp ult i64 %92, %93, !dbg !194
  br i1 %94, label %95, label %97, !dbg !194

95:                                               ; preds = %89
  %96 = load i64, ptr %13, align 8, !dbg !194
  br label %99, !dbg !194

97:                                               ; preds = %89
  %98 = load i64, ptr %14, align 8, !dbg !194
  br label %99, !dbg !194

99:                                               ; preds = %97, %95
  %100 = phi i64 [ %96, %95 ], [ %98, %97 ], !dbg !194
  store i64 %100, ptr %15, align 8, !dbg !194
  %101 = load i64, ptr %15, align 8, !dbg !194
  store i64 %101, ptr %12, align 8, !dbg !191
  call void @llvm.dbg.declare(metadata ptr %16, metadata !196, metadata !DIExpression()), !dbg !197
  call void @llvm.dbg.declare(metadata ptr %17, metadata !198, metadata !DIExpression()), !dbg !200
  %102 = load i64, ptr %7, align 8, !dbg !200
  store i64 %102, ptr %17, align 8, !dbg !200
  call void @llvm.dbg.declare(metadata ptr %18, metadata !201, metadata !DIExpression()), !dbg !200
  %103 = load i64, ptr %6, align 8, !dbg !200
  %104 = load i64, ptr %12, align 8, !dbg !200
  %105 = sub i64 %103, %104, !dbg !200
  store i64 %105, ptr %18, align 8, !dbg !200
  %106 = load i64, ptr %17, align 8, !dbg !200
  %107 = load i64, ptr %18, align 8, !dbg !200
  %108 = icmp ult i64 %106, %107, !dbg !200
  br i1 %108, label %109, label %111, !dbg !200

109:                                              ; preds = %99
  %110 = load i64, ptr %17, align 8, !dbg !200
  br label %113, !dbg !200

111:                                              ; preds = %99
  %112 = load i64, ptr %18, align 8, !dbg !200
  br label %113, !dbg !200

113:                                              ; preds = %111, %109
  %114 = phi i64 [ %110, %109 ], [ %112, %111 ], !dbg !200
  store i64 %114, ptr %19, align 8, !dbg !200
  %115 = load i64, ptr %19, align 8, !dbg !200
  store i64 %115, ptr %16, align 8, !dbg !197
  call void @llvm.dbg.declare(metadata ptr %20, metadata !202, metadata !DIExpression()), !dbg !203
  %116 = load i64, ptr %6, align 8, !dbg !203
  %117 = load i64, ptr %12, align 8, !dbg !203
  %118 = sub i64 %116, %117, !dbg !203
  %119 = load i64, ptr %16, align 8, !dbg !203
  %120 = sub i64 %118, %119, !dbg !203
  store i64 %120, ptr %20, align 8, !dbg !203
  call void @llvm.dbg.declare(metadata ptr %21, metadata !204, metadata !DIExpression()), !dbg !205
  %121 = load ptr, ptr %10, align 8, !dbg !205
  %122 = getelementptr inbounds %struct.Zeta_CircularVector, ptr %121, i32 0, i32 4, !dbg !205
  %123 = load i64, ptr %122, align 8, !dbg !205
  store i64 %123, ptr %21, align 8, !dbg !205
  call void @llvm.dbg.declare(metadata ptr %22, metadata !206, metadata !DIExpression()), !dbg !207
  store i64 0, ptr %22, align 8, !dbg !207
  call void @llvm.dbg.declare(metadata ptr %23, metadata !208, metadata !DIExpression()), !dbg !215
  call void @llvm.dbg.declare(metadata ptr %24, metadata !216, metadata !DIExpression()), !dbg !217
  %124 = load ptr, ptr %10, align 8, !dbg !218
  call void @Zeta_CircularVector_GetRBCursor(ptr noundef %124, ptr noundef %23), !dbg !218
  %125 = load i64, ptr %6, align 8, !dbg !219
  %126 = load ptr, ptr %10, align 8, !dbg !219
  %127 = call ptr @Zeta_CircularVector_Insert(ptr noundef %126, ptr noundef %23, i64 noundef %125), !dbg !219
  call void @llvm.dbg.declare(metadata ptr %25, metadata !220, metadata !DIExpression()), !dbg !222
  %128 = load i64, ptr %12, align 8, !dbg !222
  store i64 %128, ptr %25, align 8, !dbg !222
  br label %129, !dbg !222

129:                                              ; preds = %141, %113
  %130 = load i64, ptr %25, align 8, !dbg !222
  %131 = add i64 %130, -1, !dbg !222
  store i64 %131, ptr %25, align 8, !dbg !222
  %132 = icmp ult i64 0, %130, !dbg !222
  br i1 %132, label %133, label %146, !dbg !222

133:                                              ; preds = %129
  %134 = load i64, ptr %11, align 8, !dbg !223
  %135 = load i64, ptr %22, align 8, !dbg !223
  %136 = load ptr, ptr %9, align 8, !dbg !223
  %137 = call ptr @Zeta_CircularVector_Access(ptr noundef %136, ptr noundef null, ptr noundef null, i64 noundef %135), !dbg !223
  %138 = load i64, ptr %21, align 8, !dbg !223
  %139 = load ptr, ptr %10, align 8, !dbg !223
  %140 = call ptr @Zeta_CircularVector_Access(ptr noundef %139, ptr noundef null, ptr noundef null, i64 noundef %138), !dbg !223
  call void @Zeta_MemCopy(ptr noundef %140, ptr noundef %137, i64 noundef %134), !dbg !223
  br label %141, !dbg !226

141:                                              ; preds = %133
  %142 = load i64, ptr %21, align 8, !dbg !227
  %143 = add i64 %142, 1, !dbg !227
  store i64 %143, ptr %21, align 8, !dbg !227
  %144 = load i64, ptr %22, align 8, !dbg !227
  %145 = add i64 %144, 1, !dbg !227
  store i64 %145, ptr %22, align 8, !dbg !227
  br label %129, !dbg !227, !llvm.loop !228

146:                                              ; preds = %129
  %147 = load i64, ptr %16, align 8, !dbg !231
  %148 = load i64, ptr %21, align 8, !dbg !231
  %149 = add i64 %148, %147, !dbg !231
  store i64 %149, ptr %21, align 8, !dbg !231
  call void @llvm.dbg.declare(metadata ptr %26, metadata !232, metadata !DIExpression()), !dbg !234
  %150 = load i64, ptr %20, align 8, !dbg !234
  store i64 %150, ptr %26, align 8, !dbg !234
  br label %151, !dbg !234

151:                                              ; preds = %163, %146
  %152 = load i64, ptr %26, align 8, !dbg !234
  %153 = add i64 %152, -1, !dbg !234
  store i64 %153, ptr %26, align 8, !dbg !234
  %154 = icmp ult i64 0, %152, !dbg !234
  br i1 %154, label %155, label %168, !dbg !234

155:                                              ; preds = %151
  %156 = load i64, ptr %11, align 8, !dbg !235
  %157 = load i64, ptr %22, align 8, !dbg !235
  %158 = load ptr, ptr %9, align 8, !dbg !235
  %159 = call ptr @Zeta_CircularVector_Access(ptr noundef %158, ptr noundef null, ptr noundef null, i64 noundef %157), !dbg !235
  %160 = load i64, ptr %21, align 8, !dbg !235
  %161 = load ptr, ptr %10, align 8, !dbg !235
  %162 = call ptr @Zeta_CircularVector_Access(ptr noundef %161, ptr noundef null, ptr noundef null, i64 noundef %160), !dbg !235
  call void @Zeta_MemCopy(ptr noundef %162, ptr noundef %159, i64 noundef %156), !dbg !235
  br label %163, !dbg !238

163:                                              ; preds = %155
  %164 = load i64, ptr %21, align 8, !dbg !239
  %165 = add i64 %164, 1, !dbg !239
  store i64 %165, ptr %21, align 8, !dbg !239
  %166 = load i64, ptr %22, align 8, !dbg !239
  %167 = add i64 %166, 1, !dbg !239
  store i64 %167, ptr %22, align 8, !dbg !239
  br label %151, !dbg !239, !llvm.loop !240

168:                                              ; preds = %151
  %169 = load ptr, ptr %9, align 8, !dbg !242
  %170 = call ptr @Zeta_CircularVector_PeekL(ptr noundef %169, ptr noundef %24, ptr noundef null), !dbg !242
  %171 = load i64, ptr %12, align 8, !dbg !243
  %172 = load i64, ptr %20, align 8, !dbg !243
  %173 = add i64 %171, %172, !dbg !243
  %174 = load ptr, ptr %9, align 8, !dbg !243
  call void @Zeta_CircularVector_Erase(ptr noundef %174, ptr noundef %24, i64 noundef %173), !dbg !243
  %175 = load i64, ptr %8, align 8, !dbg !244
  %176 = load i64, ptr %12, align 8, !dbg !244
  %177 = sub i64 %175, %176, !dbg !244
  %178 = load ptr, ptr %9, align 8, !dbg !244
  call void @Zeta_CircularVector_Cursor_AdvanceR(ptr noundef %178, ptr noundef %24, i64 noundef %177), !dbg !244
  %179 = load i64, ptr %7, align 8, !dbg !245
  %180 = load i64, ptr %16, align 8, !dbg !245
  %181 = sub i64 %179, %180, !dbg !245
  %182 = load ptr, ptr %9, align 8, !dbg !245
  %183 = call ptr @Zeta_CircularVector_Insert(ptr noundef %182, ptr noundef %24, i64 noundef %181), !dbg !245
  ret void, !dbg !246
}

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr dso_local i32 @printf(ptr noundef %0, ...) #0 comdat !dbg !247 {
  %2 = alloca ptr, align 8
  %3 = alloca i32, align 4
  %4 = alloca ptr, align 8
  store ptr %0, ptr %2, align 8
  call void @llvm.dbg.declare(metadata ptr %2, metadata !250, metadata !DIExpression()), !dbg !251
  call void @llvm.dbg.declare(metadata ptr %3, metadata !252, metadata !DIExpression()), !dbg !253
  call void @llvm.dbg.declare(metadata ptr %4, metadata !254, metadata !DIExpression()), !dbg !255
  call void @llvm.va_start(ptr %4), !dbg !256
  %5 = load ptr, ptr %4, align 8, !dbg !257
  %6 = load ptr, ptr %2, align 8, !dbg !257
  %7 = call ptr @__acrt_iob_func(i32 noundef 1), !dbg !257
  %8 = call i32 @_vfprintf_l(ptr noundef %7, ptr noundef %6, ptr noundef null, ptr noundef %5), !dbg !257
  store i32 %8, ptr %3, align 4, !dbg !257
  call void @llvm.va_end(ptr %4), !dbg !258
  %9 = load i32, ptr %3, align 4, !dbg !259
  ret i32 %9, !dbg !259
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
define dso_local void @SegShoveR(ptr noundef %0, ptr noundef %1, i64 noundef %2, i64 noundef %3, i64 noundef %4) #0 !dbg !260 {
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
  call void @llvm.dbg.declare(metadata ptr %6, metadata !261, metadata !DIExpression()), !dbg !262
  store i64 %3, ptr %7, align 8
  call void @llvm.dbg.declare(metadata ptr %7, metadata !263, metadata !DIExpression()), !dbg !262
  store i64 %2, ptr %8, align 8
  call void @llvm.dbg.declare(metadata ptr %8, metadata !264, metadata !DIExpression()), !dbg !262
  store ptr %1, ptr %9, align 8
  call void @llvm.dbg.declare(metadata ptr %9, metadata !265, metadata !DIExpression()), !dbg !266
  store ptr %0, ptr %10, align 8
  call void @llvm.dbg.declare(metadata ptr %10, metadata !267, metadata !DIExpression()), !dbg !266
  %27 = load ptr, ptr %10, align 8, !dbg !268
  %28 = icmp ne ptr %27, null, !dbg !268
  br i1 %28, label %29, label %30, !dbg !268

29:                                               ; preds = %5
  br label %35, !dbg !269

30:                                               ; preds = %5
  %31 = call i32 (ptr, ...) @printf(ptr noundef @"??_C@_0BA@LFLILLAE@?$CF48s?3?$CF?94d?7?$CF?924s?$AA@", ptr noundef @"??_C@_0BP@DOHEAJCL@D?3?1ZetaDevelop?1Zeta?1SegUtils?4c?$AA@", i32 noundef 53, ptr noundef @"??_C@_09OHLBJAFH@SegShoveR?$AA@"), !dbg !272
  %32 = call i32 (ptr, ...) @printf(ptr noundef @"??_C@_0M@BJGCKIIG@?7assert?7?$CFs?6?$AA@", ptr noundef @"??_C@_0BD@LBCLNPNL@l_cv?5?$CB?$DN?5?$CI?$CIvoid?$CK?$CJ0?$CJ?$AA@"), !dbg !272
  %33 = call ptr @__acrt_iob_func(i32 noundef 1), !dbg !272
  %34 = call i32 @fflush(ptr noundef %33), !dbg !272
  call void @exit(i32 noundef 0) #5, !dbg !272
  unreachable, !dbg !272

35:                                               ; preds = %29
  %36 = load ptr, ptr %9, align 8, !dbg !274
  %37 = icmp ne ptr %36, null, !dbg !274
  br i1 %37, label %38, label %39, !dbg !274

38:                                               ; preds = %35
  br label %44, !dbg !275

39:                                               ; preds = %35
  %40 = call i32 (ptr, ...) @printf(ptr noundef @"??_C@_0BA@LFLILLAE@?$CF48s?3?$CF?94d?7?$CF?924s?$AA@", ptr noundef @"??_C@_0BP@DOHEAJCL@D?3?1ZetaDevelop?1Zeta?1SegUtils?4c?$AA@", i32 noundef 54, ptr noundef @"??_C@_09OHLBJAFH@SegShoveR?$AA@"), !dbg !278
  %41 = call i32 (ptr, ...) @printf(ptr noundef @"??_C@_0M@BJGCKIIG@?7assert?7?$CFs?6?$AA@", ptr noundef @"??_C@_0BD@DGBEPCEH@r_cv?5?$CB?$DN?5?$CI?$CIvoid?$CK?$CJ0?$CJ?$AA@"), !dbg !278
  %42 = call ptr @__acrt_iob_func(i32 noundef 1), !dbg !278
  %43 = call i32 @fflush(ptr noundef %42), !dbg !278
  call void @exit(i32 noundef 0) #5, !dbg !278
  unreachable, !dbg !278

44:                                               ; preds = %38
  %45 = load ptr, ptr %9, align 8, !dbg !280
  %46 = getelementptr inbounds %struct.Zeta_CircularVector, ptr %45, i32 0, i32 4, !dbg !280
  %47 = load i64, ptr %46, align 8, !dbg !280
  %48 = load i64, ptr %6, align 8, !dbg !280
  %49 = add i64 %47, %48, !dbg !280
  %50 = load ptr, ptr %10, align 8, !dbg !280
  %51 = getelementptr inbounds %struct.Zeta_CircularVector, ptr %50, i32 0, i32 5, !dbg !280
  %52 = load i64, ptr %51, align 8, !dbg !280
  %53 = icmp ule i64 %49, %52, !dbg !280
  br i1 %53, label %54, label %55, !dbg !280

54:                                               ; preds = %44
  br label %60, !dbg !281

55:                                               ; preds = %44
  %56 = call i32 (ptr, ...) @printf(ptr noundef @"??_C@_0BA@LFLILLAE@?$CF48s?3?$CF?94d?7?$CF?924s?$AA@", ptr noundef @"??_C@_0BP@DOHEAJCL@D?3?1ZetaDevelop?1Zeta?1SegUtils?4c?$AA@", i32 noundef 56, ptr noundef @"??_C@_09OHLBJAFH@SegShoveR?$AA@"), !dbg !284
  %57 = call i32 (ptr, ...) @printf(ptr noundef @"??_C@_0M@BJGCKIIG@?7assert?7?$CFs?6?$AA@", ptr noundef @"??_C@_0CJ@CMKBNDAF@r_cv?9?$DOsize?5?$CL?5shove_cnt?5?$DM?$DN?5l_cv?9?$DO@"), !dbg !284
  %58 = call ptr @__acrt_iob_func(i32 noundef 1), !dbg !284
  %59 = call i32 @fflush(ptr noundef %58), !dbg !284
  call void @exit(i32 noundef 0) #5, !dbg !284
  unreachable, !dbg !284

60:                                               ; preds = %54
  %61 = load i64, ptr %6, align 8, !dbg !286
  %62 = load ptr, ptr %10, align 8, !dbg !286
  %63 = getelementptr inbounds %struct.Zeta_CircularVector, ptr %62, i32 0, i32 4, !dbg !286
  %64 = load i64, ptr %63, align 8, !dbg !286
  %65 = load i64, ptr %7, align 8, !dbg !286
  %66 = add i64 %64, %65, !dbg !286
  %67 = icmp ule i64 %61, %66, !dbg !286
  br i1 %67, label %68, label %69, !dbg !286

68:                                               ; preds = %60
  br label %74, !dbg !287

69:                                               ; preds = %60
  %70 = call i32 (ptr, ...) @printf(ptr noundef @"??_C@_0BA@LFLILLAE@?$CF48s?3?$CF?94d?7?$CF?924s?$AA@", ptr noundef @"??_C@_0BP@DOHEAJCL@D?3?1ZetaDevelop?1Zeta?1SegUtils?4c?$AA@", i32 noundef 57, ptr noundef @"??_C@_09OHLBJAFH@SegShoveR?$AA@"), !dbg !290
  %71 = call i32 (ptr, ...) @printf(ptr noundef @"??_C@_0M@BJGCKIIG@?7assert?7?$CFs?6?$AA@", ptr noundef @"??_C@_0CC@HAOIMMCE@shove_cnt?5?$DM?$DN?5l_cv?9?$DOsize?5?$CL?5ins_cn@"), !dbg !290
  %72 = call ptr @__acrt_iob_func(i32 noundef 1), !dbg !290
  %73 = call i32 @fflush(ptr noundef %72), !dbg !290
  call void @exit(i32 noundef 0) #5, !dbg !290
  unreachable, !dbg !290

74:                                               ; preds = %68
  call void @llvm.dbg.declare(metadata ptr %11, metadata !292, metadata !DIExpression()), !dbg !293
  %75 = load ptr, ptr %10, align 8, !dbg !293
  %76 = getelementptr inbounds %struct.Zeta_CircularVector, ptr %75, i32 0, i32 1, !dbg !293
  %77 = load i64, ptr %76, align 8, !dbg !293
  store i64 %77, ptr %11, align 8, !dbg !293
  %78 = load i64, ptr %11, align 8, !dbg !294
  %79 = load ptr, ptr %9, align 8, !dbg !294
  %80 = getelementptr inbounds %struct.Zeta_CircularVector, ptr %79, i32 0, i32 1, !dbg !294
  %81 = load i64, ptr %80, align 8, !dbg !294
  %82 = icmp eq i64 %78, %81, !dbg !294
  br i1 %82, label %83, label %84, !dbg !294

83:                                               ; preds = %74
  br label %89, !dbg !295

84:                                               ; preds = %74
  %85 = call i32 (ptr, ...) @printf(ptr noundef @"??_C@_0BA@LFLILLAE@?$CF48s?3?$CF?94d?7?$CF?924s?$AA@", ptr noundef @"??_C@_0BP@DOHEAJCL@D?3?1ZetaDevelop?1Zeta?1SegUtils?4c?$AA@", i32 noundef 60, ptr noundef @"??_C@_09OHLBJAFH@SegShoveR?$AA@"), !dbg !298
  %86 = call i32 (ptr, ...) @printf(ptr noundef @"??_C@_0M@BJGCKIIG@?7assert?7?$CFs?6?$AA@", ptr noundef @"??_C@_0BF@LNJADDKF@width?5?$DN?$DN?5r_cv?9?$DOwidth?$AA@"), !dbg !298
  %87 = call ptr @__acrt_iob_func(i32 noundef 1), !dbg !298
  %88 = call i32 @fflush(ptr noundef %87), !dbg !298
  call void @exit(i32 noundef 0) #5, !dbg !298
  unreachable, !dbg !298

89:                                               ; preds = %83
  call void @llvm.dbg.declare(metadata ptr %12, metadata !300, metadata !DIExpression()), !dbg !301
  call void @llvm.dbg.declare(metadata ptr %13, metadata !302, metadata !DIExpression()), !dbg !304
  %90 = load i64, ptr %8, align 8, !dbg !304
  store i64 %90, ptr %13, align 8, !dbg !304
  call void @llvm.dbg.declare(metadata ptr %14, metadata !305, metadata !DIExpression()), !dbg !304
  %91 = load i64, ptr %6, align 8, !dbg !304
  store i64 %91, ptr %14, align 8, !dbg !304
  %92 = load i64, ptr %13, align 8, !dbg !304
  %93 = load i64, ptr %14, align 8, !dbg !304
  %94 = icmp ult i64 %92, %93, !dbg !304
  br i1 %94, label %95, label %97, !dbg !304

95:                                               ; preds = %89
  %96 = load i64, ptr %13, align 8, !dbg !304
  br label %99, !dbg !304

97:                                               ; preds = %89
  %98 = load i64, ptr %14, align 8, !dbg !304
  br label %99, !dbg !304

99:                                               ; preds = %97, %95
  %100 = phi i64 [ %96, %95 ], [ %98, %97 ], !dbg !304
  store i64 %100, ptr %15, align 8, !dbg !304
  %101 = load i64, ptr %15, align 8, !dbg !304
  store i64 %101, ptr %12, align 8, !dbg !301
  call void @llvm.dbg.declare(metadata ptr %16, metadata !306, metadata !DIExpression()), !dbg !307
  call void @llvm.dbg.declare(metadata ptr %17, metadata !308, metadata !DIExpression()), !dbg !310
  %102 = load i64, ptr %7, align 8, !dbg !310
  store i64 %102, ptr %17, align 8, !dbg !310
  call void @llvm.dbg.declare(metadata ptr %18, metadata !311, metadata !DIExpression()), !dbg !310
  %103 = load i64, ptr %6, align 8, !dbg !310
  %104 = load i64, ptr %12, align 8, !dbg !310
  %105 = sub i64 %103, %104, !dbg !310
  store i64 %105, ptr %18, align 8, !dbg !310
  %106 = load i64, ptr %17, align 8, !dbg !310
  %107 = load i64, ptr %18, align 8, !dbg !310
  %108 = icmp ult i64 %106, %107, !dbg !310
  br i1 %108, label %109, label %111, !dbg !310

109:                                              ; preds = %99
  %110 = load i64, ptr %17, align 8, !dbg !310
  br label %113, !dbg !310

111:                                              ; preds = %99
  %112 = load i64, ptr %18, align 8, !dbg !310
  br label %113, !dbg !310

113:                                              ; preds = %111, %109
  %114 = phi i64 [ %110, %109 ], [ %112, %111 ], !dbg !310
  store i64 %114, ptr %19, align 8, !dbg !310
  %115 = load i64, ptr %19, align 8, !dbg !310
  store i64 %115, ptr %16, align 8, !dbg !307
  call void @llvm.dbg.declare(metadata ptr %20, metadata !312, metadata !DIExpression()), !dbg !313
  %116 = load i64, ptr %6, align 8, !dbg !313
  %117 = load i64, ptr %12, align 8, !dbg !313
  %118 = sub i64 %116, %117, !dbg !313
  %119 = load i64, ptr %16, align 8, !dbg !313
  %120 = sub i64 %118, %119, !dbg !313
  store i64 %120, ptr %20, align 8, !dbg !313
  call void @llvm.dbg.declare(metadata ptr %21, metadata !314, metadata !DIExpression()), !dbg !315
  %121 = load ptr, ptr %10, align 8, !dbg !315
  %122 = getelementptr inbounds %struct.Zeta_CircularVector, ptr %121, i32 0, i32 4, !dbg !315
  %123 = load i64, ptr %122, align 8, !dbg !315
  %124 = sub i64 %123, 1, !dbg !315
  store i64 %124, ptr %21, align 8, !dbg !315
  call void @llvm.dbg.declare(metadata ptr %22, metadata !316, metadata !DIExpression()), !dbg !317
  %125 = load i64, ptr %6, align 8, !dbg !317
  %126 = sub i64 %125, 1, !dbg !317
  store i64 %126, ptr %22, align 8, !dbg !317
  call void @llvm.dbg.declare(metadata ptr %23, metadata !318, metadata !DIExpression()), !dbg !319
  call void @llvm.dbg.declare(metadata ptr %24, metadata !320, metadata !DIExpression()), !dbg !321
  %127 = load ptr, ptr %9, align 8, !dbg !322
  %128 = call ptr @Zeta_CircularVector_PeekL(ptr noundef %127, ptr noundef %24, ptr noundef null), !dbg !322
  %129 = load i64, ptr %6, align 8, !dbg !323
  %130 = load ptr, ptr %9, align 8, !dbg !323
  %131 = call ptr @Zeta_CircularVector_Insert(ptr noundef %130, ptr noundef %24, i64 noundef %129), !dbg !323
  call void @llvm.dbg.declare(metadata ptr %25, metadata !324, metadata !DIExpression()), !dbg !326
  %132 = load i64, ptr %12, align 8, !dbg !326
  store i64 %132, ptr %25, align 8, !dbg !326
  br label %133, !dbg !326

133:                                              ; preds = %145, %113
  %134 = load i64, ptr %25, align 8, !dbg !326
  %135 = add i64 %134, -1, !dbg !326
  store i64 %135, ptr %25, align 8, !dbg !326
  %136 = icmp ult i64 0, %134, !dbg !326
  br i1 %136, label %137, label %150, !dbg !326

137:                                              ; preds = %133
  %138 = load i64, ptr %11, align 8, !dbg !327
  %139 = load i64, ptr %21, align 8, !dbg !327
  %140 = load ptr, ptr %10, align 8, !dbg !327
  %141 = call ptr @Zeta_CircularVector_Access(ptr noundef %140, ptr noundef null, ptr noundef null, i64 noundef %139), !dbg !327
  %142 = load i64, ptr %22, align 8, !dbg !327
  %143 = load ptr, ptr %9, align 8, !dbg !327
  %144 = call ptr @Zeta_CircularVector_Access(ptr noundef %143, ptr noundef null, ptr noundef null, i64 noundef %142), !dbg !327
  call void @Zeta_MemCopy(ptr noundef %144, ptr noundef %141, i64 noundef %138), !dbg !327
  br label %145, !dbg !330

145:                                              ; preds = %137
  %146 = load i64, ptr %21, align 8, !dbg !331
  %147 = add i64 %146, -1, !dbg !331
  store i64 %147, ptr %21, align 8, !dbg !331
  %148 = load i64, ptr %22, align 8, !dbg !331
  %149 = add i64 %148, -1, !dbg !331
  store i64 %149, ptr %22, align 8, !dbg !331
  br label %133, !dbg !331, !llvm.loop !332

150:                                              ; preds = %133
  %151 = load i64, ptr %16, align 8, !dbg !334
  %152 = load i64, ptr %22, align 8, !dbg !334
  %153 = sub i64 %152, %151, !dbg !334
  store i64 %153, ptr %22, align 8, !dbg !334
  call void @llvm.dbg.declare(metadata ptr %26, metadata !335, metadata !DIExpression()), !dbg !337
  %154 = load i64, ptr %20, align 8, !dbg !337
  store i64 %154, ptr %26, align 8, !dbg !337
  br label %155, !dbg !337

155:                                              ; preds = %167, %150
  %156 = load i64, ptr %26, align 8, !dbg !337
  %157 = add i64 %156, -1, !dbg !337
  store i64 %157, ptr %26, align 8, !dbg !337
  %158 = icmp ult i64 0, %156, !dbg !337
  br i1 %158, label %159, label %172, !dbg !337

159:                                              ; preds = %155
  %160 = load i64, ptr %11, align 8, !dbg !338
  %161 = load i64, ptr %21, align 8, !dbg !338
  %162 = load ptr, ptr %10, align 8, !dbg !338
  %163 = call ptr @Zeta_CircularVector_Access(ptr noundef %162, ptr noundef null, ptr noundef null, i64 noundef %161), !dbg !338
  %164 = load i64, ptr %22, align 8, !dbg !338
  %165 = load ptr, ptr %9, align 8, !dbg !338
  %166 = call ptr @Zeta_CircularVector_Access(ptr noundef %165, ptr noundef null, ptr noundef null, i64 noundef %164), !dbg !338
  call void @Zeta_MemCopy(ptr noundef %166, ptr noundef %163, i64 noundef %160), !dbg !338
  br label %167, !dbg !341

167:                                              ; preds = %159
  %168 = load i64, ptr %21, align 8, !dbg !342
  %169 = add i64 %168, -1, !dbg !342
  store i64 %169, ptr %21, align 8, !dbg !342
  %170 = load i64, ptr %22, align 8, !dbg !342
  %171 = add i64 %170, -1, !dbg !342
  store i64 %171, ptr %22, align 8, !dbg !342
  br label %155, !dbg !342, !llvm.loop !343

172:                                              ; preds = %155
  %173 = load ptr, ptr %10, align 8, !dbg !345
  %174 = getelementptr inbounds %struct.Zeta_CircularVector, ptr %173, i32 0, i32 4, !dbg !345
  %175 = load i64, ptr %174, align 8, !dbg !345
  %176 = load i64, ptr %12, align 8, !dbg !345
  %177 = sub i64 %175, %176, !dbg !345
  %178 = load i64, ptr %20, align 8, !dbg !345
  %179 = sub i64 %177, %178, !dbg !345
  %180 = load ptr, ptr %10, align 8, !dbg !345
  %181 = call ptr @Zeta_CircularVector_Access(ptr noundef %180, ptr noundef %23, ptr noundef null, i64 noundef %179), !dbg !345
  %182 = load i64, ptr %12, align 8, !dbg !346
  %183 = load i64, ptr %20, align 8, !dbg !346
  %184 = add i64 %182, %183, !dbg !346
  %185 = load ptr, ptr %10, align 8, !dbg !346
  call void @Zeta_CircularVector_Erase(ptr noundef %185, ptr noundef %23, i64 noundef %184), !dbg !346
  %186 = load i64, ptr %8, align 8, !dbg !347
  %187 = load i64, ptr %12, align 8, !dbg !347
  %188 = sub i64 %186, %187, !dbg !347
  %189 = load ptr, ptr %10, align 8, !dbg !347
  call void @Zeta_CircularVector_Cursor_AdvanceL(ptr noundef %189, ptr noundef %23, i64 noundef %188), !dbg !347
  %190 = load i64, ptr %7, align 8, !dbg !348
  %191 = load i64, ptr %16, align 8, !dbg !348
  %192 = sub i64 %190, %191, !dbg !348
  %193 = load ptr, ptr %10, align 8, !dbg !348
  %194 = call ptr @Zeta_CircularVector_Insert(ptr noundef %193, ptr noundef %23, i64 noundef %192), !dbg !348
  ret void, !dbg !349
}

declare dso_local void @Zeta_CircularVector_Cursor_AdvanceL(ptr noundef, ptr noundef, i64 noundef) #2

; Function Attrs: nocallback nofree nosync nounwind willreturn
declare void @llvm.va_start(ptr) #4

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr dso_local i32 @_vsprintf_l(ptr noundef %0, ptr noundef %1, ptr noundef %2, ptr noundef %3) #0 comdat !dbg !350 {
  %5 = alloca ptr, align 8
  %6 = alloca ptr, align 8
  %7 = alloca ptr, align 8
  %8 = alloca ptr, align 8
  store ptr %3, ptr %5, align 8
  call void @llvm.dbg.declare(metadata ptr %5, metadata !366, metadata !DIExpression()), !dbg !367
  store ptr %2, ptr %6, align 8
  call void @llvm.dbg.declare(metadata ptr %6, metadata !368, metadata !DIExpression()), !dbg !369
  store ptr %1, ptr %7, align 8
  call void @llvm.dbg.declare(metadata ptr %7, metadata !370, metadata !DIExpression()), !dbg !371
  store ptr %0, ptr %8, align 8
  call void @llvm.dbg.declare(metadata ptr %8, metadata !372, metadata !DIExpression()), !dbg !373
  %9 = load ptr, ptr %5, align 8, !dbg !374
  %10 = load ptr, ptr %6, align 8, !dbg !374
  %11 = load ptr, ptr %7, align 8, !dbg !374
  %12 = load ptr, ptr %8, align 8, !dbg !374
  %13 = call i32 @_vsnprintf_l(ptr noundef %12, i64 noundef -1, ptr noundef %11, ptr noundef %10, ptr noundef %9), !dbg !374
  ret i32 %13, !dbg !374
}

; Function Attrs: nocallback nofree nosync nounwind willreturn
declare void @llvm.va_end(ptr) #4

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr dso_local i32 @_vsnprintf_l(ptr noundef %0, i64 noundef %1, ptr noundef %2, ptr noundef %3, ptr noundef %4) #0 comdat !dbg !375 {
  %6 = alloca ptr, align 8
  %7 = alloca ptr, align 8
  %8 = alloca ptr, align 8
  %9 = alloca i64, align 8
  %10 = alloca ptr, align 8
  %11 = alloca i32, align 4
  store ptr %4, ptr %6, align 8
  call void @llvm.dbg.declare(metadata ptr %6, metadata !378, metadata !DIExpression()), !dbg !379
  store ptr %3, ptr %7, align 8
  call void @llvm.dbg.declare(metadata ptr %7, metadata !380, metadata !DIExpression()), !dbg !381
  store ptr %2, ptr %8, align 8
  call void @llvm.dbg.declare(metadata ptr %8, metadata !382, metadata !DIExpression()), !dbg !383
  store i64 %1, ptr %9, align 8
  call void @llvm.dbg.declare(metadata ptr %9, metadata !384, metadata !DIExpression()), !dbg !385
  store ptr %0, ptr %10, align 8
  call void @llvm.dbg.declare(metadata ptr %10, metadata !386, metadata !DIExpression()), !dbg !387
  call void @llvm.dbg.declare(metadata ptr %11, metadata !388, metadata !DIExpression()), !dbg !390
  %12 = load ptr, ptr %6, align 8, !dbg !390
  %13 = load ptr, ptr %7, align 8, !dbg !390
  %14 = load ptr, ptr %8, align 8, !dbg !390
  %15 = load i64, ptr %9, align 8, !dbg !390
  %16 = load ptr, ptr %10, align 8, !dbg !390
  %17 = call ptr @__local_stdio_printf_options(), !dbg !390
  %18 = load i64, ptr %17, align 8, !dbg !390
  %19 = or i64 %18, 1, !dbg !390
  %20 = call i32 @__stdio_common_vsprintf(i64 noundef %19, ptr noundef %16, i64 noundef %15, ptr noundef %14, ptr noundef %13, ptr noundef %12), !dbg !390
  store i32 %20, ptr %11, align 4, !dbg !390
  %21 = load i32, ptr %11, align 4, !dbg !391
  %22 = icmp slt i32 %21, 0, !dbg !391
  br i1 %22, label %23, label %24, !dbg !391

23:                                               ; preds = %5
  br label %26, !dbg !391

24:                                               ; preds = %5
  %25 = load i32, ptr %11, align 4, !dbg !391
  br label %26, !dbg !391

26:                                               ; preds = %24, %23
  %27 = phi i32 [ -1, %23 ], [ %25, %24 ], !dbg !391
  ret i32 %27, !dbg !391
}

declare dso_local i32 @__stdio_common_vsprintf(i64 noundef, ptr noundef, i64 noundef, ptr noundef, ptr noundef, ptr noundef) #2

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr dso_local ptr @__local_stdio_printf_options() #0 comdat !dbg !53 {
  ret ptr @__local_stdio_printf_options._OptionsStorage, !dbg !392
}

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr dso_local i32 @_vfprintf_l(ptr noundef %0, ptr noundef %1, ptr noundef %2, ptr noundef %3) #0 comdat !dbg !393 {
  %5 = alloca ptr, align 8
  %6 = alloca ptr, align 8
  %7 = alloca ptr, align 8
  %8 = alloca ptr, align 8
  store ptr %3, ptr %5, align 8
  call void @llvm.dbg.declare(metadata ptr %5, metadata !403, metadata !DIExpression()), !dbg !404
  store ptr %2, ptr %6, align 8
  call void @llvm.dbg.declare(metadata ptr %6, metadata !405, metadata !DIExpression()), !dbg !406
  store ptr %1, ptr %7, align 8
  call void @llvm.dbg.declare(metadata ptr %7, metadata !407, metadata !DIExpression()), !dbg !408
  store ptr %0, ptr %8, align 8
  call void @llvm.dbg.declare(metadata ptr %8, metadata !409, metadata !DIExpression()), !dbg !410
  %9 = load ptr, ptr %5, align 8, !dbg !411
  %10 = load ptr, ptr %6, align 8, !dbg !411
  %11 = load ptr, ptr %7, align 8, !dbg !411
  %12 = load ptr, ptr %8, align 8, !dbg !411
  %13 = call ptr @__local_stdio_printf_options(), !dbg !411
  %14 = load i64, ptr %13, align 8, !dbg !411
  %15 = call i32 @__stdio_common_vfprintf(i64 noundef %14, ptr noundef %12, ptr noundef %11, ptr noundef %10, ptr noundef %9), !dbg !411
  ret i32 %15, !dbg !411
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
!1 = distinct !DIGlobalVariable(scope: null, file: !2, line: 7, type: !3, isLocal: true, isDefinition: true)
!2 = !DIFile(filename: "D:/ZetaDevelop/Zeta/SegUtils.c", directory: "", checksumkind: CSK_MD5, checksum: "27dba9744b4d3276e3df53ced9dd2bb9")
!3 = !DICompositeType(tag: DW_TAG_array_type, baseType: !4, size: 80, elements: !6)
!4 = !DIDerivedType(tag: DW_TAG_const_type, baseType: !5)
!5 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!6 = !{!7}
!7 = !DISubrange(count: 10)
!8 = !DIGlobalVariableExpression(var: !9, expr: !DIExpression())
!9 = distinct !DIGlobalVariable(scope: null, file: !2, line: 7, type: !10, isLocal: true, isDefinition: true)
!10 = !DICompositeType(tag: DW_TAG_array_type, baseType: !5, size: 248, elements: !11)
!11 = !{!12}
!12 = !DISubrange(count: 31)
!13 = !DIGlobalVariableExpression(var: !14, expr: !DIExpression())
!14 = distinct !DIGlobalVariable(scope: null, file: !2, line: 7, type: !15, isLocal: true, isDefinition: true)
!15 = !DICompositeType(tag: DW_TAG_array_type, baseType: !5, size: 128, elements: !16)
!16 = !{!17}
!17 = !DISubrange(count: 16)
!18 = !DIGlobalVariableExpression(var: !19, expr: !DIExpression())
!19 = distinct !DIGlobalVariable(scope: null, file: !2, line: 7, type: !20, isLocal: true, isDefinition: true)
!20 = !DICompositeType(tag: DW_TAG_array_type, baseType: !5, size: 152, elements: !21)
!21 = !{!22}
!22 = !DISubrange(count: 19)
!23 = !DIGlobalVariableExpression(var: !24, expr: !DIExpression())
!24 = distinct !DIGlobalVariable(scope: null, file: !2, line: 7, type: !25, isLocal: true, isDefinition: true)
!25 = !DICompositeType(tag: DW_TAG_array_type, baseType: !5, size: 96, elements: !26)
!26 = !{!27}
!27 = !DISubrange(count: 12)
!28 = !DIGlobalVariableExpression(var: !29, expr: !DIExpression())
!29 = distinct !DIGlobalVariable(scope: null, file: !2, line: 8, type: !20, isLocal: true, isDefinition: true)
!30 = !DIGlobalVariableExpression(var: !31, expr: !DIExpression())
!31 = distinct !DIGlobalVariable(scope: null, file: !2, line: 10, type: !32, isLocal: true, isDefinition: true)
!32 = !DICompositeType(tag: DW_TAG_array_type, baseType: !5, size: 328, elements: !33)
!33 = !{!34}
!34 = !DISubrange(count: 41)
!35 = !DIGlobalVariableExpression(var: !36, expr: !DIExpression())
!36 = distinct !DIGlobalVariable(scope: null, file: !2, line: 11, type: !37, isLocal: true, isDefinition: true)
!37 = !DICompositeType(tag: DW_TAG_array_type, baseType: !5, size: 272, elements: !38)
!38 = !{!39}
!39 = !DISubrange(count: 34)
!40 = !DIGlobalVariableExpression(var: !41, expr: !DIExpression())
!41 = distinct !DIGlobalVariable(scope: null, file: !2, line: 14, type: !42, isLocal: true, isDefinition: true)
!42 = !DICompositeType(tag: DW_TAG_array_type, baseType: !5, size: 168, elements: !43)
!43 = !{!44}
!44 = !DISubrange(count: 21)
!45 = !DIGlobalVariableExpression(var: !46, expr: !DIExpression())
!46 = distinct !DIGlobalVariable(scope: null, file: !2, line: 53, type: !3, isLocal: true, isDefinition: true)
!47 = !DIGlobalVariableExpression(var: !48, expr: !DIExpression())
!48 = distinct !DIGlobalVariable(scope: null, file: !2, line: 56, type: !32, isLocal: true, isDefinition: true)
!49 = !DIGlobalVariableExpression(var: !50, expr: !DIExpression())
!50 = distinct !DIGlobalVariable(scope: null, file: !2, line: 57, type: !37, isLocal: true, isDefinition: true)
!51 = !DIGlobalVariableExpression(var: !52, expr: !DIExpression())
!52 = distinct !DIGlobalVariable(name: "_OptionsStorage", scope: !53, file: !54, line: 91, type: !58, isLocal: true, isDefinition: true)
!53 = distinct !DISubprogram(name: "__local_stdio_printf_options", scope: !54, file: !54, line: 89, type: !55, scopeLine: 90, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !59)
!54 = !DIFile(filename: "C:\\Program Files (x86)\\Windows Kits\\10\\Include\\10.0.22621.0\\ucrt\\corecrt_stdio_config.h", directory: "", checksumkind: CSK_MD5, checksum: "dacf907bda504afb0b64f53a242bdae6")
!55 = !DISubroutineType(types: !56)
!56 = !{!57}
!57 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !58, size: 64)
!58 = !DIBasicType(name: "unsigned long long", size: 64, encoding: DW_ATE_unsigned)
!59 = distinct !DICompileUnit(language: DW_LANG_C11, file: !60, producer: "clang version 18.1.4", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, retainedTypes: !61, globals: !65, splitDebugInlining: false, nameTableKind: None)
!60 = !DIFile(filename: "D:/ZetaDevelop/Zeta\\SegUtils.c", directory: "D:\\ZetaDevelop", checksumkind: CSK_MD5, checksum: "27dba9744b4d3276e3df53ced9dd2bb9")
!61 = !{!62, !63}
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
!137 = distinct !DISubprogram(name: "SegShoveL", scope: !2, file: !2, line: 5, type: !138, scopeLine: 6, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !59, retainedNodes: !82)
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
!151 = !DILocalVariable(name: "shove_cnt", arg: 5, scope: !137, file: !2, line: 6, type: !63)
!152 = !DILocation(line: 6, scope: !137)
!153 = !DILocalVariable(name: "ins_cnt", arg: 4, scope: !137, file: !2, line: 6, type: !63)
!154 = !DILocalVariable(name: "rl_cnt", arg: 3, scope: !137, file: !2, line: 6, type: !63)
!155 = !DILocalVariable(name: "r_cv", arg: 2, scope: !137, file: !2, line: 5, type: !140)
!156 = !DILocation(line: 5, scope: !137)
!157 = !DILocalVariable(name: "l_cv", arg: 1, scope: !137, file: !2, line: 5, type: !140)
!158 = !DILocation(line: 7, scope: !137)
!159 = !DILocation(line: 7, scope: !160)
!160 = distinct !DILexicalBlock(scope: !161, file: !2, line: 7)
!161 = distinct !DILexicalBlock(scope: !137, file: !2, line: 7)
!162 = !DILocation(line: 7, scope: !163)
!163 = distinct !DILexicalBlock(scope: !161, file: !2, line: 7)
!164 = !DILocation(line: 8, scope: !137)
!165 = !DILocation(line: 8, scope: !166)
!166 = distinct !DILexicalBlock(scope: !167, file: !2, line: 8)
!167 = distinct !DILexicalBlock(scope: !137, file: !2, line: 8)
!168 = !DILocation(line: 8, scope: !169)
!169 = distinct !DILexicalBlock(scope: !167, file: !2, line: 8)
!170 = !DILocation(line: 10, scope: !137)
!171 = !DILocation(line: 10, scope: !172)
!172 = distinct !DILexicalBlock(scope: !173, file: !2, line: 10)
!173 = distinct !DILexicalBlock(scope: !137, file: !2, line: 10)
!174 = !DILocation(line: 10, scope: !175)
!175 = distinct !DILexicalBlock(scope: !173, file: !2, line: 10)
!176 = !DILocation(line: 11, scope: !137)
!177 = !DILocation(line: 11, scope: !178)
!178 = distinct !DILexicalBlock(scope: !179, file: !2, line: 11)
!179 = distinct !DILexicalBlock(scope: !137, file: !2, line: 11)
!180 = !DILocation(line: 11, scope: !181)
!181 = distinct !DILexicalBlock(scope: !179, file: !2, line: 11)
!182 = !DILocalVariable(name: "width", scope: !137, file: !2, line: 13, type: !63)
!183 = !DILocation(line: 13, scope: !137)
!184 = !DILocation(line: 14, scope: !137)
!185 = !DILocation(line: 14, scope: !186)
!186 = distinct !DILexicalBlock(scope: !187, file: !2, line: 14)
!187 = distinct !DILexicalBlock(scope: !137, file: !2, line: 14)
!188 = !DILocation(line: 14, scope: !189)
!189 = distinct !DILexicalBlock(scope: !187, file: !2, line: 14)
!190 = !DILocalVariable(name: "cnt_a", scope: !137, file: !2, line: 16, type: !63)
!191 = !DILocation(line: 16, scope: !137)
!192 = !DILocalVariable(name: "ZETA_tmp_0", scope: !193, file: !2, line: 16, type: !63)
!193 = distinct !DILexicalBlock(scope: !137, file: !2, line: 16)
!194 = !DILocation(line: 16, scope: !193)
!195 = !DILocalVariable(name: "ZETA_tmp_1", scope: !193, file: !2, line: 16, type: !63)
!196 = !DILocalVariable(name: "cnt_b", scope: !137, file: !2, line: 17, type: !63)
!197 = !DILocation(line: 17, scope: !137)
!198 = !DILocalVariable(name: "ZETA_tmp_2", scope: !199, file: !2, line: 17, type: !63)
!199 = distinct !DILexicalBlock(scope: !137, file: !2, line: 17)
!200 = !DILocation(line: 17, scope: !199)
!201 = !DILocalVariable(name: "ZETA_tmp_3", scope: !199, file: !2, line: 17, type: !63)
!202 = !DILocalVariable(name: "cnt_c", scope: !137, file: !2, line: 18, type: !63)
!203 = !DILocation(line: 18, scope: !137)
!204 = !DILocalVariable(name: "l_i", scope: !137, file: !2, line: 20, type: !63)
!205 = !DILocation(line: 20, scope: !137)
!206 = !DILocalVariable(name: "r_i", scope: !137, file: !2, line: 21, type: !63)
!207 = !DILocation(line: 21, scope: !137)
!208 = !DILocalVariable(name: "l_cv_cursor", scope: !137, file: !2, line: 23, type: !209)
!209 = !DIDerivedType(tag: DW_TAG_typedef, name: "Zeta_CircularVector_Cursor", file: !142, line: 11, baseType: !210)
!210 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "Zeta_CircularVector_Cursor", file: !142, line: 22, size: 192, elements: !211)
!211 = !{!212, !213, !214}
!212 = !DIDerivedType(tag: DW_TAG_member, name: "cv", scope: !210, file: !142, line: 23, baseType: !140, size: 64)
!213 = !DIDerivedType(tag: DW_TAG_member, name: "idx", scope: !210, file: !142, line: 24, baseType: !63, size: 64, offset: 64)
!214 = !DIDerivedType(tag: DW_TAG_member, name: "ele", scope: !210, file: !142, line: 25, baseType: !62, size: 64, offset: 128)
!215 = !DILocation(line: 23, scope: !137)
!216 = !DILocalVariable(name: "r_cv_cursor", scope: !137, file: !2, line: 24, type: !209)
!217 = !DILocation(line: 24, scope: !137)
!218 = !DILocation(line: 26, scope: !137)
!219 = !DILocation(line: 28, scope: !137)
!220 = !DILocalVariable(name: "i", scope: !221, file: !2, line: 30, type: !63)
!221 = distinct !DILexicalBlock(scope: !137, file: !2, line: 30)
!222 = !DILocation(line: 30, scope: !221)
!223 = !DILocation(line: 31, scope: !224)
!224 = distinct !DILexicalBlock(scope: !225, file: !2, line: 30)
!225 = distinct !DILexicalBlock(scope: !221, file: !2, line: 30)
!226 = !DILocation(line: 33, scope: !224)
!227 = !DILocation(line: 30, scope: !225)
!228 = distinct !{!228, !222, !229, !230}
!229 = !DILocation(line: 33, scope: !221)
!230 = !{!"llvm.loop.mustprogress"}
!231 = !DILocation(line: 35, scope: !137)
!232 = !DILocalVariable(name: "i", scope: !233, file: !2, line: 37, type: !63)
!233 = distinct !DILexicalBlock(scope: !137, file: !2, line: 37)
!234 = !DILocation(line: 37, scope: !233)
!235 = !DILocation(line: 38, scope: !236)
!236 = distinct !DILexicalBlock(scope: !237, file: !2, line: 37)
!237 = distinct !DILexicalBlock(scope: !233, file: !2, line: 37)
!238 = !DILocation(line: 40, scope: !236)
!239 = !DILocation(line: 37, scope: !237)
!240 = distinct !{!240, !234, !241, !230}
!241 = !DILocation(line: 40, scope: !233)
!242 = !DILocation(line: 42, scope: !137)
!243 = !DILocation(line: 44, scope: !137)
!244 = !DILocation(line: 46, scope: !137)
!245 = !DILocation(line: 48, scope: !137)
!246 = !DILocation(line: 49, scope: !137)
!247 = distinct !DISubprogram(name: "printf", scope: !74, file: !74, line: 950, type: !248, scopeLine: 956, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !59, retainedNodes: !82)
!248 = !DISubroutineType(types: !249)
!249 = !{!77, !80, null}
!250 = !DILocalVariable(name: "_Format", arg: 1, scope: !247, file: !74, line: 951, type: !80)
!251 = !DILocation(line: 951, scope: !247)
!252 = !DILocalVariable(name: "_Result", scope: !247, file: !74, line: 957, type: !77)
!253 = !DILocation(line: 957, scope: !247)
!254 = !DILocalVariable(name: "_ArgList", scope: !247, file: !74, line: 958, type: !90)
!255 = !DILocation(line: 958, scope: !247)
!256 = !DILocation(line: 959, scope: !247)
!257 = !DILocation(line: 960, scope: !247)
!258 = !DILocation(line: 961, scope: !247)
!259 = !DILocation(line: 962, scope: !247)
!260 = distinct !DISubprogram(name: "SegShoveR", scope: !2, file: !2, line: 51, type: !138, scopeLine: 52, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !59, retainedNodes: !82)
!261 = !DILocalVariable(name: "shove_cnt", arg: 5, scope: !260, file: !2, line: 52, type: !63)
!262 = !DILocation(line: 52, scope: !260)
!263 = !DILocalVariable(name: "ins_cnt", arg: 4, scope: !260, file: !2, line: 52, type: !63)
!264 = !DILocalVariable(name: "lr_cnt", arg: 3, scope: !260, file: !2, line: 52, type: !63)
!265 = !DILocalVariable(name: "r_cv", arg: 2, scope: !260, file: !2, line: 51, type: !140)
!266 = !DILocation(line: 51, scope: !260)
!267 = !DILocalVariable(name: "l_cv", arg: 1, scope: !260, file: !2, line: 51, type: !140)
!268 = !DILocation(line: 53, scope: !260)
!269 = !DILocation(line: 53, scope: !270)
!270 = distinct !DILexicalBlock(scope: !271, file: !2, line: 53)
!271 = distinct !DILexicalBlock(scope: !260, file: !2, line: 53)
!272 = !DILocation(line: 53, scope: !273)
!273 = distinct !DILexicalBlock(scope: !271, file: !2, line: 53)
!274 = !DILocation(line: 54, scope: !260)
!275 = !DILocation(line: 54, scope: !276)
!276 = distinct !DILexicalBlock(scope: !277, file: !2, line: 54)
!277 = distinct !DILexicalBlock(scope: !260, file: !2, line: 54)
!278 = !DILocation(line: 54, scope: !279)
!279 = distinct !DILexicalBlock(scope: !277, file: !2, line: 54)
!280 = !DILocation(line: 56, scope: !260)
!281 = !DILocation(line: 56, scope: !282)
!282 = distinct !DILexicalBlock(scope: !283, file: !2, line: 56)
!283 = distinct !DILexicalBlock(scope: !260, file: !2, line: 56)
!284 = !DILocation(line: 56, scope: !285)
!285 = distinct !DILexicalBlock(scope: !283, file: !2, line: 56)
!286 = !DILocation(line: 57, scope: !260)
!287 = !DILocation(line: 57, scope: !288)
!288 = distinct !DILexicalBlock(scope: !289, file: !2, line: 57)
!289 = distinct !DILexicalBlock(scope: !260, file: !2, line: 57)
!290 = !DILocation(line: 57, scope: !291)
!291 = distinct !DILexicalBlock(scope: !289, file: !2, line: 57)
!292 = !DILocalVariable(name: "width", scope: !260, file: !2, line: 59, type: !63)
!293 = !DILocation(line: 59, scope: !260)
!294 = !DILocation(line: 60, scope: !260)
!295 = !DILocation(line: 60, scope: !296)
!296 = distinct !DILexicalBlock(scope: !297, file: !2, line: 60)
!297 = distinct !DILexicalBlock(scope: !260, file: !2, line: 60)
!298 = !DILocation(line: 60, scope: !299)
!299 = distinct !DILexicalBlock(scope: !297, file: !2, line: 60)
!300 = !DILocalVariable(name: "cnt_a", scope: !260, file: !2, line: 62, type: !63)
!301 = !DILocation(line: 62, scope: !260)
!302 = !DILocalVariable(name: "ZETA_tmp_4", scope: !303, file: !2, line: 62, type: !63)
!303 = distinct !DILexicalBlock(scope: !260, file: !2, line: 62)
!304 = !DILocation(line: 62, scope: !303)
!305 = !DILocalVariable(name: "ZETA_tmp_5", scope: !303, file: !2, line: 62, type: !63)
!306 = !DILocalVariable(name: "cnt_b", scope: !260, file: !2, line: 63, type: !63)
!307 = !DILocation(line: 63, scope: !260)
!308 = !DILocalVariable(name: "ZETA_tmp_6", scope: !309, file: !2, line: 63, type: !63)
!309 = distinct !DILexicalBlock(scope: !260, file: !2, line: 63)
!310 = !DILocation(line: 63, scope: !309)
!311 = !DILocalVariable(name: "ZETA_tmp_7", scope: !309, file: !2, line: 63, type: !63)
!312 = !DILocalVariable(name: "cnt_c", scope: !260, file: !2, line: 64, type: !63)
!313 = !DILocation(line: 64, scope: !260)
!314 = !DILocalVariable(name: "l_i", scope: !260, file: !2, line: 66, type: !63)
!315 = !DILocation(line: 66, scope: !260)
!316 = !DILocalVariable(name: "r_i", scope: !260, file: !2, line: 67, type: !63)
!317 = !DILocation(line: 67, scope: !260)
!318 = !DILocalVariable(name: "l_cv_cursor", scope: !260, file: !2, line: 69, type: !209)
!319 = !DILocation(line: 69, scope: !260)
!320 = !DILocalVariable(name: "r_cv_cursor", scope: !260, file: !2, line: 70, type: !209)
!321 = !DILocation(line: 70, scope: !260)
!322 = !DILocation(line: 72, scope: !260)
!323 = !DILocation(line: 74, scope: !260)
!324 = !DILocalVariable(name: "i", scope: !325, file: !2, line: 76, type: !63)
!325 = distinct !DILexicalBlock(scope: !260, file: !2, line: 76)
!326 = !DILocation(line: 76, scope: !325)
!327 = !DILocation(line: 77, scope: !328)
!328 = distinct !DILexicalBlock(scope: !329, file: !2, line: 76)
!329 = distinct !DILexicalBlock(scope: !325, file: !2, line: 76)
!330 = !DILocation(line: 79, scope: !328)
!331 = !DILocation(line: 76, scope: !329)
!332 = distinct !{!332, !326, !333, !230}
!333 = !DILocation(line: 79, scope: !325)
!334 = !DILocation(line: 81, scope: !260)
!335 = !DILocalVariable(name: "i", scope: !336, file: !2, line: 83, type: !63)
!336 = distinct !DILexicalBlock(scope: !260, file: !2, line: 83)
!337 = !DILocation(line: 83, scope: !336)
!338 = !DILocation(line: 84, scope: !339)
!339 = distinct !DILexicalBlock(scope: !340, file: !2, line: 83)
!340 = distinct !DILexicalBlock(scope: !336, file: !2, line: 83)
!341 = !DILocation(line: 86, scope: !339)
!342 = !DILocation(line: 83, scope: !340)
!343 = distinct !{!343, !337, !344, !230}
!344 = !DILocation(line: 86, scope: !336)
!345 = !DILocation(line: 88, scope: !260)
!346 = !DILocation(line: 91, scope: !260)
!347 = !DILocation(line: 93, scope: !260)
!348 = !DILocation(line: 95, scope: !260)
!349 = !DILocation(line: 96, scope: !260)
!350 = distinct !DISubprogram(name: "_vsprintf_l", scope: !74, file: !74, line: 1449, type: !351, scopeLine: 1458, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !59, retainedNodes: !82)
!351 = !DISubroutineType(types: !352)
!352 = !{!77, !78, !80, !353, !90}
!353 = !DIDerivedType(tag: DW_TAG_const_type, baseType: !354)
!354 = !DIDerivedType(tag: DW_TAG_typedef, name: "_locale_t", file: !355, line: 623, baseType: !356)
!355 = !DIFile(filename: "C:\\Program Files (x86)\\Windows Kits\\10\\Include\\10.0.22621.0\\ucrt\\corecrt.h", directory: "", checksumkind: CSK_MD5, checksum: "4ce81db8e96f94c79f8dce86dd46b97f")
!356 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !357, size: 64)
!357 = !DIDerivedType(tag: DW_TAG_typedef, name: "__crt_locale_pointers", file: !355, line: 621, baseType: !358)
!358 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "__crt_locale_pointers", file: !355, line: 617, size: 128, elements: !359)
!359 = !{!360, !363}
!360 = !DIDerivedType(tag: DW_TAG_member, name: "locinfo", scope: !358, file: !355, line: 619, baseType: !361, size: 64)
!361 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !362, size: 64)
!362 = !DICompositeType(tag: DW_TAG_structure_type, name: "__crt_locale_data", file: !355, line: 619, flags: DIFlagFwdDecl)
!363 = !DIDerivedType(tag: DW_TAG_member, name: "mbcinfo", scope: !358, file: !355, line: 620, baseType: !364, size: 64, offset: 64)
!364 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !365, size: 64)
!365 = !DICompositeType(tag: DW_TAG_structure_type, name: "__crt_multibyte_data", file: !355, line: 620, flags: DIFlagFwdDecl)
!366 = !DILocalVariable(name: "_ArgList", arg: 4, scope: !350, file: !74, line: 1453, type: !90)
!367 = !DILocation(line: 1453, scope: !350)
!368 = !DILocalVariable(name: "_Locale", arg: 3, scope: !350, file: !74, line: 1452, type: !353)
!369 = !DILocation(line: 1452, scope: !350)
!370 = !DILocalVariable(name: "_Format", arg: 2, scope: !350, file: !74, line: 1451, type: !80)
!371 = !DILocation(line: 1451, scope: !350)
!372 = !DILocalVariable(name: "_Buffer", arg: 1, scope: !350, file: !74, line: 1450, type: !78)
!373 = !DILocation(line: 1450, scope: !350)
!374 = !DILocation(line: 1459, scope: !350)
!375 = distinct !DISubprogram(name: "_vsnprintf_l", scope: !74, file: !74, line: 1381, type: !376, scopeLine: 1391, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !59, retainedNodes: !82)
!376 = !DISubroutineType(types: !377)
!377 = !{!77, !78, !110, !80, !353, !90}
!378 = !DILocalVariable(name: "_ArgList", arg: 5, scope: !375, file: !74, line: 1386, type: !90)
!379 = !DILocation(line: 1386, scope: !375)
!380 = !DILocalVariable(name: "_Locale", arg: 4, scope: !375, file: !74, line: 1385, type: !353)
!381 = !DILocation(line: 1385, scope: !375)
!382 = !DILocalVariable(name: "_Format", arg: 3, scope: !375, file: !74, line: 1384, type: !80)
!383 = !DILocation(line: 1384, scope: !375)
!384 = !DILocalVariable(name: "_BufferCount", arg: 2, scope: !375, file: !74, line: 1383, type: !110)
!385 = !DILocation(line: 1383, scope: !375)
!386 = !DILocalVariable(name: "_Buffer", arg: 1, scope: !375, file: !74, line: 1382, type: !78)
!387 = !DILocation(line: 1382, scope: !375)
!388 = !DILocalVariable(name: "_Result", scope: !375, file: !74, line: 1392, type: !389)
!389 = !DIDerivedType(tag: DW_TAG_const_type, baseType: !77)
!390 = !DILocation(line: 1392, scope: !375)
!391 = !DILocation(line: 1396, scope: !375)
!392 = !DILocation(line: 92, scope: !53)
!393 = distinct !DISubprogram(name: "_vfprintf_l", scope: !74, file: !74, line: 635, type: !394, scopeLine: 644, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !59, retainedNodes: !82)
!394 = !DISubroutineType(types: !395)
!395 = !{!77, !396, !80, !353, !90}
!396 = !DIDerivedType(tag: DW_TAG_const_type, baseType: !397)
!397 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !398, size: 64)
!398 = !DIDerivedType(tag: DW_TAG_typedef, name: "FILE", file: !399, line: 31, baseType: !400)
!399 = !DIFile(filename: "C:\\Program Files (x86)\\Windows Kits\\10\\Include\\10.0.22621.0\\ucrt\\corecrt_wstdio.h", directory: "", checksumkind: CSK_MD5, checksum: "bf50373b435d0afd0235dd3e05c4a277")
!400 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "_iobuf", file: !399, line: 28, size: 64, elements: !401)
!401 = !{!402}
!402 = !DIDerivedType(tag: DW_TAG_member, name: "_Placeholder", scope: !400, file: !399, line: 30, baseType: !62, size: 64)
!403 = !DILocalVariable(name: "_ArgList", arg: 4, scope: !393, file: !74, line: 639, type: !90)
!404 = !DILocation(line: 639, scope: !393)
!405 = !DILocalVariable(name: "_Locale", arg: 3, scope: !393, file: !74, line: 638, type: !353)
!406 = !DILocation(line: 638, scope: !393)
!407 = !DILocalVariable(name: "_Format", arg: 2, scope: !393, file: !74, line: 637, type: !80)
!408 = !DILocation(line: 637, scope: !393)
!409 = !DILocalVariable(name: "_Stream", arg: 1, scope: !393, file: !74, line: 636, type: !396)
!410 = !DILocation(line: 636, scope: !393)
!411 = !DILocation(line: 645, scope: !393)
