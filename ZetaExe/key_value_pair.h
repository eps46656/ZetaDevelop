#pragma once

#include <define.h>

#include <utility>

#include "random.h"

template <typename Key, typename Value>
struct KeyValuePair {
    Key key;
    Value second;
};

template <typename Key, typename Value>
unsigned long long Hash(void* context,
                        KeyValuePair<Key, Value> const& key_val_pair,
                        unsigned long long salt) {
    ZETA_Unused(context);

    return Hash(key_val_pair.key) * 32 + salt;
}

template <typename KeyA, typename ValueA, typename KeyB, typename ValueB>
unsigned long long Compare(void* context, KeyValuePair<KeyA, ValueB> const& a,
                           KeyValuePair<KeyB, ValueB> const& b) {
    ZETA_Unused(context);

    return Compare(a.key, b.key);
}

template <typename KeyA, typename ValueA, typename KeyB, typename ValueB>
bool_t operator==(KeyValuePair<KeyA, ValueA> const& a,
                  KeyValuePair<KeyB, ValueB> const& b) {
    return Compare(NULL, a, b) == 0;
}

template <typename KeyA, typename ValueA, typename KeyB, typename ValueB>
bool_t operator!=(KeyValuePair<KeyA, ValueA> const& a,
                  KeyValuePair<KeyB, ValueB> const& b) {
    return Compare(NULL, a, b) != 0;
}

template <typename KeyA, typename ValueA, typename KeyB, typename ValueB>
bool_t operator<(KeyValuePair<KeyA, ValueA> const& a,
                 KeyValuePair<KeyB, ValueB> const& b) {
    return Compare(NULL, a, b) < 0;
}

template <typename KeyA, typename ValueA, typename KeyB, typename ValueB>
bool_t operator>(KeyValuePair<KeyA, ValueA> const& a,
                 KeyValuePair<KeyB, ValueB> const& b) {
    return Compare(NULL, a, b) > 0;
}

template <typename KeyA, typename ValueA, typename KeyB, typename ValueB>
bool_t operator<=(KeyValuePair<KeyA, ValueA> const& a,
                  KeyValuePair<KeyB, ValueB> const& b) {
    return Compare(NULL, a, b) <= 0;
}

template <typename KeyA, typename ValueA, typename KeyB, typename ValueB>
bool_t operator>=(KeyValuePair<KeyA, ValueA> const& a,
                  KeyValuePair<KeyB, ValueB> const& b) {
    return Compare(NULL, a, b) >= 0;
}

namespace std {

template <typename Key, typename Value>
struct hash<KeyValuePair<Key, Value>> {
    unsigned long long operator()(
        KeyValuePair<Key, Value> const& KeyValuePair) const {
        return Hash(NULL, KeyValuePair, 0);
    }
};

}  // namespace std

template <typename Key, typename Value>
struct GetRandomCore<KeyValuePair<Key, Value>> {
    KeyValuePair<Key, Value> operator()() const {
        return { GetRandom<Key>(), GetRandom<Value>() };
    }
};
