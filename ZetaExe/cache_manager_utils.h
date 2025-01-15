#pragma once

#include <cache_manager.h>
#include <debugger.h>

#include <unordered_map>

struct CacheManagerUtils_Funcs {
    void (*Destroy)(Zeta_CacheManager* cm) = NULL;

    void (*Sanitize)(Zeta_CacheManager const* cm) = NULL;
};

auto& CacheManagerUtils_GetFuncs() {
    static std::unordered_map<size_t (*)(void const* constext),
                              CacheManagerUtils_Funcs>
        instance;
    return instance;
}

// ---

void CacheManagerUtils_AddDestroyFunc(
    size_t (*GetCacheSize)(void const* context),
    void (*Destroy)(Zeta_CacheManager* cm)) {
    auto& map{ CacheManagerUtils_GetFuncs() };

    CacheManagerUtils_Funcs& funcs{
        map.insert({ GetCacheSize, {} }).first->second
    };

    ZETA_DebugAssert(funcs.Destroy == NULL || funcs.Destroy == Destroy);

    funcs.Destroy = Destroy;
}

void CacheManagerUtils_Destroy(Zeta_CacheManager* cm) {
    if (cm == NULL) { return; }

    auto& map{ CacheManagerUtils_GetFuncs() };

    auto iter{ map.find(cm->GetCacheSize) };
    ZETA_DebugAssert(iter != map.end());

    auto Destroy{ iter->second.Destroy };

    ZETA_DebugAssert(Destroy != NULL);

    Destroy(cm);
}

// ---

void CacheManagerUtils_AddSanitizeFunc(
    size_t (*GetCacheSize)(void const* context),
    void (*Sanitize)(Zeta_CacheManager const* cm)) {
    auto& map{ CacheManagerUtils_GetFuncs() };

    CacheManagerUtils_Funcs& funcs{
        map.insert({ GetCacheSize, {} }).first->second
    };

    ZETA_DebugAssert(funcs.Sanitize == NULL || funcs.Sanitize == Sanitize);

    funcs.Sanitize = Sanitize;
}

void CacheManagerUtils_Sanitize(Zeta_CacheManager const* cm) {
    auto& map{ CacheManagerUtils_GetFuncs() };

    auto iter{ map.find(cm->GetCacheSize) };
    ZETA_DebugAssert(iter != map.end());

    auto Sanitize{ iter->second.Sanitize };

    ZETA_DebugAssert(Sanitize != NULL);

    Sanitize(cm);
}
