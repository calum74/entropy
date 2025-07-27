#pragma once
#include <iostream>
#include <random>

namespace entropy_store
{
// Checks that multiple samplings of a uniform distribution
// occur in the expected range. According to the central limit theorem
// However, they are not independent.
class debug_uniform
{
  public:
    using value_type = std::uint64_t;
    void record(value_type value, value_type size)
    {
        assert(value < size);
        total_value += value;
        total_size += size - 1;
        total_count++;
        sum_s_squared += size * size - 1;
    }

    double sd() const
    {
        return std::sqrt(sum_s_squared / 12.0);
    }

    double mean() const
    {
        return total_size / 2.0;
    }

    double get_relative_bias() const
    {
        return (total_value - mean()) / sd();
    }

    double get_percentage_bias() const
    {
        return 100.0 * (total_value - mean()) / total_value;
    }

    debug_uniform(const char *place) : place(place)
    {
    }

    ~debug_uniform()
    {
        std::cout << place << ": uniform distribution bias = " << get_percentage_bias() << "%\n";
    }

  private:
    const char *place;
    std::uint64_t total_value = 0, total_size = 0, total_count = 0;
    double sum_s_squared = 0;
};

template <entropy_generator Source> class stored_source
{
  public:
    using value_type = typename Source::value_type;
    using distribution_type = typename Source::distribution_type;
    using size_type = std::size_t;
    using source_type = typename Source::source_type;

    stored_source(const Source &source, size_type count) : m_source(source)
    {
        m_values.reserve(count);
        for (; count > 0; --count)
            m_values.push_back(m_source());
    }

    auto operator()()
    {
        if (m_values.empty())
            throw std::runtime_error("Out of values!");
        auto result = m_values.back();
        m_values.pop_back();
        return result;
    }

    auto distribution() const
    {
        return m_source.distribution();
    }

    int fetch_bit()
    {
        return m_source.fetch_bit();
    }

    std::vector<value_type> m_values;

  private:
    Source m_source;
};

template <entropy_generator Source> class wrapped_source
{
  public:
    using value_type = typename Source::value_type;
    using distribution_type = typename Source::distribution_type;
    using size_type = std::size_t;
    using source_type = Source;

    wrapped_source(const Source &source, size_type count) : m_source(source)
    {
        m_values.reserve(count);
        for (; count > 0; --count)
            m_values.push_back(m_source());
    }

    auto operator()()
    {
        ++m_index;
        if (m_index >= m_values.size())
            m_index = 0;
        return m_values[m_index];
    }

    auto distribution() const
    {
        return m_source.distribution();
    }

    int fetch_bit()
    {
        return m_source.fetch_bit();
    }

    std::vector<value_type> m_values;

  private:
    Source m_source;
    std::size_t m_index = 0;
};

template <binary_entropy_generator Source> class fast_dice_roller
{
  public:
    fast_dice_roller(const Source &source) : m_source(source)
    {
    }

    template <std::integral T> auto operator()(const uniform_distribution<T> &dist)
    {
        T r = 0, s = 1;
        const auto N = dist.size();
        for (;;)
        {
            while (s < N)
            {
                s = s << 1;
                r = (r << 1) | m_source();
            }
            if (r < N)
            {
                return r + dist.min();
            }
            r -= N;
            s -= N;
        }
    }

    const Source &source() const
    {
        return m_source;
    }

  private:
    Source m_source;
};

template <binary_entropy_generator Source> class von_neumann
{
  public:
    von_neumann(const Source &source) : m_source(source)
    {
    }

    template <std::integral T> auto operator()(const uniform_distribution<T> &dist)
    {
        const auto N = dist.size();
        for (;;)
        {
            T r = 0, s = 1;
            while (s < N)
            {
                s = s << 1;
                r = (r << 1) | m_source();
            }
            if (r < N)
            {
                return r + dist.min();
            }
        }
    }

    const Source &source() const
    {
        return m_source;
    }

  private:
    Source m_source;
};

template <typename Source, distribution Dist> class bound_entropy_generator
{
  public:
    bound_entropy_generator(const Source &source, const Dist &dist) : m_source(source), m_dist(dist)
    {
    }

    auto operator()()
    {
        return m_source(m_dist);
    }

    using distribution_type = Dist;
    using value_type = typename distribution_type::value_type;
    using source_type = Source;

    const Dist &distribution() const
    {
        return m_dist;
    }

    const source_type &source() const
    {
        return m_source;
    }

  private:
    Source m_source;
    Dist m_dist;
};

template <typename Source, distribution Dist> auto bits_fetched(const bound_entropy_generator<Source, Dist> &g)
{
    return bits_fetched(g.source());
}

template <typename Source, distribution Dist> auto internal_entropy(const bound_entropy_generator<Source, Dist> &g)
{
    return 0;
}

template <typename Source> auto bits_fetched(const von_neumann<Source> &g)
{
    return bits_fetched(g.source());
}

template <typename Source> auto bits_fetched(const fast_dice_roller<Source> &g)
{
    return bits_fetched(g.source());
}

// Lemire's nearly divisionless (See appendix A)

uint64_t nearlydivisionless(uint64_t s, auto random64)
{
    uint64_t x = random64();
    __uint128_t m = (__uint128_t)x * (__uint128_t)s;
    uint64_t l = (uint64_t)m;
    if (l < s) [[unlikely]]
    {
        uint64_t t = -s % s;
        while (l < t)
        {
            x = random64();
            m = (__uint128_t)x * (__uint128_t)s;
            l = (uint64_t)m;
        }
    }
    return m >> 64;
}

template <entropy_generator Source> class lemire
{
  public:
    lemire(const Source &source) : m_source(source)
    {
    }

    template <std::integral T> auto operator()(const uniform_distribution<T> &dist)
    {
        return nearlydivisionless(dist.size(), [&]() { return (uint64_t(m_source()) << 32) | m_source(); }) +
               dist.min();
    }

    const auto &source() const
    {
        return m_source;
    }

  private:
    Source m_source;
};

template <typename PRNG = std::mt19937> class prng_source
{
  public:
    prng_source(const PRNG &prng = {}) : m_prng(prng)
    {
    }

    using value_type = typename PRNG::result_type;
    auto max() const
    {
        return m_prng.max();
    }
    auto min() const
    {
        return m_prng.min();
    }

    value_type operator()()
    {
        return m_prng();
    }

    using distribution_type = const_uniform<PRNG::min(), PRNG::max()>;

    distribution_type distribution() const
    {
        return distribution_type{};
    }

  private:
    PRNG m_prng;
};

template <entropy_generator Source> auto bits_fetched(const lemire<Source> &l)
{
    return bits_fetched(l.source());
}

// Code copied from https://prng.di.unimi.it/xoshiro128plusplus.c
class xoshiro128
{
  public:
    xoshiro128(entropy_generator auto &seed)
    {
        for (int i = 0; i < 4; i++)
            s[i] = seed();
    }

    using value_type = uint32_t;
    using distribution_type = const_uniform_distribution<value_type, std::numeric_limits<value_type>::min(),
                                                         std::numeric_limits<value_type>::max()>;

    constexpr distribution_type distribution() const
    {
        return {};
    }

    constexpr value_type min() const
    {
        return 0;
    }
    constexpr value_type max() const
    {
        return std::numeric_limits<value_type>::max();
    }

    value_type operator()()
    {
        const uint32_t result = rotl(s[0] + s[3], 7) + s[0];

        const uint32_t t = s[1] << 9;

        s[2] ^= s[0];
        s[3] ^= s[1];
        s[1] ^= s[2];
        s[0] ^= s[3];

        s[2] ^= t;

        s[3] = rotl(s[3], 11);

        return result;
    }

    int bits() const
    {
        return 32;
    }

  private:
    uint32_t rotl(const uint32_t x, int k)
    {
        return (x << k) | (x >> (32 - k));
    }

    uint32_t s[4];
};

template <binary_entropy_generator Source> class huber_vargas
{
  public:
    huber_vargas(Source source) : m_source(source)
    {
    }

    template <std::integral T> auto operator()(const uniform_distribution<T> &dist)
    {
        // See Huber & Vargas (2024), "Optimal rolling of fair dice using fair coins"
        T m = 1, X = 1;
        T n = dist.size();
        while (m < n)
        {
            auto B = m_source();
            X = X + B * m;
            m = m << 1;
            if (m >= n && X <= n)
                m = n;
            if (m >= n && X > n)
            {
                X = X - n;
                m = m - n;
            }
        }

        return X + dist.min() -1;
    }

    const auto & source() const { return m_source; }

  private:
    Source m_source;
};

template<binary_entropy_generator Source>
auto bits_fetched(const huber_vargas<Source> & src)
{
    return bits_fetched(src.source());
}

} // namespace entropy_store
