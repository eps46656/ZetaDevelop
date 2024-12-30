#include <algorithm>
#include <vector>

template <typename Elem, typename Compare>
class StaticSearchTable {
public:
    static constexpr size_t threshold{ 8 };

    struct Node {
        Node* p;
        Node* l;
        Node* r;

        Elem* seg;
        size_t seg_size;

        bool is_finished;
    };

    StaticSearchTable(const std::vector<Elem>& vec, const Compare& cmp);

    ~StaticSearchTable();

    template <typename Key>
    const Elem* LowerBound(const Key& key);

    template <typename Key>
    const Elem* UpperBound(const Key& key);

private:
    Compare cmp_;

    std::vector<Elem> vec_;

    Node* root_;

    void Destroy_(Node* n);

    template <typename Key>
    const Elem* BinaryLowerBound_(const Elem* seg, size_t seg_size,
                                  const Key& key);

    template <typename Key>
    const Elem* BinaryUpperBound_(const Elem* seg, size_t seg_size,
                                  const Key& key);

    template <typename Key>
    const Elem* LinearLowerBound_(const Elem* seg, size_t seg_size,
                                  const Key& key);

    template <typename Key>
    const Elem* LinearUpperBound_(const Elem* seg, size_t seg_size,
                                  const Key& key);

    void CheckIsFinished_(Node* n);
};

// -----------------------------------------------------------------------------

template <typename Elem, typename Compare>
StaticSearchTable<Elem, Compare>::StaticSearchTable(
    const std::vector<Elem>& vec, const Compare& cmp)
    : cmp_{ cmp }, vec_{ vec } {
    if (this->vec_.size() <= threshold) {
        sort(this->vec_.begin(), this->vec_.end(), this->cmp_);

        this->root_ = nullptr;

        this->is_finished_ = true;
    } else {
        this->root_ = new Node{
            .p = nullptr,
            .l = nullptr,
            .r = nullptr,
            .seg = &this->vec_[0],
            .seg_size = this->vec_.size(),
        };

        this->is_finished_ = false;
    }
}

template <typename Elem, typename Compare>
void StaticSearchTable<Elem, Compare>::Destroy_(Node* n) {
    if (n == nullptr) { return; }
    this->Destroy_(n->l);
    this->Destroy_(n->r);
    delete n;
}

template <typename Elem, typename Compare>
StaticSearchTable<Elem, Compare>::~StaticSearchTable() {
    Destroy_(this->root_);
}

template <typename Elem, typename Compare>
template <typename Key>
const Elem* StaticSearchTable<Elem, Compare>::LowerBound(const Key& key) {
    if (this->is_finished_) {
        return this->BinaryLowerBound_(&this->vec_[0], this->vec_.size(), key);
    }

    const Elem* ret{ nullptr };

    Node* n{ this->root_ };

    int quata{ 2 };

    while (n != nullptr) {
        if (n->is_finished) {
            const Elem* cur{ this->BinaryLowerBound_(n->seg, n->seg_size,
                                                     key) };

            return cur == nullptr ? ret : cur;
        }

        if (1 < n->seg_size) {
            if (n->seg_size <= threshold) {
                std::sort(n->seg, n->seg + n->seg_size, this->cmp_);
                n->is_finished = true;

                const Elem* cur{ this->BinaryLowerBound_(n->seg, n->seg_size,
                                                         key) };

                this->CheckIsFinished_(n);

                return cur == nullptr ? ret : cur;
            }

            if (quata == 0) {
                return this->LinearLowerBound_(n->seg, n->seg_size, key);
            }

            size_t half_seg_size{ n->seg_size / 2 };

            Elem* beg{ n->seg };
            Elem* mid{ n->seg + half_seg_size };
            Elem* end{ n->seg + n->seg_size };

            std::nth_element(beg, mid, end, this->cmp_);

            n->l = new Node{
                .p = n,
                .l = nullptr,
                .r = nullptr,
                .seg = beg,
                .seg_size = half_seg_size,
                .is_finished = false,
            };

            n->r = new Node{
                .p = n,
                .l = nullptr,
                .r = nullptr,
                .seg = mid + 1,
                .seg_size = n->seg_size - 1 - half_seg_size,
                .is_finished = false,
            };

            --quata;
        }

        if (this->cmp_(*n->seg, key)) {
            n = n->r;
        } else {
            ret = n->seg;
            n = n->l;
        }
    }

    return ret;
}

template <typename Elem, typename Compare>
template <typename Key>
const Elem* StaticSearchTable<Elem, Compare>::UpperBound(const Key& key) {
    if (this->is_finished_) {
        return this->BinaryUpperBound_(&this->vec_[0], this->vec_.size(), key);
    }

    const Elem* ret{ nullptr };

    Node* n{ this->root_ };

    int quata{ 2 };

    while (n != nullptr) {
        if (n->is_finished) {
            const Elem* cur{ this->BinaryUpperBound_(n->seg, n->seg_size,
                                                     key) };

            return cur == nullptr ? ret : cur;
        }

        if (1 < n->seg_size) {
            if (n->seg_size <= threshold) {
                std::sort(n->seg, n->seg + n->seg_size, this->cmp_);
                n->is_finished = true;

                const Elem* cur{ this->BinaryUpperBound_(n->seg, n->seg_size,
                                                         key) };

                this->CheckIsFinished_(n);

                return cur == nullptr ? ret : cur;
            }

            if (quata == 0) {
                return this->LinearUpperBound_(n->seg, n->seg_size, key);
            }

            size_t half_seg_size{ n->seg_size / 2 };

            Elem* beg{ n->seg };
            Elem* mid{ n->seg + half_seg_size };
            Elem* end{ n->seg + n->seg_size };

            std::nth_element(beg, mid, end, this->cmp_);

            n->l = new Node{
                .p = n,
                .l = nullptr,
                .r = nullptr,
                .seg = beg,
                .seg_size = half_seg_size,
                .is_finished = false,
            };

            n->r = new Node{
                .p = n,
                .l = nullptr,
                .r = nullptr,
                .seg = mid + 1,
                .seg_size = n->seg_size - 1 - half_seg_size,
                .is_finished = false,
            };

            --quata;
        }

        if (this->cmp_(key, *n->seg)) {
            ret = n->seg;
            n = n->l;
        } else {
            n = n->r;
        }
    }

    return ret;
}

template <typename Elem, typename Compare>
template <typename Key>
const Elem* StaticSearchTable<Elem, Compare>::BinaryLowerBound_(
    const Elem* seg, size_t seg_size, const Key& key) {
    size_t lb{ 0 };
    size_t rb{ seg_size };

    while (lb < rb) {
        size_t mb{ (lb + rb) / 2 };

        if (this->cmp_(seg[mb], key)) {
            lb = mb + 1;
        } else {
            rb = mb;
        }
    }

    return lb < seg_size ? seg + lb : nullptr;
}

template <typename Elem, typename Compare>
template <typename Key>
const Elem* StaticSearchTable<Elem, Compare>::BinaryUpperBound_(
    const Elem* seg, size_t seg_size, const Key& key) {
    size_t lb{ 0 };
    size_t rb{ seg_size };

    while (lb < rb) {
        size_t mb{ (lb + rb + 1) / 2 };

        if (this->cmp_(key, seg[mb])) {
            rb = mb;
        } else {
            lb = mb + 1;
        }
    }

    return lb < seg_size ? seg + lb : nullptr;
}

template <typename Elem, typename Compare>
template <typename Key>
const Elem* StaticSearchTable<Elem, Compare>::LinearLowerBound_(
    const Elem* seg, size_t seg_size, const Key& key) {
    const Elem* ret{ nullptr };

    for (size_t i{ 0 }; i < seg_size; ++i) {
        const Elem* cur{ seg + i };

        if (!this->cmp_(*cur, key) &&
            (ret == nullptr || this->cmp_(*cur, *ret))) {
            ret = cur;
        }
    }

    return ret;
}

template <typename Elem, typename Compare>
template <typename Key>
const Elem* StaticSearchTable<Elem, Compare>::LinearUpperBound_(
    const Elem* seg, size_t seg_size, const Key& key) {
    const Elem* ret{ nullptr };

    for (size_t i{ 0 }; i < seg_size; ++i) {
        const Elem* cur{ seg + i };

        if (this->cmp_(key, *cur) &&
            (ret == nullptr || this->cmp_(*cur, *ret))) {
            ret = cur;
        }
    }

    return ret;
}

template <typename Elem, typename Compare>
void StaticSearchTable<Elem, Compare>::CheckIsFinished_(Node* n) {
    while (n != nullptr) {
        if (n->is_finished) { n = n->p; }

        Node* nl{ n->l };
        Node* nr{ n->r };

        if ((nl != nullptr && !nl->is_finished) ||
            (nr != nullptr && !nr->is_finished)) {
            break;
        }

        if (nl != nullptr) {
            n->seg = nl->seg;
            n->seg_size += nl->seg_size;
        }

        if (nr != nullptr) { n->seg_size += nr->seg_size; }

        delete nl;
        delete nr;

        n->l = nullptr;
        n->r = nullptr;

        n->is_finished = true;

        n = n->p;
    }

    delete this->root_;

    this->root_ = nullptr;

    this->is_finished_ = true;
}
