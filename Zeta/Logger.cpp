

#include "Logger.h"

#include <iostream>
#include <list>

void* debug_logger = NULL;

struct Log {
    std::string filename;
    size_t line;
    std::string func;
    std::string msg;
};

struct Zeta_Logger {
    size_t capacity;
    std::list<Log> logs;
};

void* Zeta_Logger_Create() {
    Zeta_Logger* logger = new Zeta_Logger;
    logger->capacity = 1024;

    return logger;
}

void Zeta_Logger_Destroy(void* logger_) {
    Zeta_Logger* logger = (Zeta_Logger*)logger_;

    delete logger;
}

void Zeta_Logger_Log(void* logger_, char const* filename, size_t line,
                     char const* func, char const* msg) {
    Zeta_Logger* logger = (Zeta_Logger*)logger_;

    while (logger->capacity <= logger->logs.size()) {
        logger->logs.pop_front();
    }

    logger->logs.push_back({ filename, line, func, msg == NULL ? "" : msg });

    // ZETA_PrintPosFormat(filename, line, func);
    // printf("\n");
}

void Zeta_Logger_Clear(void* logger_) {
    Zeta_Logger* logger = (Zeta_Logger*)logger_;

    logger->logs.clear();
}

void Zeta_Logger_Print(void* logger_) {
    Zeta_Logger* logger = (Zeta_Logger*)logger_;
    ZETA_DebugAssert(logger != NULL);

    printf("check point recorder print:\n");

    for (auto iter{ logger->logs.begin() }, end{ logger->logs.end() };
         iter != end; ++iter) {
        ZETA_PrintPosFormat(iter->filename.c_str(), iter->line,
                            iter->func.c_str());
        printf("    %s\n", iter->msg.c_str());
    }
}

static int DebugLoggerAssertCallback(void* context) {
    ZETA_Unused(context);
    Zeta_Logger_Print(debug_logger);
    return 0;
}

void Zeta_InitDebugLogger() {
    if (debug_logger != NULL) { return; }

    ZETA_DebugAssert(!zeta_debugger_assert_stage);
    ZETA_DebugAssert(zeta_debugger_assert_callback == NULL);

    debug_logger = Zeta_Logger_Create();

    zeta_debugger_assert_callback_context = debug_logger;
    zeta_debugger_assert_callback = DebugLoggerAssertCallback;
}
