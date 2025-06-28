#include "divider.hpp"


#include <iostream>
#include <concepts>
#include <cstdint>
#include <cassert>

namespace detail
{
    constexpr std::uint32_t step(std::uint64_t x, std::uint64_t n)
    {
        return (x * ((2ull<<32) - n*x))>>32;
    }

    template<std::uint32_t N, std::uint32_t X1=0x20000001, std::uint32_t X2 = step(X1, N)>
    struct inverse
    {
        static const std::uint32_t value = inverse<N, X2>::value;
    };

    template<int N>
    struct is_pow_2
    {
        static const bool value = N&1 ? false : is_pow_2<(N>>1)>::value;
    };

    template<>
    struct is_pow_2<1>
    {
        static const bool value = true;
    };

    static_assert(is_pow_2<1>::value);
    static_assert(is_pow_2<16>::value);
    static_assert(!is_pow_2<7>::value);

    template<std::uint32_t N, std::uint32_t X>
    struct inverse<N,X,X>
    {
        static const std::uint32_t value = X+1;
    };
}


template<int Div> constexpr int inverse()
{
    return detail::inverse<Div>::value;
}

template<int Div> int divide(int n)
{
    static_assert(Div>1);
    return ((std::uint64_t)n * (std::uint64_t)detail::inverse<Div>::value)>>32;
}

template<int Div>
void test_div()
{
    //std::cout << "1/" << Div << " = 0x" << std::hex << inverse<Div>() << std::dec << std::endl;
    for(int i=0; i<1000; ++i)
    {
        if(i/Div != divide<Div>(i))
        {
            std::cout << "Failed on " << i << "/" << Div << std::endl;
            std::cout << "Inverse = " << std::hex << "0x" << inverse<Div>() << std::dec << std::endl;
            std::cout << "Result = " << divide<Div>(i) << std::endl;
        }
        assert(i/Div == divide<Div>(i));
    }
    for(int i=0; i<10000000; i+=23)
    {
        assert(i/Div == divide<Div>(i));
    }

    if constexpr (Div>2)
    {
        test_div<Div-1>();
    }
}


int main()
{    
    test_div<250>();
}