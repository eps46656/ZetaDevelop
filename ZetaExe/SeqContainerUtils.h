#pragma once

#include <Debugger.h>
#include <SeqContainer.h>

#include <unordered_map>

auto& GetSanitizeFuncs() {
    static std::unordered_map<size_t (*)(void* constext),
                              void (*)(Zeta_SeqContainer const* seq_cntr)>
        instance;
    return instance;
}

void SeqContainer_AddSanitizeFunc(
    size_t (*GetWidth)(void* context),
    void (*Sanitize)(Zeta_SeqContainer const* seq_cntr)) {
    auto& map{ GetSanitizeFuncs() };

    auto iter{ map.insert({ GetWidth, Sanitize }).first };
    ZETA_DebugAssert(iter->second == Sanitize);
}

void SeqContainer_Sanitize(Zeta_SeqContainer const* seq_cntr) {
    if (seq_cntr == NULL) { return; }

    auto& map{ GetSanitizeFuncs() };

    auto iter{ map.find(seq_cntr->GetWidth) };
    ZETA_DebugAssert(iter != map.end());

    iter->second(seq_cntr);
}
