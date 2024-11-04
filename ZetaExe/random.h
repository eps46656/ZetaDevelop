#pragma once

#include <define.h>

#include <random>

std::mt19937_64& RandomEngine() {
    static std::mt19937_64 en;
    return en;
}

void SetRandomSeed(unsigned seed) { RandomEngine().seed(seed); }

template <typename Int>
constexpr bool IsInteger() {
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

template <typename Int, typename Range>
Int GetRandomInt(Range lb, Range rb) {
    static std::uniform_int_distribution<unsigned long long> ll_generator{
        0, ZETA_RangeMaxOf(unsigned long long)
    };

    ZETA_StaticAssert(IsInteger<Int>());
    ZETA_StaticAssert(IsInteger<Range>());

    return static_cast<Int>(
        lb + static_cast<Range>(ll_generator(RandomEngine()) %
                                (static_cast<unsigned long long>(rb) -
                                 static_cast<unsigned long long>(lb))));
}

template <typename Int>
Int GetRandom() {
    return GetRandomInt<Int>(ZETA_RangeMinOf(Int), ZETA_RangeMaxOf(Int));
}

template <typename Value, typename Iterator>
void GetRandoms(Iterator beg, Iterator end) {
    for (; beg != end; ++beg) { *beg = GetRandom<Value>(); }
}
