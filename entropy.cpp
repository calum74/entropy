#include <iostream>
#include <random>

template<int N, int V=1, typename Src>
int fast_dice_roller(Src&& src, int c=0)
{
    c = 2*c + src();
    if constexpr ((2*V)>=N)
    {
        if (c<N)
            return c;
        else 
            return fast_dice_roller<N, 2*V-N>(src, c-N);
    }
    else
    {
        return fast_dice_roller<N,2*V>(src, c);
    }
}

// Binary entropy stream

template<typename Generator>
class binary_entropy_stream
{
public:
    using value_type = std::uint32_t;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type&;
    using iterator_category = std::input_iterator_tag;
    using generator_type = Generator;

    binary_entropy_stream() : m_value(m_random()), m_shift(0) {}
    binary_entropy_stream(binary_entropy_stream&&other)
    {
        m_value = other.m_value;
        m_shift = other.m_shift;
    }

    binary_entropy_stream& operator=(binary_entropy_stream&&other) { return *this; }

    value_type operator*() const { return (m_value>>m_shift)&1; }

    binary_entropy_stream & operator++() {
        if(++m_shift>=32)
        {
            m_value = m_random();
            m_shift = 0;
        }
        return *this;
    }

    binary_entropy_stream & operator++(int)
    {
        return ++*this;
    }

    bool operator==(const binary_entropy_stream&) const { return true; }
    bool operator!=(const binary_entropy_stream&) const { return false; }

private:
    std::random_device m_random;

    // We should really handle arbitrary sizes,
    // but for now, assume 32-bit (and check it!)
    static_assert(generator_type::min()==0);
    static_assert(generator_type::max()==0xffffffff);
    value_type m_value, m_shift;
};


template<std::unsigned_integral T = std::uint32_t, T M = (1u<<((sizeof(T)*8)-1))>
class efficient_entropy_converter
{
public:
    using value_type = T;

    template<std::invocable Fn>
    value_type convert(Fn fetch, value_type m)
    {
        for(;;)
        {
            // Preload entropy one bit at a time into s
            while(s_range < M)
            {
                s_value <<= 1;
                s_value |= fetch();
                s_range <<= 1;
            }
            // Resample entropy s to a multiple of m
            auto r = s_range / m;
            auto c = s_range % m;
            if(s_value >= c)  [[likely]]
            {
                // Resample successful
                s_value -= c;
                auto a = s_value / m;
                auto b = s_value % m;
                s_value = a;
                s_range = r; 
                return b;
            }
            else
            {
                // Resample unsuccessful
                s_range = c;
            }
        }
    }

    template<std::input_iterator It>
    value_type convert(It & binary_entropy, value_type m)
    {
        return convert([&] { return *binary_entropy++; }, m);
    }

    efficient_entropy_converter() : s_value(0), s_range(1) {}

    efficient_entropy_converter(efficient_entropy_converter && other) : s_value(other.s_value), s_range(other.s_range)
    {
        other.s_value = 0;
        other.s_range = 1;
    }

    efficient_entropy_converter & operator=(efficient_entropy_converter &&other)
    {
        s_value = other.s_value;
        s_range = other.s_range;
        other = {};
        return *this;
    }

private:
    value_type s_value = 0, s_range = 1;
};



int main()
{
    std::random_device rd;
    std::uniform_int_distribution u {0,1};
    auto entropy = [&] { return u(rd); };
    std::cout << fast_dice_roller<47>(entropy) << std::endl;

    binary_entropy_stream<std::random_device> b;
    efficient_entropy_converter<std::uint32_t> ec;

    for(int i=0; i<100; ++i)
        std::cout << *b++;
    std::cout << std::endl;
    for(int i=0; i<100; ++i)
        std::cout << ec.convert(b, 6);
    std::cout << std::endl;
}