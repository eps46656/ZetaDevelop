#pragma once

#include <Debugger.h>
#include <assoc_cntr.h>

#include <unordered_map>

auto& AssocCntr_GetSanitizeFuncs() {
    static std::unordered_map<size_t (*)(void* constext),
                              void (*)(Zeta_AssocCntr const* assoc_cntr)>
        instance;
    return instance;
}

void AssocCntr_AddSanitizeFunc(
    size_t (*GetWidth)(void* context),
    void (*Sanitize)(Zeta_AssocCntr const* assoc_cntr)) {
    auto& map{ AssocCntr_GetSanitizeFuncs() };

    auto iter{ map.insert({ GetWidth, Sanitize }).first };
    ZETA_DebugAssert(iter->second == Sanitize);
}

void AssocCntr_Sanitize(Zeta_AssocCntr const* assoc_cntr) {
    if (assoc_cntr == NULL) { return; }

    auto& map{ AssocCntr_GetSanitizeFuncs() };

    auto iter{ map.find(assoc_cntr->GetWidth) };
    ZETA_DebugAssert(iter != map.end());

    iter->second(assoc_cntr);
}
