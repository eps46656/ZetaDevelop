

#include "CheckPointMeter.h"

#include <iostream>
#include <list>

void* std_check_point_recorder = NULL;

struct Record {
    std::string filename;
    size_t line;
    std::string func;
};

struct Zeta_CheckPointMeter {
    size_t capacity;
    std::list<Record> records;
};

void* Zeta_CheckPointMeter_Create() {
    Zeta_CheckPointMeter* cpr = new Zeta_CheckPointMeter;
    cpr->capacity = 1024;

    return cpr;
}

void Zeta_CheckPointMeter_Destroy(void* cpr_) {
    Zeta_CheckPointMeter* cpr = (Zeta_CheckPointMeter*)cpr_;

    delete cpr;
}

void Zeta_CheckPointMeter_Record(void* cpr_, char const* filename, size_t line,
                                 char const* func) {
    Zeta_CheckPointMeter* cpr = (Zeta_CheckPointMeter*)cpr_;

    while (cpr->capacity <= cpr->records.size()) { cpr->records.pop_front(); }

    cpr->records.push_back({ filename, line, func });
}

void Zeta_CheckPointMeter_Clear(void* cpr_) {
    Zeta_CheckPointMeter* cpr = (Zeta_CheckPointMeter*)cpr_;

    cpr->records.clear();
}

void Zeta_CheckPointMeter_Print(void* cpr_) {
    Zeta_CheckPointMeter* cpr = (Zeta_CheckPointMeter*)cpr_;
    ZETA_DebugAssert(cpr != NULL);

    printf("check point recorder print:\n");

    for (auto iter{ cpr->records.begin() }, end{ cpr->records.end() };
         iter != end; ++iter) {
        ZETA_PrintPosFormat(iter->filename.c_str(), iter->line,
                            iter->func.c_str());
        printf("\n");
    }
}

static int StdCheckPointMeterAssertCallback(void* context) {
    ZETA_Unused(context);
    Zeta_CheckPointMeter_Print(std_check_point_recorder);
    return 0;
}

void Zeta_InitStdCheckPointMeter() {
    if (std_check_point_recorder != NULL) { return; }

    ZETA_DebugAssert(!zeta_debugger_assert_stage);
    ZETA_DebugAssert(zeta_debugger_assert_callback == NULL);

    std_check_point_recorder = Zeta_CheckPointMeter_Create();

    zeta_debugger_assert_callback_context = std_check_point_recorder;
    zeta_debugger_assert_callback = StdCheckPointMeterAssertCallback;
}
