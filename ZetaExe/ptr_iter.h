#pragma once

#include <debugger.h>
#include <stddef.h>

template <typename Value>
struct PtrIter {
    using difference_type = ptrdiff_t;
    using value_type = Value;
    using pointer_type = Value*;
    using reference = Value&;
    using iterator_category = std::random_access_iterator_tag;

    void const* ptr;
    ptrdiff_t stride;

    Value* operator->() const;

    Value& operator[](ptrdiff_t idx) const;
};

template <typename ValueA, typename ValueB>
bool operator==(const PtrIter<ValueA>& ptr_iter_a,
                const PtrIter<ValueB>& ptr_iter_b);

template <typename ValueA, typename ValueB>
bool operator!=(const PtrIter<ValueA>& ptr_iter_a,
                const PtrIter<ValueB>& ptr_iter_b);

template <typename ValueA, typename ValueB>
bool operator<(const PtrIter<ValueA>& ptr_iter_a,
               const PtrIter<ValueB>& ptr_iter_b);

template <typename ValueA, typename ValueB>
bool operator>(const PtrIter<ValueA>& ptr_iter_a,
               const PtrIter<ValueB>& ptr_iter_b);

template <typename ValueA, typename ValueB>
bool operator<=(const PtrIter<ValueA>& ptr_iter_a,
                const PtrIter<ValueB>& ptr_iter_b);

template <typename ValueA, typename ValueB>
bool operator>=(const PtrIter<ValueA>& ptr_iter_a,
                const PtrIter<ValueB>& ptr_iter_b);

template <typename Value>
PtrIter<Value>& operator++(PtrIter<Value>& ptr_iter);

template <typename Value>
PtrIter<Value>& operator--(PtrIter<Value>& ptr_iter);

template <typename Value>
PtrIter<Value> operator++(PtrIter<Value>& ptr_iter, int);

template <typename Value>
PtrIter<Value> operator--(PtrIter<Value>& ptr_iter, int);

template <typename Value>
PtrIter<Value> operator+(const PtrIter<Value>& ptr_iter, ptrdiff_t step);

template <typename Value>
PtrIter<Value> operator-(const PtrIter<Value>& ptr_iter, ptrdiff_t step);

template <typename Value>
PtrIter<Value>& operator+=(const PtrIter<Value>& ptr_iter, ptrdiff_t step);

template <typename Value>
PtrIter<Value>& operator-=(const PtrIter<Value>& ptr_iter, ptrdiff_t step);

template <typename ValueA, typename ValueB>
ptrdiff_t operator-(const PtrIter<ValueA>& ptr_iter_a,
                    const PtrIter<ValueB>& ptr_iter_b);

template <typename Value>
Value& operator*(const PtrIter<Value>& ptr_iter);

// -----------------------------------------------------------------------------

template <typename Value>
Value* PtrIter<Value>::operator->() const {
    return (Value*)(this->ptr);
}

template <typename Value>
Value& PtrIter<Value>::operator[](ptrdiff_t idx) const {
    return *(Value*)(this->ptr + this->stride * idx);
}

template <typename ValueA, typename ValueB>
bool operator==(const PtrIter<ValueA>& ptr_iter_a,
                const PtrIter<ValueB>& ptr_iter_b) {
    return ptr_iter_a.ptr == ptr_iter_b.ptr;
}

template <typename ValueA, typename ValueB>
bool operator!=(const PtrIter<ValueA>& ptr_iter_a,
                const PtrIter<ValueB>& ptr_iter_b) {
    return ptr_iter_a.ptr != ptr_iter_b.ptr;
}

template <typename ValueA, typename ValueB>
bool operator<(const PtrIter<ValueA>& ptr_iter_a,
               const PtrIter<ValueB>& ptr_iter_b) {
    return ptr_iter_a.ptr <= ptr_iter_b.ptr;
}

template <typename ValueA, typename ValueB>
bool operator>(const PtrIter<ValueA>& ptr_iter_a,
               const PtrIter<ValueB>& ptr_iter_b) {
    return ptr_iter_a.ptr >= ptr_iter_b.ptr;
}

template <typename ValueA, typename ValueB>
bool operator<=(const PtrIter<ValueA>& ptr_iter_a,
                const PtrIter<ValueB>& ptr_iter_b) {
    return ptr_iter_a.ptr <= ptr_iter_b.ptr;
}

template <typename ValueA, typename ValueB>
bool operator>=(const PtrIter<ValueA>& ptr_iter_a,
                const PtrIter<ValueB>& ptr_iter_b) {
    return ptr_iter_a.ptr >= ptr_iter_b.ptr;
}

template <typename Value>
PtrIter<Value>& operator++(PtrIter<Value>& ptr_iter) {
    ptr_iter.ptr =
        static_cast<unsigned char const*>(ptr_iter.ptr) + ptr_iter.stride;
    return ptr_iter;
}

template <typename Value>
PtrIter<Value>& operator--(PtrIter<Value>& ptr_iter) {
    ptr_iter.ptr =
        static_cast<unsigned char const*>(ptr_iter.ptr) - ptr_iter.stride;

    return ptr_iter;
}

template <typename Value>
PtrIter<Value> operator++(PtrIter<Value>& ptr_iter, int) {
    PtrIter ret{ ptr_iter };
    ++ptr_iter;
    return ret;
}

template <typename Value>
PtrIter<Value> operator--(PtrIter<Value>& ptr_iter, int) {
    PtrIter ret{ ptr_iter };
    --ptr_iter;
    return ret;
}

template <typename Value>
PtrIter<Value> operator+(const PtrIter<Value>& ptr_iter, ptrdiff_t step) {
    return { static_cast<unsigned char const*>(ptr_iter.ptr) +
                 ptr_iter.stride * step,
             ptr_iter.stride };
}

template <typename Value>
PtrIter<Value> operator-(const PtrIter<Value>& ptr_iter, ptrdiff_t step) {
    return { static_cast<unsigned char const*>(ptr_iter.ptr) -
                 ptr_iter.stride * step,
             ptr_iter.stride };
}

template <typename Value>
PtrIter<Value>& operator+=(const PtrIter<Value>& ptr_iter, ptrdiff_t step) {
    ptr_iter.ptr = static_cast<unsigned char const*>(ptr_iter.ptr) +
                   ptr_iter.stride * step;

    return ptr_iter;
}

template <typename Value>
PtrIter<Value>& operator-=(const PtrIter<Value>& ptr_iter, ptrdiff_t step) {
    ptr_iter.ptr = static_cast<unsigned char const*>(ptr_iter.ptr) -
                   ptr_iter.stride * step;

    return ptr_iter;
}

template <typename ValueA, typename ValueB>
ptrdiff_t operator-(const PtrIter<ValueA>& ptr_iter_a,
                    const PtrIter<ValueB>& ptr_iter_b) {
    ZETA_DebugAssert(ptr_iter_a.stride == ptr_iter_b.stride);

    ptrdiff_t diff{ static_cast<unsigned char const*>(ptr_iter_b.ptr) -
                    static_cast<unsigned char const*>(ptr_iter_a.ptr) };

    ZETA_DebugAssert(diff % ptr_iter_a.stride == 0);

    return diff / ptr_iter_a.stride;
}

template <typename Value>
Value& operator*(const PtrIter<Value>& ptr_iter) {
    return *(Value*)(ptr_iter.ptr);
}
