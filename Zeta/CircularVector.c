#include "CircularVector.h"

#include "utils.h"

void Zeta_CircularVector_Init(void* cv_) {
    Zeta_CircularVector* cv = cv_;
    ZETA_DebugAssert(cv != NULL);

    cv->vec = NULL;
    cv->offset = 0;
    cv->size = 0;
}

void Zeta_CircularVector_Check(void* cv_) {
    Zeta_CircularVector* cv = cv_;
    ZETA_DebugAssert(cv != NULL);

    Zeta_Vector* vec = cv->vec;

    if (cv->vec == NULL) {
        ZETA_DebugAssert(cv->offset == 0);
        ZETA_DebugAssert(cv->size == 0);
        return;
    }

    ZETA_DebugAssert(vec->GetSize != NULL);

    size_t offset = cv->offset;
    size_t size = cv->size;
    size_t capacity = vec->GetSize(vec->context);

    ZETA_DebugAssert(offset < capacity);
    ZETA_DebugAssert(size <= capacity);
    ZETA_DebugAssert(capacity <= ZETA_max_mod_under_size_t);
}

void* Zeta_CircularVector_GetVector(void* cv_) {
    Zeta_CircularVector* cv = cv_;
    ZETA_DebugAssert(cv != NULL);
    return cv->vec;
}

size_t Zeta_CircularVector_GetWidth(void* cv_) {
    Zeta_CircularVector* cv = cv_;
    ZETA_DebugAssert(cv != NULL);

    Zeta_Vector* vec = cv->vec;
    ZETA_DebugAssert(vec->GetWidth != NULL);

    return vec->GetWidth(vec->context);
}

size_t Zeta_CircularVector_GetOffset(void* cv_) {
    Zeta_CircularVector* cv = cv_;
    ZETA_DebugAssert(cv != NULL);
    return cv->offset;
}

size_t Zeta_CircularVector_GetSize(void* cv_) {
    Zeta_CircularVector* cv = cv_;
    ZETA_DebugAssert(cv != NULL);
    return cv->size;
}

size_t Zeta_CircularVector_GetCapacity(void* cv_) {
    Zeta_CircularVector* cv = cv_;
    ZETA_DebugAssert(cv != NULL);

    if (cv->vec == NULL) { return 0; }

    Zeta_Vector* vec = cv->vec;
    ZETA_DebugAssert(vec->GetSize != NULL);

    return vec->GetSize(vec->context);
}

void Zeta_CircularVector_Entrust(void* cv_, void* vec, size_t size,
                                 size_t offset) {
    Zeta_CircularVector* cv = cv_;
    ZETA_DebugAssert(cv != NULL);

    if (vec == NULL) {
        ZETA_DebugAssert(offset == 0);
        ZETA_DebugAssert(size == 0);

        cv->vec = NULL;
        cv->offset = 0;
        cv->size = 0;
    } else {
        Zeta_Vector* vec = cv->vec;
        ZETA_DebugAssert(vec->GetSize != NULL);

        size_t capacity = vec->GetSize(vec->context);

        ZETA_DebugAssert(offset < capacity);
        ZETA_DebugAssert(size <= capacity);
        ZETA_DebugAssert(capacity <= ZETA_max_mod_under_size_t);

        cv->vec = vec;
        cv->offset = offset;
        cv->size = size;
    }
}

void* Zeta_CircularVector_Access(void* cv_, size_t idx) {
    Zeta_CircularVector* cv = cv_;
    ZETA_DebugAssert(cv != NULL);

    Zeta_Vector* vec = cv->vec;
    ZETA_DebugAssert(vec != NULL);
    ZETA_DebugAssert(vec->GetSize != NULL);

    size_t offset = cv->offset;
    size_t size = cv->size;
    size_t capacity = vec->GetSize(vec->context);

    ZETA_DebugAssert(offset < capacity);
    ZETA_DebugAssert(size <= capacity);
    ZETA_DebugAssert(capacity <= ZETA_max_mod_under_size_t);

    ZETA_DebugAssert(idx < size);
    ZETA_DebugAssert(vec->Access != NULL);

    return vec->Access(vec->context, (offset + idx) % capacity);
}

void* Zeta_CircularVector_PeekL(void* cv_) {
    Zeta_CircularVector* cv = cv_;
    ZETA_DebugAssert(cv != NULL);

    ZETA_DebugAssert(0 < cv->size);

    return Zeta_CircularVector_Access(cv, 0);
}

void* Zeta_CircularVector_PeekR(void* cv_) {
    Zeta_CircularVector* cv = cv_;
    ZETA_DebugAssert(cv != NULL);

    ZETA_DebugAssert(0 < cv->size);

    return Zeta_CircularVector_Access(cv, cv->size - 1);
}

void* Zeta_CircularVector_Insert(void* cv_, size_t idx) {
    Zeta_CircularVector* cv = cv_;
    ZETA_DebugAssert(cv != NULL);

    Zeta_Vector* vec = cv->vec;
    ZETA_DebugAssert(vec != NULL);
    ZETA_DebugAssert(vec->GetWidth != NULL);
    ZETA_DebugAssert(vec->GetSize != NULL);

    size_t width = vec->GetWidth(vec->context);
    size_t offset = cv->offset;
    size_t size = cv->size;
    size_t capacity = vec->GetSize(vec->context);

    ZETA_DebugAssert(0 < width);
    ZETA_DebugAssert(offset < capacity);
    ZETA_DebugAssert(size < capacity);
    ZETA_DebugAssert(capacity <= ZETA_max_mod_under_size_t);

    ZETA_DebugAssert(idx <= size);

    size_t hole_idx;

    if (idx < size - idx) {
        cv->offset = (offset + capacity - 1) % capacity;
        ++cv->size;
        hole_idx = 0;
    } else {
        ++cv->size;
        hole_idx = size;
    }

    for (; hole_idx < idx; ++hole_idx) {
        Zeta_MemCopy(width, Zeta_CircularVector_Access(cv, hole_idx),
                     Zeta_CircularVector_Access(cv, hole_idx + 1));
    }

    for (; idx < hole_idx; --hole_idx) {
        Zeta_MemCopy(width, Zeta_CircularVector_Access(cv, hole_idx),
                     Zeta_CircularVector_Access(cv, hole_idx - 1));
    }

    return Zeta_CircularVector_Access(cv, idx);
}

void* Zeta_CircularVector_PushL(void* cv_) {
    return Zeta_CircularVector_Insert(cv_, 0);
}

void* Zeta_CircularVector_PushR(void* cv_) {
    Zeta_CircularVector* cv = cv_;
    ZETA_DebugAssert(cv != NULL);

    return Zeta_CircularVector_Insert(cv, cv->size);
}

void Zeta_CircularVector_Erase(void* cv_, size_t idx) {
    Zeta_CircularVector* cv = cv_;
    ZETA_DebugAssert(cv != NULL);

    Zeta_Vector* vec = cv->vec;
    ZETA_DebugAssert(vec != NULL);
    ZETA_DebugAssert(vec->GetWidth != NULL);
    ZETA_DebugAssert(vec->GetSize != NULL);

    size_t width = vec->GetWidth(vec->context);
    size_t offset = cv->offset;
    size_t size = cv->size;
    size_t capacity = vec->GetSize(vec->context);

    ZETA_DebugAssert(0 < width);
    ZETA_DebugAssert(offset < capacity);
    ZETA_DebugAssert(size <= capacity);
    ZETA_DebugAssert(capacity <= ZETA_max_mod_under_size_t);

    ZETA_DebugAssert(idx < size);

    if (idx < size - 1 - idx) {
        for (size_t i = idx; 0 < i; --i) {
            Zeta_MemCopy(width, Zeta_CircularVector_Access(cv, i),
                         Zeta_CircularVector_Access(cv, i - 1));
        }

        cv->offset = (offset + 1) % capacity;
        --cv->size;
    } else {
        for (size_t i = idx; i < size - 1; ++i) {
            Zeta_MemCopy(width, Zeta_CircularVector_Access(cv, i),
                         Zeta_CircularVector_Access(cv, i + 1));
        }

        --cv->size;
    }
}

void Zeta_CircularVector_PopL(void* cv_) {
    Zeta_CircularVector* cv = cv_;
    ZETA_DebugAssert(cv != NULL);

    ZETA_DebugAssert(0 < cv->size);

    Zeta_CircularVector_Erase(cv, 0);
}

void Zeta_CircularVector_PopR(void* cv_) {
    Zeta_CircularVector* cv = cv_;
    ZETA_DebugAssert(cv != NULL);

    ZETA_DebugAssert(0 < cv->size);

    Zeta_CircularVector_Erase(cv, cv->size - 1);
}

void Zeta_CircularVector_EraseAll(void* cv_, void (*Callback)(void* ptr)) {
    Zeta_CircularVector* cv = cv_;
    ZETA_DebugAssert(cv != NULL);

    if (Callback != NULL) {
        for (size_t i = 0; i < cv->size; ++i) {
            Callback(Zeta_CircularVector_Access(cv, i));
        }
    }

    cv->offset = 0;
    cv->size = 0;
}

void Zeta_CircularVector_ToVector(void* cv_, Zeta_Vector* dst) {
    Zeta_CircularVector* cv = cv_;
    ZETA_DebugAssert(cv != NULL);

    ZETA_DebugAssert(dst != NULL);

    dst->context = cv;
    dst->GetWidth = Zeta_CircularVector_GetWidth;
    dst->GetSize = Zeta_CircularVector_GetSize;
    dst->Access = Zeta_CircularVector_Access;
}
