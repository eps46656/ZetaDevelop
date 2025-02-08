#pragma once

#include <define.h>

#include <random>

std::mt19937_64& GetRandomEngine() {
    static std::mt19937_64 en;
    return en;
}

void SetRandomSeed(unsigned random_seed) {
    GetRandomEngine().seed(random_seed);
}

template <typename Int>
constexpr bool_t IsInteger() {
    return std::is_same<Int, char>() ||                //
           std::is_same<Int, unsigned char>() ||       //
           std::is_same<Int, signed char>() ||         //
           std::is_same<Int, unsigned short>() ||      //
           std::is_same<Int, signed short>() ||        //
           std::is_same<Int, unsigned>() ||            //
           std::is_same<Int, signed>() ||              //
           std::is_same<Int, unsigned long>() ||       //
           std::is_same<Int, signed long>() ||         //
           std::is_same<Int, unsigned long long>() ||  //
           std::is_same<Int, signed long long>();      //
}

template <typename RetInt, typename RangeInt>
RetInt GetRandomInt(RangeInt lb, RangeInt rb) {
    static std::uniform_int_distribution<unsigned long long> ll_generator;

    ZETA_StaticAssert(IsInteger<RetInt>());
    ZETA_StaticAssert(IsInteger<RangeInt>());

    return static_cast<RetInt>(
        lb + static_cast<RangeInt>(ll_generator(GetRandomEngine()) %
                                   (static_cast<unsigned long long>(rb) -
                                    static_cast<unsigned long long>(lb) + 1)));
}

template <typename Value>
struct GetRandomCore {
    Value operator()() const {
        std::uniform_int_distribution<Value> generator;
        return generator(GetRandomEngine());
        ;
    }
};

template <typename Value>
Value GetRandom() {
    return GetRandomCore<Value>{}();
}

template <typename Value, typename Iterator>
void GetRandoms(Iterator beg, Iterator end) {
    for (; beg != end; ++beg) { *beg = GetRandom<Value>(); }
}
