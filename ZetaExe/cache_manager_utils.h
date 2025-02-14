#pragma once

#include <cache_manager.h>
#include <debugger.h>

#include <unordered_map>

struct CacheManagerUtils_Funcs {
    void (*Destroy)(Zeta_CacheManager cm) = NULL;

    void (*Sanitize)(Zeta_CacheManager cm) = NULL;
};

auto& CacheManagerUtils_GetFuncs() {
    static std::unordered_map<Zeta_CacheManager_VTable const*,
                              CacheManagerUtils_Funcs>
        instance;
    return instance;
}

// ---

void CacheManagerUtils_AddDestroyFunc(
    Zeta_CacheManager_VTable const* cache_manager_vtable,
    void (*Destroy)(Zeta_CacheManager cm)) {
    auto& map{ CacheManagerUtils_GetFuncs() };

    CacheManagerUtils_Funcs& funcs{
        map.insert({ cache_manager_vtable, {} }).first->second
    };

    ZETA_DebugAssert(funcs.Destroy == NULL || funcs.Destroy == Destroy);

    funcs.Destroy = Destroy;
}

void CacheManagerUtils_Destroy(Zeta_CacheManager cache_manager) {
    auto& map{ CacheManagerUtils_GetFuncs() };

    auto iter{ map.find(cache_manager.vtable) };
    ZETA_DebugAssert(iter != map.end());

    auto Destroy{ iter->second.Destroy };

    ZETA_DebugAssert(Destroy != NULL);

    Destroy(cache_manager);
}

// ---

void CacheManagerUtils_AddSanitizeFunc(
    Zeta_CacheManager_VTable const* cache_manager_vtable,
    void (*Sanitize)(Zeta_CacheManager cm)) {
    auto& map{ CacheManagerUtils_GetFuncs() };

    CacheManagerUtils_Funcs& funcs{
        map.insert({ cache_manager_vtable, {} }).first->second
    };

    ZETA_DebugAssert(funcs.Sanitize == NULL || funcs.Sanitize == Sanitize);

    funcs.Sanitize = Sanitize;
}

void CacheManagerUtils_Sanitize(Zeta_CacheManager cm) {
    auto& map{ CacheManagerUtils_GetFuncs() };

    auto iter{ map.find(cm.vtable) };
    ZETA_DebugAssert(iter != map.end());

    auto Sanitize{ iter->second.Sanitize };

    ZETA_DebugAssert(Sanitize != NULL);

    Sanitize(cm);
}
