#pragma once

#include <AssocContainer.h>
#include <Debugger.h>

#include <unordered_map>

auto& AssocContainer_GetSanitizeFuncs() {
    static std::unordered_map<size_t (*)(void* constext),
                              void (*)(Zeta_AssocContainer const* assoc_cntr)>
        instance;
    return instance;
}

void AssocContainer_AddSanitizeFunc(
    size_t (*GetWidth)(void* context),
    void (*Sanitize)(Zeta_AssocContainer const* assoc_cntr)) {
    auto& map{ AssocContainer_GetSanitizeFuncs() };

    auto iter{ map.insert({ GetWidth, Sanitize }).first };
    ZETA_DebugAssert(iter->second == Sanitize);
}

void AssocContainer_Sanitize(Zeta_AssocContainer const* assoc_cntr) {
    if (assoc_cntr == NULL) { return; }

    auto& map{ AssocContainer_GetSanitizeFuncs() };

    auto iter{ map.find(assoc_cntr->GetWidth) };
    ZETA_DebugAssert(iter != map.end());

    iter->second(assoc_cntr);
}
