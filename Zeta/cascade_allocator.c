#include "cascade_allocator.h"

#include "debugger.h"
#include "ord_llist_node.h"
#include "utils.h"

#if ZETA_EnableDebug

#define Check_(ca) Zeta_CascadeAllocator_Check(ca)

#else

#define Check_(ca) ZETA_Unused((ca))

#endif

void Zeta_CascadeAllocator_Init(void* ca_) {
    Zeta_CascadeAllocator* ca = ca_;
    Check_(ca);

    size_t align = ca->align;
    ZETA_DebugAssert(0 < align);

    align = Zeta_GCD(alignof(Zeta_OrdLListNode), align);

    void* mem_beg = __builtin_align_up(ca->mem, align);
    void* mem_end = __builtin_align_down(ca->mem + ca->size, align);

    ZETA_DebugAssert(mem_beg <= mem_end);

    size_t size = mem_end - mem_beg;

    ZETA_DebugAssert(sizeof(void*) * 1024 <= size);

    size_t node_size = ZETA_IntRoundUp(sizeof(Zeta_OrdLListNode), align);

    void* first_node = mem_end - node_size;

    Zeta_OrdLListNode_Init(first_node);

    ca->align = align;

    ca->mem = mem_beg;
    ca->size = size;

    ca->first_node = first_node;
}

size_t Zeta_CascadeAllocator_GetAlign(void const* ca_) {
    Zeta_CascadeAllocator const* ca = ca_;
    Check_(ca);

    return ca->align;
}

size_t Zeta_CascadeAllocator_Query(void const* ca_, size_t size) {
    Zeta_CascadeAllocator const* ca = ca_;
    Check_(ca);

    size_t align = ca->align;

    return ZETA_IntRoundUp(size, align);
}

void* Zeta_CascadeAllocator_Allocate(void* ca_, size_t size) {
    Zeta_CascadeAllocator* ca = ca_;
    Check_(ca);

    if (size == 0) { return NULL; }

    size_t align = ca->align;

    void* mem_beg = ca->mem;

    size_t node_size = ZETA_IntRoundUp(sizeof(Zeta_OrdLListNode), align);

    size = ZETA_IntRoundUp(size, align);

    void* first_node = ca->first_node;

    if ((size_t)((void*)first_node - mem_beg) < node_size + size) {
        return NULL;
    }

    void* new_first_node = first_node - (node_size + size);

    Zeta_OrdLListNode_Init(new_first_node);

    Zeta_LList_OrdLListNode_InsertL(first_node, new_first_node);

    ca->first_node = new_first_node;

    return (void*)new_first_node + node_size;
}

void Zeta_CascadeAllocator_Deallocate(void* ca_, void* ptr) {
    Zeta_CascadeAllocator* ca = ca_;
    Check_(ca);

    if (ptr == NULL) { return; }

    size_t align = ca->align;

    size_t node_size = ZETA_IntRoundUp(sizeof(Zeta_OrdLListNode), align);

    void* node = ptr - node_size;

    if (ca->first_node == node) {
        ca->first_node = Zeta_OrdLListNode_GetR(ca->first_node);
    }

    Zeta_LList_OrdLListNode_Extract(ptr - node_size);
}

void Zeta_CascadeAllocator_Check(void const* ca_) {
    Zeta_CascadeAllocator const* ca = ca_;
    ZETA_DebugAssert(ca != NULL);

    size_t align = ca->align;

    ZETA_DebugAssert(0 < align);
    ZETA_DebugAssert(align % alignof(Zeta_OrdLListNode) == 0);

    void* mem_beg = ca->mem;
    void* mem_end = mem_beg + ca->size;

    ZETA_DebugAssert(__builtin_is_aligned(mem_beg, align));
    ZETA_DebugAssert(__builtin_is_aligned(mem_end, align));
}

void Zeta_CascadeAllocator_Sanitize(void const* ca_, Zeta_MemRecorder* dst) {
    Zeta_CascadeAllocator const* ca = ca_;
    Check_(ca);

    size_t align = ca->align;

    size_t node_size = ZETA_IntRoundUp(sizeof(Zeta_OrdLListNode), align);

    void* first_node = ca->first_node;

    for (void* node = first_node;;) {
        void* nxt_node = Zeta_OrdLListNode_GetR(node);
        ZETA_DebugAssert(Zeta_OrdLListNode_GetL(nxt_node) == node);

        if (nxt_node == first_node) { break; }

        ZETA_DebugAssert(node < nxt_node);
        ZETA_DebugAssert(node_size <= (size_t)(nxt_node - node));

        if (dst != NULL) {
            Zeta_MemRecorder_Record(dst, node + node_size,
                                    nxt_node - node - node_size);
        }

        node = nxt_node;
    }

    ZETA_DebugAssert(ca->mem + ca->size - node_size ==
                     Zeta_OrdLListNode_GetL(first_node));
}

void Zeta_CascadeAllocator_DeployAllocator(void* ca_, Zeta_Allocator* dst) {
    Zeta_CascadeAllocator* ca = ca_;
    Check_(ca);

    ZETA_DebugAssert(dst != NULL);

    Zeta_Allocator_Init(dst);

    dst->context = ca;

    dst->GetAlign = Zeta_CascadeAllocator_GetAlign;

    dst->Query = Zeta_CascadeAllocator_Query;

    dst->Allocate = Zeta_CascadeAllocator_Allocate;

    dst->Deallocate = Zeta_CascadeAllocator_Deallocate;
}
