#pragma once

struct Zeta_IteratorOperator {
    void* context;

    size_t iter_align;

    size_t iter_width;

    size_t ele_align;

    size_t ele_width;

    int (*Compare)(void* context, void* iter_a, void* iter_b);

    size_t (*GetDiff)(void* context, void* iter_a, void* iter_b);

    void* (*Ref)(void* context, void* iter);

    void (*StepL)(void* context, void* iter);

    void (*StepR)(void* context, void* iter);

    void (*AdvanceL)(void* context, void* iter, size_t step);

    void (*AdvanceR)(void* context, void* iter, size_t step);
};
