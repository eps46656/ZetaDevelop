#include <memory>
#include <unordered_map>

#include "../Zeta/define.h"

template <typename T>
class CppStdAllocator {
public:
    template <typename U>
    friend class CppStdAllocator;

    using value_type = T;

    CppStdAllocator(size_t buffered_size, size_t max_buffered_ptrs_num);

    template <typename U>
    CppStdAllocator(const CppStdAllocator<U>& cpp_std_allocator);

    size_t usage() const;

    T* allocate(std::size_t size);

    void deallocate(T* ptr, std::size_t size);

private:
    std::unordered_map<void*, std::size_t> records_;
    std::shared_ptr<std::size_t> usage_;

    size_t buffered_size_;
    size_t max_buffered_ptrs_num_;
    std::vector<void*> buffered_ptrs_;
};

template <typename T>
constexpr bool operator==(const CppStdAllocator<T>& a,
                          const CppStdAllocator<T>& b);

template <typename T>
constexpr bool operator!=(const CppStdAllocator<T>& a,
                          const CppStdAllocator<T>& b);

// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------

template <typename T>
CppStdAllocator<T>::CppStdAllocator(size_t buffered_size,
                                    size_t max_buffered_ptrs_num)
    : usage_{ new std::size_t{ 0 } },
      buffered_size_{ buffered_size },
      max_buffered_ptrs_num_{ max_buffered_ptrs_num } {}

template <typename T>
template <typename U>
CppStdAllocator<T>::CppStdAllocator(const CppStdAllocator<U>& cpp_std_allocator)
    : usage_{ cpp_std_allocator.usage_ },
      buffered_size_{ cpp_std_allocator.buffered_size_ },
      max_buffered_ptrs_num_{ cpp_std_allocator.max_buffered_ptrs_num_ } {}

template <typename T>
std::size_t CppStdAllocator<T>::usage() const {
    return *this->usage_;
}

template <typename T>
T* CppStdAllocator<T>::allocate(std::size_t size) {
    if (size == 0) { return nullptr; }

    size *= sizeof(T);

    void* ptr;

    if (this->buffered_size_ == size && !this->buffered_ptrs_.empty()) {
        ptr = this->buffered_ptrs_.back();
        this->buffered_ptrs_.pop_back();
    } else {
        ptr = std::malloc(size);
    }

    ZETA_DebugAssert(ptr != nullptr);

    if (size != 48) { ZETA_PrintVar(size); }

#if ZETA_IsDebug
    bool b{ this->records_.insert({ ptr, size }).second };
    ZETA_DebugAssert(b);

    *this->usage_ += size;
#endif

    return static_cast<T*>(ptr);
}

template <typename T>
void CppStdAllocator<T>::deallocate(T* ptr, std::size_t size) {
    if (ptr == nullptr) {
        ZETA_DebugAssert(size == 0);
        return;
    }

#if ZETA_IsDebug
    auto iter{ this->records_.find(static_cast<void*>(ptr)) };
    ZETA_DebugAssert(iter != this->records_.end());
    ZETA_DebugAssert(size <= iter->second);

    *this->usage_ -= iter->second;
    this->records_.erase(iter);
#endif

    if (this->buffered_size_ == size &&
        this->buffered_ptrs_.size() < this->max_buffered_ptrs_num_) {
        this->buffered_ptrs_.push_back(ptr);
    } else {
        std::free(ptr);
    }
}

template <typename T>
constexpr bool operator==(const CppStdAllocator<T>& a,
                          const CppStdAllocator<T>& b) {
    return &a == &b;
}

template <typename T>
constexpr bool operator!=(const CppStdAllocator<T>& a,
                          const CppStdAllocator<T>& b) {
    return &a != &b;
}
