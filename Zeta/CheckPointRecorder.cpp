

#include "CheckPointRecorder.h"

#include <iostream>
#include <list>

void* std_check_point_recorder = NULL;

struct Record {
    std::string filename;
    size_t line;
    std::string func;
};

struct Zeta_CheckPointRecorder {
    size_t capacity;
    std::list<Record> records;
};

void* Zeta_CheckPointRecorder_Create() {
    Zeta_CheckPointRecorder* cpr = new Zeta_CheckPointRecorder;
    cpr->capacity = 1024;

    return cpr;
}

void Zeta_CheckPointRecorder_Destroy(void* cpr_) {
    Zeta_CheckPointRecorder* cpr = (Zeta_CheckPointRecorder*)cpr_;

    delete cpr;
}

void Zeta_CheckPointRecorder_Record(void* cpr_, char const* filename,
                                    size_t line, char const* func) {
    Zeta_CheckPointRecorder* cpr = (Zeta_CheckPointRecorder*)cpr_;

    while (cpr->capacity <= cpr->records.size()) { cpr->records.pop_front(); }

    cpr->records.push_back({ filename, line, func });
}

void Zeta_CheckPointRecorder_Print(void* cpr_) {
    Zeta_CheckPointRecorder* cpr = (Zeta_CheckPointRecorder*)cpr_;
    ZETA_DebugAssert(cpr != NULL);

    printf("check point recorder print:\n");

    for (auto iter{ cpr->records.begin() }, end{ cpr->records.end() };
         iter != end; ++iter) {
        ZETA_PrintPosFormat(iter->filename.c_str(), iter->line,
                            iter->func.c_str());
        printf("\n");
    }
}

static int StdCheckPointRecorderAssertCallback(void* context) {
    ZETA_Unused(context);
    Zeta_CheckPointRecorder_Print(std_check_point_recorder);
    return 0;
}

void Zeta_InitStdCheckPointRecorder() {
    if (std_check_point_recorder != NULL) { return; }

    ZETA_DebugAssert(!zeta_debugger_assert_stage);
    ZETA_DebugAssert(zeta_debugger_assert_callback == NULL);

    std_check_point_recorder = Zeta_CheckPointRecorder_Create();

    zeta_debugger_assert_callback_context = std_check_point_recorder;
    zeta_debugger_assert_callback = StdCheckPointRecorderAssertCallback;
}
