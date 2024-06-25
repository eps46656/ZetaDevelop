; ModuleID = 'D:/ZetaDevelop/Zeta/CheckPointRecorder.cpp'
source_filename = "D:/ZetaDevelop/Zeta/CheckPointRecorder.cpp"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc19.38.33133"

%rtti.TypeDescriptor30 = type { ptr, ptr, [31 x i8] }
%rtti.TypeDescriptor19 = type { ptr, ptr, [20 x i8] }
%rtti.CompleteObjectLocator = type { i32, i32, i32, i32, i32, i32 }
%rtti.ClassHierarchyDescriptor = type { i32, i32, i32, i32 }
%rtti.BaseClassDescriptor = type { i32, i32, i32, i32, i32, i32, i32 }
%struct.Zeta_CheckPointRecorder = type { i64, %"class.std::list" }
%"class.std::list" = type { %"class.std::_Compressed_pair" }
%"class.std::_Compressed_pair" = type { %"class.std::_List_val" }
%"class.std::_List_val" = type { ptr, i64 }
%"struct.std::_List_node" = type { ptr, ptr, %struct.Record }
%struct.Record = type { %"class.std::basic_string", i64, %"class.std::basic_string" }
%"class.std::basic_string" = type { %"class.std::_Compressed_pair.0" }
%"class.std::_Compressed_pair.0" = type { %"class.std::_String_val" }
%"class.std::_String_val" = type { %"union.std::_String_val<std::_Simple_types<char>>::_Bxty", i64, i64 }
%"union.std::_String_val<std::_Simple_types<char>>::_Bxty" = type { ptr, [8 x i8] }
%"class.std::exception" = type { ptr, %struct.__std_exception_data }
%struct.__std_exception_data = type { ptr, i8 }

$"??1?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@QEAA@XZ" = comdat any

$"??1Record@@QEAA@XZ" = comdat any

$printf = comdat any

$"??_Gbad_array_new_length@std@@UEAAPEAXI@Z" = comdat any

$"?what@exception@std@@UEBAPEBDXZ" = comdat any

$"??_Gbad_alloc@std@@UEAAPEAXI@Z" = comdat any

$"??_Gexception@std@@UEAAPEAXI@Z" = comdat any

$__local_stdio_printf_options = comdat any

$"?_Xlen_string@std@@YAXXZ" = comdat any

$"??_C@_0BO@EHAFBIND@Zeta_CheckPointRecorder_Print?$AA@" = comdat any

$"??_C@_0CL@DIKKEIGP@D?3?1ZetaDevelop?1Zeta?1CheckPointRe@" = comdat any

$"??_C@_0BC@CENDOPBP@?$CF48s?3?$CF?94llu?7?$CF?924s?$AA@" = comdat any

$"??_C@_08LFPIFFEP@cpr?5?$CB?$DN?50?$AA@" = comdat any

$"??_C@_0M@BJGCKIIG@?7assert?7?$CFs?6?$AA@" = comdat any

$"??_C@_0BP@OIDFFGNC@Zeta_InitStdCheckPointRecorder?$AA@" = comdat any

$"??_C@_0BM@CMKDHNKC@?$CBzeta_debugger_assert_stage?$AA@" = comdat any

$"??_C@_0CD@KGPBLEGJ@zeta_debugger_assert_callback?5?$DN?$DN@" = comdat any

$"??_R0?AVbad_array_new_length@std@@@8" = comdat any

$"??_R0?AVbad_alloc@std@@@8" = comdat any

$"??_R0?AVexception@std@@@8" = comdat any

$"??_7bad_array_new_length@std@@6B@" = comdat largest

$"??_R4bad_array_new_length@std@@6B@" = comdat any

$"??_R3bad_array_new_length@std@@8" = comdat any

$"??_R2bad_array_new_length@std@@8" = comdat any

$"??_R1A@?0A@EA@bad_array_new_length@std@@8" = comdat any

$"??_R1A@?0A@EA@bad_alloc@std@@8" = comdat any

$"??_R3bad_alloc@std@@8" = comdat any

$"??_R2bad_alloc@std@@8" = comdat any

$"??_R1A@?0A@EA@exception@std@@8" = comdat any

$"??_R3exception@std@@8" = comdat any

$"??_R2exception@std@@8" = comdat any

$"??_7bad_alloc@std@@6B@" = comdat largest

$"??_R4bad_alloc@std@@6B@" = comdat any

$"??_7exception@std@@6B@" = comdat largest

$"??_R4exception@std@@6B@" = comdat any

$"??_C@_0BC@EOODALEL@Unknown?5exception?$AA@" = comdat any

$"?_OptionsStorage@?1??__local_stdio_printf_options@@9@4_KA" = comdat any

$"??_C@_0BA@JFNIOLAK@string?5too?5long?$AA@" = comdat any

$"??_C@_0O@NKNMEGII@list?5too?5long?$AA@" = comdat any

@std_check_point_recorder = dso_local local_unnamed_addr global ptr null, align 8
@"??_C@_0BO@EHAFBIND@Zeta_CheckPointRecorder_Print?$AA@" = linkonce_odr dso_local unnamed_addr constant [30 x i8] c"Zeta_CheckPointRecorder_Print\00", comdat, align 1
@"??_C@_0CL@DIKKEIGP@D?3?1ZetaDevelop?1Zeta?1CheckPointRe@" = linkonce_odr dso_local unnamed_addr constant [43 x i8] c"D:/ZetaDevelop/Zeta/CheckPointRecorder.cpp\00", comdat, align 1
@"??_C@_0BC@CENDOPBP@?$CF48s?3?$CF?94llu?7?$CF?924s?$AA@" = linkonce_odr dso_local unnamed_addr constant [18 x i8] c"%48s:%-4llu\09%-24s\00", comdat, align 1
@"??_C@_08LFPIFFEP@cpr?5?$CB?$DN?50?$AA@" = linkonce_odr dso_local unnamed_addr constant [9 x i8] c"cpr != 0\00", comdat, align 1
@"??_C@_0M@BJGCKIIG@?7assert?7?$CFs?6?$AA@" = linkonce_odr dso_local unnamed_addr constant [12 x i8] c"\09assert\09%s\0A\00", comdat, align 1
@zeta_debugger_assert_stage = external dso_local local_unnamed_addr global i8, align 1
@zeta_debugger_assert_callback = external dso_local local_unnamed_addr global ptr, align 8
@zeta_debugger_assert_callback_context = external dso_local local_unnamed_addr global ptr, align 8
@"??_C@_0BP@OIDFFGNC@Zeta_InitStdCheckPointRecorder?$AA@" = linkonce_odr dso_local unnamed_addr constant [31 x i8] c"Zeta_InitStdCheckPointRecorder\00", comdat, align 1
@"??_C@_0BM@CMKDHNKC@?$CBzeta_debugger_assert_stage?$AA@" = linkonce_odr dso_local unnamed_addr constant [28 x i8] c"!zeta_debugger_assert_stage\00", comdat, align 1
@"??_C@_0CD@KGPBLEGJ@zeta_debugger_assert_callback?5?$DN?$DN@" = linkonce_odr dso_local unnamed_addr constant [35 x i8] c"zeta_debugger_assert_callback == 0\00", comdat, align 1
@"??_7type_info@@6B@" = external constant ptr
@"??_R0?AVbad_array_new_length@std@@@8" = linkonce_odr global %rtti.TypeDescriptor30 { ptr @"??_7type_info@@6B@", ptr null, [31 x i8] c".?AVbad_array_new_length@std@@\00" }, comdat
@__ImageBase = external dso_local constant i8
@"??_R0?AVbad_alloc@std@@@8" = linkonce_odr global %rtti.TypeDescriptor19 { ptr @"??_7type_info@@6B@", ptr null, [20 x i8] c".?AVbad_alloc@std@@\00" }, comdat
@"??_R0?AVexception@std@@@8" = linkonce_odr global %rtti.TypeDescriptor19 { ptr @"??_7type_info@@6B@", ptr null, [20 x i8] c".?AVexception@std@@\00" }, comdat
@0 = private unnamed_addr constant { [3 x ptr] } { [3 x ptr] [ptr @"??_R4bad_array_new_length@std@@6B@", ptr @"??_Gbad_array_new_length@std@@UEAAPEAXI@Z", ptr @"?what@exception@std@@UEBAPEBDXZ"] }, comdat($"??_7bad_array_new_length@std@@6B@")
@"??_R4bad_array_new_length@std@@6B@" = linkonce_odr constant %rtti.CompleteObjectLocator { i32 1, i32 0, i32 0, i32 trunc (i64 sub nuw nsw (i64 ptrtoint (ptr @"??_R0?AVbad_array_new_length@std@@@8" to i64), i64 ptrtoint (ptr @__ImageBase to i64)) to i32), i32 trunc (i64 sub nuw nsw (i64 ptrtoint (ptr @"??_R3bad_array_new_length@std@@8" to i64), i64 ptrtoint (ptr @__ImageBase to i64)) to i32), i32 trunc (i64 sub nuw nsw (i64 ptrtoint (ptr @"??_R4bad_array_new_length@std@@6B@" to i64), i64 ptrtoint (ptr @__ImageBase to i64)) to i32) }, comdat
@"??_R3bad_array_new_length@std@@8" = linkonce_odr constant %rtti.ClassHierarchyDescriptor { i32 0, i32 0, i32 3, i32 trunc (i64 sub nuw nsw (i64 ptrtoint (ptr @"??_R2bad_array_new_length@std@@8" to i64), i64 ptrtoint (ptr @__ImageBase to i64)) to i32) }, comdat
@"??_R2bad_array_new_length@std@@8" = linkonce_odr constant [4 x i32] [i32 trunc (i64 sub nuw nsw (i64 ptrtoint (ptr @"??_R1A@?0A@EA@bad_array_new_length@std@@8" to i64), i64 ptrtoint (ptr @__ImageBase to i64)) to i32), i32 trunc (i64 sub nuw nsw (i64 ptrtoint (ptr @"??_R1A@?0A@EA@bad_alloc@std@@8" to i64), i64 ptrtoint (ptr @__ImageBase to i64)) to i32), i32 trunc (i64 sub nuw nsw (i64 ptrtoint (ptr @"??_R1A@?0A@EA@exception@std@@8" to i64), i64 ptrtoint (ptr @__ImageBase to i64)) to i32), i32 0], comdat
@"??_R1A@?0A@EA@bad_array_new_length@std@@8" = linkonce_odr constant %rtti.BaseClassDescriptor { i32 trunc (i64 sub nuw nsw (i64 ptrtoint (ptr @"??_R0?AVbad_array_new_length@std@@@8" to i64), i64 ptrtoint (ptr @__ImageBase to i64)) to i32), i32 2, i32 0, i32 -1, i32 0, i32 64, i32 trunc (i64 sub nuw nsw (i64 ptrtoint (ptr @"??_R3bad_array_new_length@std@@8" to i64), i64 ptrtoint (ptr @__ImageBase to i64)) to i32) }, comdat
@"??_R1A@?0A@EA@bad_alloc@std@@8" = linkonce_odr constant %rtti.BaseClassDescriptor { i32 trunc (i64 sub nuw nsw (i64 ptrtoint (ptr @"??_R0?AVbad_alloc@std@@@8" to i64), i64 ptrtoint (ptr @__ImageBase to i64)) to i32), i32 1, i32 0, i32 -1, i32 0, i32 64, i32 trunc (i64 sub nuw nsw (i64 ptrtoint (ptr @"??_R3bad_alloc@std@@8" to i64), i64 ptrtoint (ptr @__ImageBase to i64)) to i32) }, comdat
@"??_R3bad_alloc@std@@8" = linkonce_odr constant %rtti.ClassHierarchyDescriptor { i32 0, i32 0, i32 2, i32 trunc (i64 sub nuw nsw (i64 ptrtoint (ptr @"??_R2bad_alloc@std@@8" to i64), i64 ptrtoint (ptr @__ImageBase to i64)) to i32) }, comdat
@"??_R2bad_alloc@std@@8" = linkonce_odr constant [3 x i32] [i32 trunc (i64 sub nuw nsw (i64 ptrtoint (ptr @"??_R1A@?0A@EA@bad_alloc@std@@8" to i64), i64 ptrtoint (ptr @__ImageBase to i64)) to i32), i32 trunc (i64 sub nuw nsw (i64 ptrtoint (ptr @"??_R1A@?0A@EA@exception@std@@8" to i64), i64 ptrtoint (ptr @__ImageBase to i64)) to i32), i32 0], comdat
@"??_R1A@?0A@EA@exception@std@@8" = linkonce_odr constant %rtti.BaseClassDescriptor { i32 trunc (i64 sub nuw nsw (i64 ptrtoint (ptr @"??_R0?AVexception@std@@@8" to i64), i64 ptrtoint (ptr @__ImageBase to i64)) to i32), i32 0, i32 0, i32 -1, i32 0, i32 64, i32 trunc (i64 sub nuw nsw (i64 ptrtoint (ptr @"??_R3exception@std@@8" to i64), i64 ptrtoint (ptr @__ImageBase to i64)) to i32) }, comdat
@"??_R3exception@std@@8" = linkonce_odr constant %rtti.ClassHierarchyDescriptor { i32 0, i32 0, i32 1, i32 trunc (i64 sub nuw nsw (i64 ptrtoint (ptr @"??_R2exception@std@@8" to i64), i64 ptrtoint (ptr @__ImageBase to i64)) to i32) }, comdat
@"??_R2exception@std@@8" = linkonce_odr constant [2 x i32] [i32 trunc (i64 sub nuw nsw (i64 ptrtoint (ptr @"??_R1A@?0A@EA@exception@std@@8" to i64), i64 ptrtoint (ptr @__ImageBase to i64)) to i32), i32 0], comdat
@1 = private unnamed_addr constant { [3 x ptr] } { [3 x ptr] [ptr @"??_R4bad_alloc@std@@6B@", ptr @"??_Gbad_alloc@std@@UEAAPEAXI@Z", ptr @"?what@exception@std@@UEBAPEBDXZ"] }, comdat($"??_7bad_alloc@std@@6B@")
@"??_R4bad_alloc@std@@6B@" = linkonce_odr constant %rtti.CompleteObjectLocator { i32 1, i32 0, i32 0, i32 trunc (i64 sub nuw nsw (i64 ptrtoint (ptr @"??_R0?AVbad_alloc@std@@@8" to i64), i64 ptrtoint (ptr @__ImageBase to i64)) to i32), i32 trunc (i64 sub nuw nsw (i64 ptrtoint (ptr @"??_R3bad_alloc@std@@8" to i64), i64 ptrtoint (ptr @__ImageBase to i64)) to i32), i32 trunc (i64 sub nuw nsw (i64 ptrtoint (ptr @"??_R4bad_alloc@std@@6B@" to i64), i64 ptrtoint (ptr @__ImageBase to i64)) to i32) }, comdat
@2 = private unnamed_addr constant { [3 x ptr] } { [3 x ptr] [ptr @"??_R4exception@std@@6B@", ptr @"??_Gexception@std@@UEAAPEAXI@Z", ptr @"?what@exception@std@@UEBAPEBDXZ"] }, comdat($"??_7exception@std@@6B@")
@"??_R4exception@std@@6B@" = linkonce_odr constant %rtti.CompleteObjectLocator { i32 1, i32 0, i32 0, i32 trunc (i64 sub nuw nsw (i64 ptrtoint (ptr @"??_R0?AVexception@std@@@8" to i64), i64 ptrtoint (ptr @__ImageBase to i64)) to i32), i32 trunc (i64 sub nuw nsw (i64 ptrtoint (ptr @"??_R3exception@std@@8" to i64), i64 ptrtoint (ptr @__ImageBase to i64)) to i32), i32 trunc (i64 sub nuw nsw (i64 ptrtoint (ptr @"??_R4exception@std@@6B@" to i64), i64 ptrtoint (ptr @__ImageBase to i64)) to i32) }, comdat
@"??_C@_0BC@EOODALEL@Unknown?5exception?$AA@" = linkonce_odr dso_local unnamed_addr constant [18 x i8] c"Unknown exception\00", comdat, align 1
@"?_OptionsStorage@?1??__local_stdio_printf_options@@9@4_KA" = linkonce_odr dso_local global i64 0, comdat, align 8
@"??_C@_0BA@JFNIOLAK@string?5too?5long?$AA@" = linkonce_odr dso_local unnamed_addr constant [16 x i8] c"string too long\00", comdat, align 1
@"??_C@_0O@NKNMEGII@list?5too?5long?$AA@" = linkonce_odr dso_local unnamed_addr constant [14 x i8] c"list too long\00", comdat, align 1
@str = private unnamed_addr constant [28 x i8] c"check point recorder print:\00", align 1

@"??_7bad_array_new_length@std@@6B@" = unnamed_addr alias ptr, getelementptr inbounds ({ [3 x ptr] }, ptr @0, i32 0, i32 0, i32 1)
@"??_7bad_alloc@std@@6B@" = unnamed_addr alias ptr, getelementptr inbounds ({ [3 x ptr] }, ptr @1, i32 0, i32 0, i32 1)
@"??_7exception@std@@6B@" = unnamed_addr alias ptr, getelementptr inbounds ({ [3 x ptr] }, ptr @2, i32 0, i32 0, i32 1)

; Function Attrs: mustprogress uwtable
define dso_local noundef nonnull ptr @"?Zeta_CheckPointRecorder_Create@@YAPEAXXZ"() local_unnamed_addr #0 personality ptr @__CxxFrameHandler3 {
  %1 = tail call noalias noundef nonnull dereferenceable(24) ptr @"??2@YAPEAX_K@Z"(i64 noundef 24) #18
  %2 = getelementptr inbounds %struct.Zeta_CheckPointRecorder, ptr %1, i64 0, i32 1
  tail call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %2, i8 0, i64 16, i1 false)
  %3 = invoke noalias noundef nonnull dereferenceable(88) ptr @"??2@YAPEAX_K@Z"(i64 noundef 88) #19
          to label %4 unwind label %6

4:                                                ; preds = %0
  store ptr %3, ptr %3, align 8, !tbaa !12
  %5 = getelementptr inbounds %"struct.std::_List_node", ptr %3, i64 0, i32 1
  store ptr %3, ptr %5, align 8, !tbaa !12
  store ptr %3, ptr %2, align 8, !tbaa !16
  store i64 1024, ptr %1, align 8, !tbaa !21
  ret ptr %1

6:                                                ; preds = %0
  %7 = cleanuppad within none []
  call void @"??3@YAXPEAX@Z"(ptr noundef nonnull %1) #20 [ "funclet"(token %7) ]
  cleanupret from %7 unwind to caller
}

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.start.p0(i64 immarg, ptr nocapture) #1

; Function Attrs: nobuiltin allocsize(0)
declare dso_local noundef nonnull ptr @"??2@YAPEAX_K@Z"(i64 noundef) local_unnamed_addr #2

declare dso_local i32 @__CxxFrameHandler3(...)

; Function Attrs: nobuiltin nounwind
declare dso_local void @"??3@YAXPEAX@Z"(ptr noundef) local_unnamed_addr #3

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.end.p0(i64 immarg, ptr nocapture) #1

; Function Attrs: mustprogress nounwind uwtable
define dso_local void @Zeta_CheckPointRecorder_Destroy(ptr noundef %0) local_unnamed_addr #4 personality ptr @__CxxFrameHandler3 {
  %2 = icmp eq ptr %0, null
  br i1 %2, label %19, label %3

3:                                                ; preds = %1
  %4 = getelementptr inbounds %struct.Zeta_CheckPointRecorder, ptr %0, i64 0, i32 1
  %5 = load ptr, ptr %4, align 8, !tbaa !23
  %6 = getelementptr inbounds %"struct.std::_List_node", ptr %5, i64 0, i32 1
  %7 = load ptr, ptr %6, align 8, !tbaa !24
  store ptr null, ptr %7, align 8, !tbaa !30
  %8 = load ptr, ptr %5, align 8, !tbaa !30
  %9 = icmp eq ptr %8, null
  br i1 %9, label %17, label %10

10:                                               ; preds = %3, %10
  %11 = phi ptr [ %12, %10 ], [ %8, %3 ]
  %12 = load ptr, ptr %11, align 8, !tbaa !30
  %13 = getelementptr inbounds %"struct.std::_List_node", ptr %11, i64 0, i32 2
  tail call void @"??1Record@@QEAA@XZ"(ptr noundef nonnull align 8 dereferenceable(72) %13) #21
  tail call void @"??3@YAXPEAX_K@Z"(ptr noundef nonnull %11, i64 noundef 88) #21
  %14 = icmp eq ptr %12, null
  br i1 %14, label %15, label %10, !llvm.loop !31

15:                                               ; preds = %10
  %16 = load ptr, ptr %4, align 8, !tbaa !23
  br label %17

17:                                               ; preds = %3, %15
  %18 = phi ptr [ %16, %15 ], [ %5, %3 ]
  tail call void @"??3@YAXPEAX_K@Z"(ptr noundef %18, i64 noundef 88) #21
  tail call void @"??3@YAXPEAX@Z"(ptr noundef nonnull %0) #20
  br label %19

19:                                               ; preds = %17, %1
  ret void
}

; Function Attrs: mustprogress uwtable
define dso_local void @Zeta_CheckPointRecorder_Record(ptr nocapture noundef %0, ptr nocapture noundef readonly %1, i64 noundef %2, ptr nocapture noundef readonly %3) local_unnamed_addr #0 personality ptr @__CxxFrameHandler3 {
  %5 = alloca %struct.Record, align 8
  %6 = getelementptr inbounds %struct.Zeta_CheckPointRecorder, ptr %0, i64 0, i32 1
  %7 = getelementptr inbounds %struct.Zeta_CheckPointRecorder, ptr %0, i64 0, i32 1, i32 0, i32 0, i32 1
  %8 = load i64, ptr %0, align 8, !tbaa !21
  %9 = load i64, ptr %7, align 8, !tbaa !33
  %10 = icmp ugt i64 %8, %9
  br i1 %10, label %24, label %11

11:                                               ; preds = %4, %11
  %12 = phi i64 [ %22, %11 ], [ %9, %4 ]
  %13 = load ptr, ptr %6, align 8, !tbaa !16
  %14 = load ptr, ptr %13, align 8, !tbaa !30
  %15 = load ptr, ptr %14, align 8, !tbaa !30
  %16 = add i64 %12, -1
  store i64 %16, ptr %7, align 8, !tbaa !33
  %17 = getelementptr inbounds %"struct.std::_List_node", ptr %14, i64 0, i32 1
  %18 = load ptr, ptr %17, align 8, !tbaa !24
  store ptr %15, ptr %18, align 8, !tbaa !30
  %19 = getelementptr inbounds %"struct.std::_List_node", ptr %15, i64 0, i32 1
  store ptr %18, ptr %19, align 8, !tbaa !24
  %20 = getelementptr inbounds %"struct.std::_List_node", ptr %14, i64 0, i32 2
  tail call void @"??1Record@@QEAA@XZ"(ptr noundef nonnull align 8 dereferenceable(72) %20) #21
  tail call void @"??3@YAXPEAX_K@Z"(ptr noundef %14, i64 noundef 88) #21
  %21 = load i64, ptr %0, align 8, !tbaa !21
  %22 = load i64, ptr %7, align 8, !tbaa !33
  %23 = icmp ugt i64 %21, %22
  br i1 %23, label %24, label %11, !llvm.loop !34

24:                                               ; preds = %11, %4
  call void @llvm.lifetime.start.p0(i64 72, ptr nonnull %5) #21
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(32) %5, i8 0, i64 32, i1 false)
  %25 = tail call noundef i64 @strlen(ptr noundef nonnull dereferenceable(1) %1) #21
  %26 = icmp slt i64 %25, 0
  br i1 %26, label %27, label %28

27:                                               ; preds = %24
  tail call void @"?_Xlen_string@std@@YAXXZ"() #22
  unreachable

28:                                               ; preds = %24
  %29 = icmp ult i64 %25, 16
  br i1 %29, label %30, label %34

30:                                               ; preds = %28
  %31 = getelementptr inbounds %"class.std::_String_val", ptr %5, i64 0, i32 1
  store i64 %25, ptr %31, align 8, !tbaa !35
  %32 = getelementptr inbounds %"class.std::_String_val", ptr %5, i64 0, i32 2
  store i64 15, ptr %32, align 8, !tbaa !36
  call void @llvm.memcpy.p0.p0.i64(ptr nonnull align 8 %5, ptr align 1 %1, i64 %25, i1 false)
  %33 = getelementptr inbounds [16 x i8], ptr %5, i64 0, i64 %25
  br label %54

34:                                               ; preds = %28
  %35 = getelementptr inbounds %"class.std::_String_val", ptr %5, i64 0, i32 2
  %36 = or i64 %25, 15
  %37 = tail call i64 @llvm.umax.i64(i64 %36, i64 22)
  %38 = icmp ugt i64 %36, 4094
  br i1 %38, label %39, label %47

39:                                               ; preds = %34
  %40 = add nuw i64 %37, 40
  %41 = tail call noalias noundef nonnull ptr @"??2@YAPEAX_K@Z"(i64 noundef %40) #19
  %42 = ptrtoint ptr %41 to i64
  %43 = add i64 %42, 39
  %44 = and i64 %43, -32
  %45 = inttoptr i64 %44 to ptr
  %46 = getelementptr inbounds i64, ptr %45, i64 -1
  store i64 %42, ptr %46, align 8, !tbaa !37
  br label %50

47:                                               ; preds = %34
  %48 = add nuw nsw i64 %37, 1
  %49 = tail call noalias noundef nonnull ptr @"??2@YAPEAX_K@Z"(i64 noundef %48) #19
  br label %50

50:                                               ; preds = %47, %39
  %51 = phi ptr [ %45, %39 ], [ %49, %47 ]
  store ptr %51, ptr %5, align 8, !tbaa !12
  %52 = getelementptr inbounds %"class.std::_String_val", ptr %5, i64 0, i32 1
  store i64 %25, ptr %52, align 8, !tbaa !35
  store i64 %37, ptr %35, align 8, !tbaa !36
  tail call void @llvm.memcpy.p0.p0.i64(ptr nonnull align 1 %51, ptr align 1 %1, i64 %25, i1 false)
  %53 = getelementptr inbounds i8, ptr %51, i64 %25
  br label %54

54:                                               ; preds = %30, %50
  %55 = phi ptr [ %53, %50 ], [ %33, %30 ]
  store i8 0, ptr %55, align 1, !tbaa !38
  %56 = getelementptr inbounds %struct.Record, ptr %5, i64 0, i32 1
  store i64 %2, ptr %56, align 8, !tbaa !39
  %57 = getelementptr inbounds %struct.Record, ptr %5, i64 0, i32 2
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(32) %57, i8 0, i64 32, i1 false)
  %58 = tail call noundef i64 @strlen(ptr noundef nonnull dereferenceable(1) %3) #21
  %59 = icmp slt i64 %58, 0
  br i1 %59, label %60, label %62

60:                                               ; preds = %54
  invoke void @"?_Xlen_string@std@@YAXXZ"() #22
          to label %61 unwind label %108

61:                                               ; preds = %60
  unreachable

62:                                               ; preds = %54
  %63 = icmp ult i64 %58, 16
  br i1 %63, label %64, label %68

64:                                               ; preds = %62
  %65 = getelementptr inbounds %struct.Record, ptr %5, i64 0, i32 2, i32 0, i32 0, i32 1
  store i64 %58, ptr %65, align 8, !tbaa !35
  %66 = getelementptr inbounds %struct.Record, ptr %5, i64 0, i32 2, i32 0, i32 0, i32 2
  store i64 15, ptr %66, align 8, !tbaa !36
  call void @llvm.memcpy.p0.p0.i64(ptr nonnull align 8 %57, ptr align 1 %3, i64 %58, i1 false)
  %67 = getelementptr inbounds [16 x i8], ptr %57, i64 0, i64 %58
  br label %89

68:                                               ; preds = %62
  %69 = getelementptr inbounds %struct.Record, ptr %5, i64 0, i32 2, i32 0, i32 0, i32 2
  store i64 15, ptr %69, align 8, !tbaa !36
  %70 = or i64 %58, 15
  %71 = tail call i64 @llvm.umax.i64(i64 %70, i64 22)
  %72 = icmp ugt i64 %70, 4094
  br i1 %72, label %73, label %82

73:                                               ; preds = %68
  %74 = add nuw i64 %71, 40
  %75 = invoke noalias noundef nonnull ptr @"??2@YAPEAX_K@Z"(i64 noundef %74) #19
          to label %76 unwind label %108

76:                                               ; preds = %73
  %77 = ptrtoint ptr %75 to i64
  %78 = add i64 %77, 39
  %79 = and i64 %78, -32
  %80 = inttoptr i64 %79 to ptr
  %81 = getelementptr inbounds i64, ptr %80, i64 -1
  store i64 %77, ptr %81, align 8, !tbaa !37
  br label %85

82:                                               ; preds = %68
  %83 = add nuw nsw i64 %71, 1
  %84 = invoke noalias noundef nonnull ptr @"??2@YAPEAX_K@Z"(i64 noundef %83) #19
          to label %85 unwind label %108

85:                                               ; preds = %82, %76
  %86 = phi ptr [ %80, %76 ], [ %84, %82 ]
  store ptr %86, ptr %57, align 8, !tbaa !12
  %87 = getelementptr inbounds %struct.Record, ptr %5, i64 0, i32 2, i32 0, i32 0, i32 1
  store i64 %58, ptr %87, align 8, !tbaa !35
  store i64 %71, ptr %69, align 8, !tbaa !36
  tail call void @llvm.memcpy.p0.p0.i64(ptr nonnull align 1 %86, ptr align 1 %3, i64 %58, i1 false)
  %88 = getelementptr inbounds i8, ptr %86, i64 %58
  br label %89

89:                                               ; preds = %85, %64
  %90 = phi ptr [ %88, %85 ], [ %67, %64 ]
  store i8 0, ptr %90, align 1, !tbaa !38
  %91 = load i64, ptr %7, align 8, !tbaa !37
  %92 = icmp eq i64 %91, 209622091746699450
  br i1 %92, label %93, label %95

93:                                               ; preds = %89
  invoke void @"?_Xlength_error@std@@YAXPEBD@Z"(ptr noundef nonnull @"??_C@_0O@NKNMEGII@list?5too?5long?$AA@") #22
          to label %94 unwind label %110

94:                                               ; preds = %93
  unreachable

95:                                               ; preds = %89
  %96 = load ptr, ptr %6, align 8, !tbaa !16
  %97 = invoke noalias noundef nonnull dereferenceable(88) ptr @"??2@YAPEAX_K@Z"(i64 noundef 88) #19
          to label %98 unwind label %110

98:                                               ; preds = %95
  %99 = getelementptr inbounds %"struct.std::_List_node", ptr %97, i64 0, i32 2
  call void @llvm.memcpy.p0.p0.i64(ptr noundef nonnull align 8 dereferenceable(32) %99, ptr noundef nonnull align 8 dereferenceable(32) %5, i64 32, i1 false)
  %100 = getelementptr inbounds %"struct.std::_List_node", ptr %97, i64 0, i32 2, i32 1
  %101 = load i64, ptr %56, align 8, !tbaa !39
  store i64 %101, ptr %100, align 8, !tbaa !39
  %102 = getelementptr inbounds %"struct.std::_List_node", ptr %97, i64 0, i32 2, i32 2
  call void @llvm.memcpy.p0.p0.i64(ptr noundef nonnull align 8 dereferenceable(32) %102, ptr noundef nonnull align 8 dereferenceable(32) %57, i64 32, i1 false)
  store i8 0, ptr %57, align 8, !tbaa !38
  %103 = load i64, ptr %7, align 8, !tbaa !37
  %104 = add i64 %103, 1
  store i64 %104, ptr %7, align 8, !tbaa !37
  %105 = getelementptr inbounds %"struct.std::_List_node", ptr %96, i64 0, i32 1
  %106 = load ptr, ptr %105, align 8, !tbaa !24
  store ptr %96, ptr %97, align 8, !tbaa !12
  %107 = getelementptr inbounds %"struct.std::_List_node", ptr %97, i64 0, i32 1
  store ptr %106, ptr %107, align 8, !tbaa !12
  store ptr %97, ptr %105, align 8, !tbaa !24
  store ptr %97, ptr %106, align 8, !tbaa !30
  call void @llvm.lifetime.end.p0(i64 72, ptr nonnull %5) #21
  ret void

108:                                              ; preds = %82, %73, %60
  %109 = cleanuppad within none []
  call void @"??1?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@QEAA@XZ"(ptr noundef nonnull align 8 dereferenceable(32) %5) #21 [ "funclet"(token %109) ]
  cleanupret from %109 unwind to caller

110:                                              ; preds = %95, %93
  %111 = cleanuppad within none []
  call void @"??1Record@@QEAA@XZ"(ptr noundef nonnull align 8 dereferenceable(72) %5) #21 [ "funclet"(token %111) ]
  cleanupret from %111 unwind to caller
}

; Function Attrs: inlinehint mustprogress nounwind uwtable
define linkonce_odr dso_local void @"??1?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@QEAA@XZ"(ptr noundef nonnull align 8 dereferenceable(32) %0) unnamed_addr #5 comdat align 2 personality ptr @__CxxFrameHandler3 {
  %2 = getelementptr inbounds %"class.std::_String_val", ptr %0, i64 0, i32 2
  %3 = load i64, ptr %2, align 8, !tbaa !36
  %4 = icmp ugt i64 %3, 15
  br i1 %4, label %5, label %26

5:                                                ; preds = %1
  %6 = load ptr, ptr %0, align 8, !tbaa !38
  %7 = add i64 %3, 1
  %8 = icmp ugt i64 %7, 4095
  br i1 %8, label %9, label %23

9:                                                ; preds = %5
  %10 = getelementptr inbounds i64, ptr %6, i64 -1
  %11 = load i64, ptr %10, align 8, !tbaa !37
  %12 = ptrtoint ptr %6 to i64
  %13 = add i64 %12, -8
  %14 = sub i64 %13, %11
  %15 = icmp ult i64 %14, 32
  br i1 %15, label %18, label %16

16:                                               ; preds = %9
  invoke void @_invalid_parameter_noinfo_noreturn() #22
          to label %17 unwind label %21

17:                                               ; preds = %16
  unreachable

18:                                               ; preds = %9
  %19 = add i64 %3, 40
  %20 = inttoptr i64 %11 to ptr
  br label %23

21:                                               ; preds = %16
  %22 = cleanuppad within none []
  call void @__std_terminate() #23 [ "funclet"(token %22) ]
  unreachable

23:                                               ; preds = %18, %5
  %24 = phi i64 [ %19, %18 ], [ %7, %5 ]
  %25 = phi ptr [ %20, %18 ], [ %6, %5 ]
  tail call void @"??3@YAXPEAX_K@Z"(ptr noundef %25, i64 noundef %24) #21
  br label %26

26:                                               ; preds = %1, %23
  %27 = getelementptr inbounds %"class.std::_String_val", ptr %0, i64 0, i32 1
  store i64 0, ptr %27, align 8, !tbaa !35
  store i64 15, ptr %2, align 8, !tbaa !36
  store i8 0, ptr %0, align 8, !tbaa !38
  ret void
}

; Function Attrs: inlinehint mustprogress nounwind uwtable
define linkonce_odr dso_local void @"??1Record@@QEAA@XZ"(ptr noundef nonnull align 8 dereferenceable(72) %0) unnamed_addr #5 comdat align 2 personality ptr @__CxxFrameHandler3 {
  %2 = getelementptr inbounds %struct.Record, ptr %0, i64 0, i32 2
  %3 = getelementptr inbounds %struct.Record, ptr %0, i64 0, i32 2, i32 0, i32 0, i32 2
  %4 = load i64, ptr %3, align 8, !tbaa !36
  %5 = icmp ugt i64 %4, 15
  br i1 %5, label %6, label %27

6:                                                ; preds = %1
  %7 = load ptr, ptr %2, align 8, !tbaa !38
  %8 = add i64 %4, 1
  %9 = icmp ugt i64 %8, 4095
  br i1 %9, label %10, label %24

10:                                               ; preds = %6
  %11 = getelementptr inbounds i64, ptr %7, i64 -1
  %12 = load i64, ptr %11, align 8, !tbaa !37
  %13 = ptrtoint ptr %7 to i64
  %14 = add i64 %13, -8
  %15 = sub i64 %14, %12
  %16 = icmp ult i64 %15, 32
  br i1 %16, label %19, label %17

17:                                               ; preds = %10
  invoke void @_invalid_parameter_noinfo_noreturn() #22
          to label %18 unwind label %22

18:                                               ; preds = %17
  unreachable

19:                                               ; preds = %10
  %20 = add i64 %4, 40
  %21 = inttoptr i64 %12 to ptr
  br label %24

22:                                               ; preds = %17
  %23 = cleanuppad within none []
  call void @__std_terminate() #23 [ "funclet"(token %23) ]
  unreachable

24:                                               ; preds = %19, %6
  %25 = phi i64 [ %20, %19 ], [ %8, %6 ]
  %26 = phi ptr [ %21, %19 ], [ %7, %6 ]
  tail call void @"??3@YAXPEAX_K@Z"(ptr noundef %26, i64 noundef %25) #21
  br label %27

27:                                               ; preds = %1, %24
  %28 = getelementptr inbounds %struct.Record, ptr %0, i64 0, i32 2, i32 0, i32 0, i32 1
  store i64 0, ptr %28, align 8, !tbaa !35
  store i64 15, ptr %3, align 8, !tbaa !36
  store i8 0, ptr %2, align 8, !tbaa !38
  %29 = getelementptr inbounds %"class.std::_String_val", ptr %0, i64 0, i32 2
  %30 = load i64, ptr %29, align 8, !tbaa !36
  %31 = icmp ugt i64 %30, 15
  br i1 %31, label %32, label %53

32:                                               ; preds = %27
  %33 = load ptr, ptr %0, align 8, !tbaa !38
  %34 = add i64 %30, 1
  %35 = icmp ugt i64 %34, 4095
  br i1 %35, label %36, label %50

36:                                               ; preds = %32
  %37 = getelementptr inbounds i64, ptr %33, i64 -1
  %38 = load i64, ptr %37, align 8, !tbaa !37
  %39 = ptrtoint ptr %33 to i64
  %40 = add i64 %39, -8
  %41 = sub i64 %40, %38
  %42 = icmp ult i64 %41, 32
  br i1 %42, label %45, label %43

43:                                               ; preds = %36
  invoke void @_invalid_parameter_noinfo_noreturn() #22
          to label %44 unwind label %48

44:                                               ; preds = %43
  unreachable

45:                                               ; preds = %36
  %46 = add i64 %30, 40
  %47 = inttoptr i64 %38 to ptr
  br label %50

48:                                               ; preds = %43
  %49 = cleanuppad within none []
  call void @__std_terminate() #23 [ "funclet"(token %49) ]
  unreachable

50:                                               ; preds = %45, %32
  %51 = phi i64 [ %46, %45 ], [ %34, %32 ]
  %52 = phi ptr [ %47, %45 ], [ %33, %32 ]
  tail call void @"??3@YAXPEAX_K@Z"(ptr noundef %52, i64 noundef %51) #21
  br label %53

53:                                               ; preds = %27, %50
  %54 = getelementptr inbounds %"class.std::_String_val", ptr %0, i64 0, i32 1
  store i64 0, ptr %54, align 8, !tbaa !35
  store i64 15, ptr %29, align 8, !tbaa !36
  store i8 0, ptr %0, align 8, !tbaa !38
  ret void
}

; Function Attrs: mustprogress uwtable
define dso_local void @Zeta_CheckPointRecorder_Print(ptr noundef readonly %0) local_unnamed_addr #0 {
  %2 = icmp eq ptr %0, null
  br i1 %2, label %3, label %16

3:                                                ; preds = %1
  %4 = tail call i32 (ptr, ...) @printf(ptr noundef nonnull dereferenceable(1) @"??_C@_0BC@CENDOPBP@?$CF48s?3?$CF?94llu?7?$CF?924s?$AA@", ptr noundef nonnull @"??_C@_0CL@DIKKEIGP@D?3?1ZetaDevelop?1Zeta?1CheckPointRe@", i64 noundef 45, ptr noundef nonnull @"??_C@_0BO@EHAFBIND@Zeta_CheckPointRecorder_Print?$AA@")
  %5 = tail call i32 (ptr, ...) @printf(ptr noundef nonnull dereferenceable(1) @"??_C@_0M@BJGCKIIG@?7assert?7?$CFs?6?$AA@", ptr noundef nonnull @"??_C@_08LFPIFFEP@cpr?5?$CB?$DN?50?$AA@")
  %6 = load i8, ptr @zeta_debugger_assert_stage, align 1, !tbaa !40, !range !42, !noundef !43
  %7 = icmp eq i8 %6, 0
  br i1 %7, label %8, label %14

8:                                                ; preds = %3
  store i8 1, ptr @zeta_debugger_assert_stage, align 1, !tbaa !40
  %9 = load ptr, ptr @zeta_debugger_assert_callback, align 8, !tbaa !12
  %10 = icmp eq ptr %9, null
  br i1 %10, label %14, label %11

11:                                               ; preds = %8
  %12 = load ptr, ptr @zeta_debugger_assert_callback_context, align 8, !tbaa !12
  %13 = tail call i32 %9(ptr noundef %12)
  br label %14

14:                                               ; preds = %8, %11, %3
  %15 = tail call i32 @putchar(i32 7)
  tail call void @exit(i32 noundef 0) #22
  unreachable

16:                                               ; preds = %1
  %17 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str)
  %18 = getelementptr inbounds %struct.Zeta_CheckPointRecorder, ptr %0, i64 0, i32 1
  %19 = load ptr, ptr %18, align 8, !tbaa !16, !noalias !43
  %20 = load ptr, ptr %19, align 8, !tbaa !30
  %21 = icmp eq ptr %20, %19
  br i1 %21, label %22, label %23

22:                                               ; preds = %23, %16
  ret void

23:                                               ; preds = %16, %23
  %24 = phi ptr [ %41, %23 ], [ %20, %16 ]
  %25 = getelementptr inbounds %"struct.std::_List_node", ptr %24, i64 0, i32 2
  %26 = getelementptr inbounds %"struct.std::_List_node", ptr %24, i64 0, i32 2, i32 2
  %27 = getelementptr inbounds %"struct.std::_List_node", ptr %24, i64 0, i32 2, i32 2, i32 0, i32 0, i32 2
  %28 = load i64, ptr %27, align 8, !tbaa !36
  %29 = icmp ugt i64 %28, 15
  %30 = load ptr, ptr %26, align 8
  %31 = select i1 %29, ptr %30, ptr %26
  %32 = getelementptr inbounds %"struct.std::_List_node", ptr %24, i64 0, i32 2, i32 1
  %33 = load i64, ptr %32, align 8, !tbaa !39
  %34 = getelementptr inbounds %"struct.std::_List_node", ptr %24, i64 0, i32 2, i32 0, i32 0, i32 0, i32 2
  %35 = load i64, ptr %34, align 8, !tbaa !36
  %36 = icmp ugt i64 %35, 15
  %37 = load ptr, ptr %25, align 8
  %38 = select i1 %36, ptr %37, ptr %25
  %39 = tail call i32 (ptr, ...) @printf(ptr noundef nonnull dereferenceable(1) @"??_C@_0BC@CENDOPBP@?$CF48s?3?$CF?94llu?7?$CF?924s?$AA@", ptr noundef %38, i64 noundef %33, ptr noundef %31)
  %40 = tail call i32 @putchar(i32 10)
  %41 = load ptr, ptr %24, align 8, !tbaa !30
  %42 = icmp eq ptr %41, %19
  br i1 %42, label %22, label %23, !llvm.loop !44
}

; Function Attrs: inlinehint mustprogress uwtable
define linkonce_odr dso_local i32 @printf(ptr noundef %0, ...) local_unnamed_addr #6 comdat {
  %2 = alloca ptr, align 8
  call void @llvm.lifetime.start.p0(i64 8, ptr nonnull %2) #21
  call void @llvm.va_start(ptr nonnull %2)
  %3 = load ptr, ptr %2, align 8, !tbaa !12
  %4 = call ptr @__acrt_iob_func(i32 noundef 1)
  %5 = call ptr @__local_stdio_printf_options()
  %6 = load i64, ptr %5, align 8, !tbaa !37
  %7 = call i32 @__stdio_common_vfprintf(i64 noundef %6, ptr noundef %4, ptr noundef %0, ptr noundef null, ptr noundef %3)
  call void @llvm.va_end(ptr nonnull %2)
  call void @llvm.lifetime.end.p0(i64 8, ptr nonnull %2) #21
  ret i32 %7
}

; Function Attrs: noreturn
declare dso_local void @exit(i32 noundef) local_unnamed_addr #7

; Function Attrs: mustprogress uwtable
define dso_local void @Zeta_InitStdCheckPointRecorder() local_unnamed_addr #0 personality ptr @__CxxFrameHandler3 {
  %1 = load ptr, ptr @std_check_point_recorder, align 8, !tbaa !12
  %2 = icmp eq ptr %1, null
  br i1 %2, label %3, label %43

3:                                                ; preds = %0
  %4 = load i8, ptr @zeta_debugger_assert_stage, align 1, !tbaa !40, !range !42, !noundef !43
  %5 = icmp eq i8 %4, 0
  br i1 %5, label %19, label %6

6:                                                ; preds = %3
  %7 = tail call i32 (ptr, ...) @printf(ptr noundef nonnull dereferenceable(1) @"??_C@_0BC@CENDOPBP@?$CF48s?3?$CF?94llu?7?$CF?924s?$AA@", ptr noundef nonnull @"??_C@_0CL@DIKKEIGP@D?3?1ZetaDevelop?1Zeta?1CheckPointRe@", i64 noundef 66, ptr noundef nonnull @"??_C@_0BP@OIDFFGNC@Zeta_InitStdCheckPointRecorder?$AA@")
  %8 = tail call i32 (ptr, ...) @printf(ptr noundef nonnull dereferenceable(1) @"??_C@_0M@BJGCKIIG@?7assert?7?$CFs?6?$AA@", ptr noundef nonnull @"??_C@_0BM@CMKDHNKC@?$CBzeta_debugger_assert_stage?$AA@")
  %9 = load i8, ptr @zeta_debugger_assert_stage, align 1, !tbaa !40, !range !42, !noundef !43
  %10 = icmp eq i8 %9, 0
  br i1 %10, label %11, label %17

11:                                               ; preds = %6
  store i8 1, ptr @zeta_debugger_assert_stage, align 1, !tbaa !40
  %12 = load ptr, ptr @zeta_debugger_assert_callback, align 8, !tbaa !12
  %13 = icmp eq ptr %12, null
  br i1 %13, label %17, label %14

14:                                               ; preds = %11
  %15 = load ptr, ptr @zeta_debugger_assert_callback_context, align 8, !tbaa !12
  %16 = tail call i32 %12(ptr noundef %15)
  br label %17

17:                                               ; preds = %11, %14, %6
  %18 = tail call i32 @putchar(i32 7)
  tail call void @exit(i32 noundef 0) #22
  unreachable

19:                                               ; preds = %3
  %20 = load ptr, ptr @zeta_debugger_assert_callback, align 8, !tbaa !12
  %21 = icmp eq ptr %20, null
  br i1 %21, label %35, label %22

22:                                               ; preds = %19
  %23 = tail call i32 (ptr, ...) @printf(ptr noundef nonnull dereferenceable(1) @"??_C@_0BC@CENDOPBP@?$CF48s?3?$CF?94llu?7?$CF?924s?$AA@", ptr noundef nonnull @"??_C@_0CL@DIKKEIGP@D?3?1ZetaDevelop?1Zeta?1CheckPointRe@", i64 noundef 67, ptr noundef nonnull @"??_C@_0BP@OIDFFGNC@Zeta_InitStdCheckPointRecorder?$AA@")
  %24 = tail call i32 (ptr, ...) @printf(ptr noundef nonnull dereferenceable(1) @"??_C@_0M@BJGCKIIG@?7assert?7?$CFs?6?$AA@", ptr noundef nonnull @"??_C@_0CD@KGPBLEGJ@zeta_debugger_assert_callback?5?$DN?$DN@")
  %25 = load i8, ptr @zeta_debugger_assert_stage, align 1, !tbaa !40, !range !42, !noundef !43
  %26 = icmp eq i8 %25, 0
  br i1 %26, label %27, label %33

27:                                               ; preds = %22
  store i8 1, ptr @zeta_debugger_assert_stage, align 1, !tbaa !40
  %28 = load ptr, ptr @zeta_debugger_assert_callback, align 8, !tbaa !12
  %29 = icmp eq ptr %28, null
  br i1 %29, label %33, label %30

30:                                               ; preds = %27
  %31 = load ptr, ptr @zeta_debugger_assert_callback_context, align 8, !tbaa !12
  %32 = tail call i32 %28(ptr noundef %31)
  br label %33

33:                                               ; preds = %27, %30, %22
  %34 = tail call i32 @putchar(i32 7)
  tail call void @exit(i32 noundef 0) #22
  unreachable

35:                                               ; preds = %19
  %36 = tail call noalias noundef nonnull dereferenceable(24) ptr @"??2@YAPEAX_K@Z"(i64 noundef 24) #18
  %37 = getelementptr inbounds %struct.Zeta_CheckPointRecorder, ptr %36, i64 0, i32 1
  tail call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %37, i8 0, i64 16, i1 false)
  %38 = invoke noalias noundef nonnull dereferenceable(88) ptr @"??2@YAPEAX_K@Z"(i64 noundef 88) #19
          to label %41 unwind label %39

39:                                               ; preds = %35
  %40 = cleanuppad within none []
  call void @"??3@YAXPEAX@Z"(ptr noundef nonnull %36) #20 [ "funclet"(token %40) ]
  cleanupret from %40 unwind to caller

41:                                               ; preds = %35
  store ptr %38, ptr %38, align 8, !tbaa !12
  %42 = getelementptr inbounds %"struct.std::_List_node", ptr %38, i64 0, i32 1
  store ptr %38, ptr %42, align 8, !tbaa !12
  store ptr %38, ptr %37, align 8, !tbaa !16
  store i64 1024, ptr %36, align 8, !tbaa !21
  store ptr %36, ptr @std_check_point_recorder, align 8, !tbaa !12
  store ptr %36, ptr @zeta_debugger_assert_callback_context, align 8, !tbaa !12
  store ptr @"?StdCheckPointRecorderAssertCallback@@YAHPEAX@Z", ptr @zeta_debugger_assert_callback, align 8, !tbaa !12
  br label %43

43:                                               ; preds = %0, %41
  ret void
}

; Function Attrs: mustprogress uwtable
define internal noundef i32 @"?StdCheckPointRecorderAssertCallback@@YAHPEAX@Z"(ptr nocapture readnone %0) #0 {
  %2 = load ptr, ptr @std_check_point_recorder, align 8, !tbaa !12
  tail call void @Zeta_CheckPointRecorder_Print(ptr noundef %2)
  ret i32 0
}

; Function Attrs: noreturn
declare dso_local void @_invalid_parameter_noinfo_noreturn() local_unnamed_addr #7

; Function Attrs: inlinehint mustprogress nounwind uwtable
define linkonce_odr dso_local noundef ptr @"??_Gbad_array_new_length@std@@UEAAPEAXI@Z"(ptr noundef nonnull align 8 dereferenceable(24) %0, i32 noundef %1) unnamed_addr #5 comdat align 2 personality ptr @__CxxFrameHandler3 {
  store ptr @"??_7exception@std@@6B@", ptr %0, align 8, !tbaa !45
  %3 = getelementptr inbounds %"class.std::exception", ptr %0, i64 0, i32 1
  invoke void @__std_exception_destroy(ptr noundef nonnull %3)
          to label %6 unwind label %4

4:                                                ; preds = %2
  %5 = cleanuppad within none []
  call void @__std_terminate() #23 [ "funclet"(token %5) ]
  unreachable

6:                                                ; preds = %2
  %7 = icmp eq i32 %1, 0
  br i1 %7, label %9, label %8

8:                                                ; preds = %6
  tail call void @"??3@YAXPEAX@Z"(ptr noundef nonnull %0) #20
  br label %9

9:                                                ; preds = %8, %6
  ret ptr %0
}

; Function Attrs: mustprogress nounwind uwtable
define linkonce_odr dso_local noundef ptr @"?what@exception@std@@UEBAPEBDXZ"(ptr noundef nonnull align 8 dereferenceable(24) %0) unnamed_addr #4 comdat align 2 {
  %2 = getelementptr inbounds %"class.std::exception", ptr %0, i64 0, i32 1
  %3 = load ptr, ptr %2, align 8, !tbaa !47
  %4 = icmp eq ptr %3, null
  %5 = select i1 %4, ptr @"??_C@_0BC@EOODALEL@Unknown?5exception?$AA@", ptr %3
  ret ptr %5
}

; Function Attrs: inlinehint mustprogress nounwind uwtable
define linkonce_odr dso_local noundef ptr @"??_Gbad_alloc@std@@UEAAPEAXI@Z"(ptr noundef nonnull align 8 dereferenceable(24) %0, i32 noundef %1) unnamed_addr #5 comdat align 2 personality ptr @__CxxFrameHandler3 {
  store ptr @"??_7exception@std@@6B@", ptr %0, align 8, !tbaa !45
  %3 = getelementptr inbounds %"class.std::exception", ptr %0, i64 0, i32 1
  invoke void @__std_exception_destroy(ptr noundef nonnull %3)
          to label %6 unwind label %4

4:                                                ; preds = %2
  %5 = cleanuppad within none []
  call void @__std_terminate() #23 [ "funclet"(token %5) ]
  unreachable

6:                                                ; preds = %2
  %7 = icmp eq i32 %1, 0
  br i1 %7, label %9, label %8

8:                                                ; preds = %6
  tail call void @"??3@YAXPEAX@Z"(ptr noundef nonnull %0) #20
  br label %9

9:                                                ; preds = %8, %6
  ret ptr %0
}

; Function Attrs: mustprogress nocallback nofree nounwind willreturn memory(argmem: write)
declare void @llvm.memset.p0.i64(ptr nocapture writeonly, i8, i64, i1 immarg) #8

; Function Attrs: mustprogress nounwind uwtable
define linkonce_odr dso_local noundef ptr @"??_Gexception@std@@UEAAPEAXI@Z"(ptr noundef nonnull align 8 dereferenceable(24) %0, i32 noundef %1) unnamed_addr #4 comdat align 2 personality ptr @__CxxFrameHandler3 {
  store ptr @"??_7exception@std@@6B@", ptr %0, align 8, !tbaa !45
  %3 = getelementptr inbounds %"class.std::exception", ptr %0, i64 0, i32 1
  invoke void @__std_exception_destroy(ptr noundef nonnull %3)
          to label %6 unwind label %4

4:                                                ; preds = %2
  %5 = cleanuppad within none []
  call void @__std_terminate() #23 [ "funclet"(token %5) ]
  unreachable

6:                                                ; preds = %2
  %7 = icmp eq i32 %1, 0
  br i1 %7, label %9, label %8

8:                                                ; preds = %6
  tail call void @"??3@YAXPEAX@Z"(ptr noundef nonnull %0) #20
  br label %9

9:                                                ; preds = %8, %6
  ret ptr %0
}

declare dso_local void @__std_exception_destroy(ptr noundef) local_unnamed_addr #9

declare dso_local void @__std_terminate() local_unnamed_addr

; Function Attrs: nounwind
declare dso_local void @"??3@YAXPEAX_K@Z"(ptr noundef, i64 noundef) local_unnamed_addr #10

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn
declare void @llvm.va_start(ptr) #11

declare dso_local ptr @__acrt_iob_func(i32 noundef) local_unnamed_addr #9

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn
declare void @llvm.va_end(ptr) #11

declare dso_local i32 @__stdio_common_vfprintf(i64 noundef, ptr noundef, ptr noundef, ptr noundef, ptr noundef) local_unnamed_addr #9

; Function Attrs: mustprogress noinline nounwind uwtable
define linkonce_odr dso_local ptr @__local_stdio_printf_options() local_unnamed_addr #12 comdat {
  ret ptr @"?_OptionsStorage@?1??__local_stdio_printf_options@@9@4_KA"
}

; Function Attrs: inlinehint mustprogress noreturn uwtable
define linkonce_odr dso_local void @"?_Xlen_string@std@@YAXXZ"() local_unnamed_addr #13 comdat {
  tail call void @"?_Xlength_error@std@@YAXPEBD@Z"(ptr noundef nonnull @"??_C@_0BA@JFNIOLAK@string?5too?5long?$AA@") #22
  unreachable
}

; Function Attrs: noreturn
declare dso_local void @"?_Xlength_error@std@@YAXPEBD@Z"(ptr noundef) local_unnamed_addr #7

; Function Attrs: mustprogress nocallback nofree nounwind willreturn memory(argmem: readwrite)
declare void @llvm.memcpy.p0.p0.i64(ptr noalias nocapture writeonly, ptr noalias nocapture readonly, i64, i1 immarg) #14

; Function Attrs: mustprogress nofree nounwind willreturn memory(argmem: read)
declare dso_local i64 @strlen(ptr nocapture noundef) local_unnamed_addr #15

; Function Attrs: nofree nounwind
declare noundef i32 @putchar(i32 noundef) local_unnamed_addr #16

; Function Attrs: nofree nounwind
declare noundef i32 @puts(ptr nocapture noundef readonly) local_unnamed_addr #16

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare i64 @llvm.umax.i64(i64, i64) #17

attributes #0 = { mustprogress uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite) }
attributes #2 = { nobuiltin allocsize(0) "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #3 = { nobuiltin nounwind "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #4 = { mustprogress nounwind uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #5 = { inlinehint mustprogress nounwind uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #6 = { inlinehint mustprogress uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #7 = { noreturn "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #8 = { mustprogress nocallback nofree nounwind willreturn memory(argmem: write) }
attributes #9 = { "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #10 = { nounwind "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #11 = { mustprogress nocallback nofree nosync nounwind willreturn }
attributes #12 = { mustprogress noinline nounwind uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #13 = { inlinehint mustprogress noreturn uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #14 = { mustprogress nocallback nofree nounwind willreturn memory(argmem: readwrite) }
attributes #15 = { mustprogress nofree nounwind willreturn memory(argmem: read) "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #16 = { nofree nounwind }
attributes #17 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }
attributes #18 = { builtin allocsize(0) }
attributes #19 = { allocsize(0) }
attributes #20 = { builtin nounwind }
attributes #21 = { nounwind }
attributes #22 = { noreturn }
attributes #23 = { noreturn nounwind }

!llvm.linker.options = !{!0, !1, !2, !3, !4, !5, !6}
!llvm.module.flags = !{!7, !8, !9, !10}
!llvm.ident = !{!11}

!0 = !{!"/FAILIFMISMATCH:\22_CRT_STDIO_ISO_WIDE_SPECIFIERS=0\22"}
!1 = !{!"/FAILIFMISMATCH:\22_MSC_VER=1900\22"}
!2 = !{!"/FAILIFMISMATCH:\22_ITERATOR_DEBUG_LEVEL=0\22"}
!3 = !{!"/FAILIFMISMATCH:\22RuntimeLibrary=MT_StaticRelease\22"}
!4 = !{!"/DEFAULTLIB:libcpmt.lib"}
!5 = !{!"/FAILIFMISMATCH:\22annotate_string=0\22"}
!6 = !{!"/FAILIFMISMATCH:\22annotate_vector=0\22"}
!7 = !{i32 1, !"wchar_size", i32 2}
!8 = !{i32 8, !"PIC Level", i32 2}
!9 = !{i32 7, !"uwtable", i32 2}
!10 = !{i32 1, !"MaxTLSAlign", i32 65536}
!11 = !{!"clang version 18.1.4"}
!12 = !{!13, !13, i64 0}
!13 = !{!"any pointer", !14, i64 0}
!14 = !{!"omnipotent char", !15, i64 0}
!15 = !{!"Simple C++ TBAA"}
!16 = !{!17, !13, i64 0}
!17 = !{!"?AV?$list@URecord@@V?$allocator@URecord@@@std@@@std@@", !18, i64 0}
!18 = !{!"?AV?$_Compressed_pair@V?$allocator@U?$_List_node@URecord@@PEAX@std@@@std@@V?$_List_val@U?$_List_simple_types@URecord@@@std@@@2@$00@std@@", !19, i64 0}
!19 = !{!"?AV?$_List_val@U?$_List_simple_types@URecord@@@std@@@std@@", !13, i64 0, !20, i64 8}
!20 = !{!"long long", !14, i64 0}
!21 = !{!22, !20, i64 0}
!22 = !{!"?AUZeta_CheckPointRecorder@@", !20, i64 0, !17, i64 8}
!23 = !{!19, !13, i64 0}
!24 = !{!25, !13, i64 8}
!25 = !{!"?AU?$_List_node@URecord@@PEAX@std@@", !13, i64 0, !13, i64 8, !26, i64 16}
!26 = !{!"?AURecord@@", !27, i64 0, !20, i64 32, !27, i64 40}
!27 = !{!"?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@", !28, i64 0}
!28 = !{!"?AV?$_Compressed_pair@V?$allocator@D@std@@V?$_String_val@U?$_Simple_types@D@std@@@2@$00@std@@", !29, i64 0}
!29 = !{!"?AV?$_String_val@U?$_Simple_types@D@std@@@std@@", !14, i64 0, !20, i64 16, !20, i64 24}
!30 = !{!25, !13, i64 0}
!31 = distinct !{!31, !32}
!32 = !{!"llvm.loop.mustprogress"}
!33 = !{!17, !20, i64 8}
!34 = distinct !{!34, !32}
!35 = !{!29, !20, i64 16}
!36 = !{!29, !20, i64 24}
!37 = !{!20, !20, i64 0}
!38 = !{!14, !14, i64 0}
!39 = !{!26, !20, i64 32}
!40 = !{!41, !41, i64 0}
!41 = !{!"bool", !14, i64 0}
!42 = !{i8 0, i8 2}
!43 = !{}
!44 = distinct !{!44, !32}
!45 = !{!46, !46, i64 0}
!46 = !{!"vtable pointer", !15, i64 0}
!47 = !{!48, !13, i64 8}
!48 = !{!"?AVexception@std@@", !49, i64 8}
!49 = !{!"?AU__std_exception_data@@", !13, i64 0, !41, i64 8}
