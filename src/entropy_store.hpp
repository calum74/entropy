#pragma once
#include <cassert>
#include <cmath>
#include <cstdint>
#include <random>
#include <span>
#include <vector>

namespace entropy_store
{
template <typename Distribution>
concept distribution = requires(Distribution dist) {
    typename Distribution::value_type;
    dist.min();
    dist.max();
};

template <int N> struct count_bits
{
    static const int value = 1 + count_bits<(N >> 1)>::value;
};

template <> struct count_bits<0>
{
    static const int value = 0;
};

template <std::integral T, T Min, T Max> class const_uniform_distribution
{
    static_assert(Min <= Max, "Invalid uniform distribution");

  public:
    using value_type = T;
    using size_type = std::size_t;

    constexpr size_type size() const
    {
        return Max - Min + 1;
    }
    constexpr size_type bits() const
    {
        return count_bits<Max - Min + 1>::value;
    }
    constexpr value_type min() const
    {
        return Min;
    }
    constexpr value_type max() const
    {
        return Max;
    }
};

template <std::uint32_t Min, std::uint32_t Max>
using const_uniform = const_uniform_distribution<std::uint32_t, Min, Max>;

template <std::integral T> class uniform_distribution
{
  public:
    using value_type = T;
    using size_type = std::size_t;

    uniform_distribution(value_type a, value_type b) : m_min(a), m_max(b), m_bits(std::ceil(std::log2(size())))
    {
    }

    size_type size() const
    {
        return max() - min() + 1;
    }
    size_type bits() const
    {
        return m_bits;
    }
    value_type min() const
    {
        return m_min;
    }
    value_type max() const
    {
        return m_max;
    }

  private:
    int m_min, m_max;     // Inclusive values, min<=max
    std::uint32_t m_bits; // Number of bits capacity required to fetch this
};

using binary_distribution = const_uniform<0, 1>;

template <std::integral uint_t, uint_t M, uint_t N> class const_bernoulli_distribution
{
    static_assert(0 <= M && M <= N, "Invalid Bernoulli distribution");

  public:
    using size_type = std::size_t;
    using value_type = std::uint32_t;

    constexpr size_type numerator() const
    {
        return M;
    }
    constexpr size_type denominator() const
    {
        return N;
    }
    constexpr value_type min() const
    {
        return 0;
    }
    constexpr value_type max() const
    {
        return 1;
    }
};

class bernoulli_distribution
{
  public:
    using size_type = std::size_t;
    using value_type = std::uint32_t;

    bernoulli_distribution(size_type numerator, size_type denominator)
        : m_numerator(numerator), m_denominator(denominator)
    {
    }

    size_type numerator() const
    {
        return m_numerator;
    }
    size_type denominator() const
    {
        return m_denominator;
    }
    constexpr value_type min() const
    {
        return 0;
    }
    constexpr value_type max() const
    {
        return 1;
    }

  private:
    size_type m_numerator, m_denominator;
};

template <std::uint32_t M, std::uint32_t N> using const_bernoulli = const_bernoulli_distribution<std::uint32_t, M, N>;

// Constructs the lookup tables for a weighted distribution
class weighted_distribution
{
  public:
    using value_type = std::uint32_t;
    using size_type = std::size_t;

    weighted_distribution(std::initializer_list<value_type> weights) : weighted_distribution(std::vector(weights))
    {
    }

    weighted_distribution(std::vector<value_type> w) : m_weights(std::move(w))
    {
        for (int i = 0; i < m_weights.size(); ++i)
        {
            m_offsets.push_back(m_outputs.size());
            for (int j = 0; j < m_weights[i]; ++j)
            {
                m_outputs.push_back(i);
            }
        }
        m_bits = std::ceil(std::log2(m_outputs.size()));
    }

    size_type bits() const
    {
        return m_bits;
    }

    std::span<const value_type> weights() const
    {
        return m_weights;
    }
    std::span<const value_type> outputs() const
    {
        return m_outputs;
    }
    std::span<const value_type> offsets() const
    {
        return m_offsets;
    }

    value_type min() const
    {
        return 0;
    }

    value_type max() const
    {
        return m_weights.size() - 1;
    }

  private:
    std::uint32_t m_bits;
    std::vector<value_type> m_weights, m_outputs, m_offsets;
};

template <typename Source>
concept entropy_generator = requires(Source source) {
    typename Source::value_type;
    typename Source::distribution_type;
    source.distribution(); // !! Check types
    source();              // !! Check types
};

class random_device_generator
{
  public:
    using value_type = std::uint32_t;
    using distribution_type = const_uniform<std::random_device::min(), std::random_device::max()>;
    random_device_generator()
    {
    }
    random_device_generator(const random_device_generator &)
    {
    }

    value_type operator()()
    {
        return m_rd();
    }

    distribution_type distribution() const
    {
        return {};
    }

  private:
    std::random_device m_rd;
};

template <entropy_generator Source> class bit_generator
{
  public:
    using source_type = Source;
    using value_type = typename Source::value_type;
    using distribution_type = binary_distribution;

    const source_type &source() const
    {
        return m_source;
    }

    bit_generator(const source_type &source = {}) : m_source(source)
    {
    }
    bit_generator(const bit_generator &src) : m_source(src.source())
    {
    }
    bit_generator(bit_generator &&src) : m_source(std::move(src.m_source))
    {
    }

    distribution_type distribution() const
    {
        return {};
    }

    value_type operator()()
    {
        if (++m_shift >= m_bits)
        {
            m_value = m_source();
            m_shift = 0;
        }
        return (m_value >> m_shift) & 1;
    }

  private:
    constexpr static value_type m_bits = 8 * sizeof(value_type);
    value_type m_value = 0, m_shift = m_bits;
    Source m_source;
};

using random_bit_generator = bit_generator<random_device_generator>;

template <std::integral uint_t> void validate(uint_t U_n, uint_t n)
{
    assert(U_n < n);
}

template <std::integral uint_t> auto combine(uint_t U_n, uint_t n, uint_t U_m, uint_t m)
{
    validate(U_n, n);
    validate(U_m, m);
    uint_t U_nm = n * U_m + U_n;
    uint_t nm = m * n;
    assert(n <= nm);
    assert(m <= nm);
    validate(U_nm, nm);
    return std::tuple{U_nm, nm};
}

template <std::integral uint_t> auto divide(uint_t U_nm, uint_t nm, uint_t m)
{
    uint_t U_m = U_nm % m;
    uint_t U_n = U_nm / m;
    uint_t n = nm / m;
    return std::tuple{U_n, n, U_m};
}

template <std::integral uint_t> auto resample(uint_t U_n, uint_t n, uint_t m)
{
    uint_t U_x, x, B;
    if (U_n < m)
    {
        U_x = U_n;
        x = m;
        B = 1;
    }
    else
    {
        U_x = U_n - m;
        x = n - m;
        B = 0;
    }
    return std::tuple{U_x, x, B};
}

template <std::integral uint_t, std::invocable<uint_t, uint_t> Fn>
auto generate_multiple(uint_t U_s, uint_t s, uint_t N, uint_t n, Fn fetch_entropy)
{
    assert(N >= n);
    validate(U_s, s);
    for (;;)
    {
        while (s < N)
            std::tie(U_s, s) = fetch_entropy(U_s, s);
        assert(s >= n);
        validate(U_s, s);
        // Resample entropy s to a multiple of m
        uint_t k = s / n;
        uint_t r = s % n;
        uint_t B;
        std::tie(U_s, s, B) = resample(U_s, s, s - r);
        // s -= r;
        if (B) [[likely]]
        {
            // Resample successful
            validate(U_s, s);
            assert(k == s / n);
            return std::tuple{U_s, s, k};
        }
    }
}

template <std::uint32_t n, std::integral uint_t, std::invocable<uint_t, uint_t> Fn>
auto generate_const_multiple(uint_t U_s, uint_t s, uint_t N, Fn fetch_entropy)
{
    assert(N >= n);
    for (;;)
    {
        while (s < N)
            std::tie(U_s, s) = fetch_entropy(U_s, s);
        assert(s >= n);
        validate(U_s, s);
        // Resample entropy s to a multiple of m
        uint_t k = s / n;
        uint_t r = s % n;
        uint_t B;
        std::tie(U_s, s, B) = resample(U_s, s, s - r);
        // s -= r;
        if (B) [[likely]]
        {
            // Resample successful
            validate(U_s, s);
            assert(k == s / n);
            return std::tuple{U_s, s, k};
        }
    }
}

template <std::integral uint_t, std::invocable<uint_t, uint_t> Fn>
auto generate_uniform(uint_t U_s, uint_t s, uint_t N, uint_t n, Fn fetch_entropy)
{
    uint_t k;
    uint_t U_n;
    std::tie(U_s, s, k) = generate_multiple(U_s, s, N, n, fetch_entropy);
    std::tie(U_s, s, U_n) = divide(U_s, s, n);
    validate(U_s, s);
    validate(U_n, n);
    return std::tuple{U_s, s, U_n};
}

template <std::uint32_t n, std::integral uint_t, std::invocable<uint_t, uint_t> Fn>
auto generate_const_uniform(uint_t U_s, uint_t s, uint_t N, Fn fetch_entropy)
{
    uint_t k;
    uint_t U_n;
    std::tie(U_s, s, k) = generate_const_multiple<n>(U_s, s, N, fetch_entropy);
    std::tie(U_s, s, U_n) = divide(U_s, s, n);
    validate(U_s, s);
    validate(U_n, n);
    return std::tuple{U_s, s, U_n};
}

template <std::integral uint_t, std::integral uint2>
auto fetch_from_source(entropy_generator auto &source, const uniform_distribution<uint2> &source_dist)
{
    return [&](uint_t U_s, uint_t s) {
        return combine(U_s, s, uint_t(source() - source_dist.min()), uint_t(source_dist.size()));
    };
}

template <std::integral uint_t, std::integral uint2, uint2 Min, uint2 Max>
auto fetch_from_source(entropy_generator auto &source, const const_uniform_distribution<uint2, Min, Max> &source_dist)
{
    return [&](uint_t U_s, uint_t s) {
        return combine(U_s, s, uint_t(source() - source_dist.min()), uint_t(source_dist.size()));
    };
}

template <std::integral uint_t>
auto fetch_from_source(entropy_generator auto &source, const binary_distribution &source_dist)
{
    return [&](uint_t U_s, uint_t s) { return std::tuple{(U_s << 1) | uint_t(source()), s << 1}; };
}

template <std::integral uint_t, std::integral T>
T generate(uint_t &U_s, uint_t &s, uint_t N, std::invocable<uint_t, uint_t> auto fetch_entropy,
           const uniform_distribution<T> &output_dist)
{
    uint_t U_n;
    std::tie(U_s, s, U_n) = generate_uniform(U_s, s, N, uint_t(output_dist.size()), fetch_entropy);
    return U_n + output_dist.min();
}

template <std::integral uint_t>
uint_t generate(uint_t &U_s, uint_t &s, uint_t N, std::invocable<uint_t, uint_t> auto fetch_entropy,
                const bernoulli_distribution &output_dist)
{
    uint_t m = output_dist.numerator();
    uint_t n = output_dist.denominator();
    assert(m < n);
    uint_t k;
    std::tie(U_s, s, k) = generate_multiple(U_s, s, N, n, fetch_entropy);
    assert(s = k * n);
    uint_t M = k * m;
    assert(M >= k);
    assert(M >= m);
    uint_t b;
    std::tie(U_s, s, b) = resample(U_s, s, M);
    return b;
}

template <std::integral uint_t, uint_t Num, uint_t Den>
uint_t generate(uint_t &U_s, uint_t &s, uint_t N, std::invocable<uint_t, uint_t> auto fetch_entropy,
                const const_bernoulli_distribution<uint_t, Num, Den> &output_dist)
{
    uint_t m = output_dist.numerator();
    uint_t n = output_dist.denominator();
    uint_t k;
    std::tie(U_s, s, k) = generate_const_multiple<Den>(U_s, s, N, fetch_entropy);
    uint_t M = k * m;
    uint_t b;
    std::tie(U_s, s, b) = resample(U_s, s, M);
    return b;
}

template <std::integral uint_t>
uint_t generate(uint_t &U_s, uint_t &s, uint_t N, std::invocable<uint_t, uint_t> auto fetch_entropy,
                const weighted_distribution &output_dist)
{
    uint_t k;

    std::tie(U_s, s, k) =
        generate_multiple(U_s, s, N, (uint_t)output_dist.outputs().size(), fetch_entropy);
    auto W = output_dist.outputs()[U_s / k];
    U_s -= k * output_dist.offsets()[W];
    s = k * output_dist.weights()[W];
    return W;
}

template <std::integral uint_t, uint_t Min, uint_t Max>
uint_t generate(uint_t &U_s, uint_t &s, uint_t N, std::invocable<uint_t, uint_t> auto fetch_entropy,
                const const_uniform_distribution<uint_t, Min, Max> &output_dist)
{
    uint_t U_n;
    std::tie(U_s, s, U_n) = generate_const_uniform<Max - Min + 1>(U_s, s, N, fetch_entropy);
    return U_n + output_dist.min();
}

template <entropy_generator Source, std::integral Buffer = std::uint32_t> class entropy_store
{
  public:
    using value_type = Buffer;
    using source_type = Source;

    entropy_store(const Source &src) : m_source(src)
    {
    }

    entropy_store(entropy_store &&other) : m_source(std::move(other.m_source)), U_s(other.U_s), s(other.s)
    {
        other.U_s = 0;
        other.s = 1;
    }

    entropy_store(const entropy_store &other) : m_source(other.m_source)
    {
    }

    auto operator()(const distribution auto &dist)
    {
        return generate(U_s, s, N, fetch_from_source<value_type>(m_source, m_source.distribution()), dist);
    }

    value_type size() const
    {
        return s;
    }

    const source_type &source() const
    {
        return m_source;
    }

  private:
    source_type m_source;
    value_type N = value_type(1) << (sizeof(value_type) * 8 - m_source.distribution().bits());
    value_type U_s = 0, s = 1;
};

template <entropy_generator Source, distribution Distribution, std::integral Buffer = std::uint32_t>
struct entropy_converter
{
    using source_type = Source;
    using distribution_type = Distribution;
    using value_type = typename distribution_type::value_type;
    entropy_converter(const source_type &source, const distribution_type &distribution)
        : m_source(source), m_distribution(distribution)
    {
    }

    auto operator()()
    {
        return m_source(m_distribution);
    }

    const distribution_type &distribution() const
    {
        return m_distribution;
    }

    const auto &source() const
    {
        return m_source;
    }

    entropy_store<source_type, Buffer> m_source;
    distribution_type m_distribution;
};

template <entropy_generator Source, distribution Distribution>
using entropy_converter64 = entropy_converter<Source, Distribution, std::uint64_t>;

template <entropy_generator Source> using entropy_store32 = entropy_store<Source, std::uint32_t>;

template <entropy_generator Source> using entropy_store64 = entropy_store<Source, std::uint64_t>;

template <entropy_generator Source, std::integral Buffer, std::random_access_iterator It>
void shuffle(entropy_store<Source, Buffer> &store, It a, It b)
{
    auto size = std::distance(a, b);
    for (int i = 1; i < size; ++i)
        std::swap(a[i], a[store(uniform_distribution{0, i})]);
}

template <entropy_generator Source, std::integral Buffer>
void shuffle(entropy_store<Source, Buffer> &store, std::ranges::random_access_range auto &cards)
{
    return shuffle(store, cards.begin(), cards.end());
}
} // namespace entropy_store
