#pragma once
#include <iostream>
#include <random>

#include "fwd.hpp"
#include "fetch.hpp"

namespace entropy_store
{

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


template <typename Source, distribution Dist> class bound_entropy_generator
{
  public:
    bound_entropy_generator(Source source, const Dist &dist) : m_source(std::move(source)), m_dist(dist)
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
    return internal_entropy(g.source());
}


inline double internal_entropy(const random_device_generator &)
{
    return 0;
}

template <typename PRNG> class prng_source
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

using mt19937_source = prng_source<std::mt19937>;

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

        return X + dist.min() - 1;
    }

    const auto &source() const
    {
        return m_source;
    }

  private:
    Source m_source;
};

template <binary_entropy_generator Source> auto bits_fetched(const huber_vargas<Source> &src)
{
    return bits_fetched(src.source());
}

template <typename Source> double internal_entropy(const huber_vargas<Source> &s)
{
    return internal_entropy(s.source());
}

inline double internal_entropy(const xoshiro128 &s)
{
    return 0;
}

extern "C" uint32_t generate_uniform32(uint32_t n);

extern "C" uint32_t s_range;

template<typename Source>
class c_code_source
{
  public:
    c_code_source(Source src) : m_source(src) {}

    template <std::integral T> auto operator()(const uniform_distribution<T> &dist)
    {
        return dist.min() + generate_uniform32(dist.size());
    }

    auto distribution() const
    {
        return binary_distribution{};
    }

    const auto & source() const { return m_source; }

    fetch_source<Source> m_source;
};

template<typename S> 
inline double internal_entropy(const c_code_source<S> &s)
{
    return std::log2(s_range);
}

template<typename S> 
inline int bits_fetched(const c_code_source<S>&s)
{
    return bits_fetched(s.source());
}

template<typename Source>
struct alias_method
{
    alias_method(Source s) : m_source(s) {}

    auto operator()(const auto & dist)
    {
        return m_source(dist);
    }

    auto operator()(const weighted_distribution&dist)
    {
        return dist.outputs()[m_source(uniform_distribution(0ul,dist.outputs().size()-1))];
    }

    int operator()(const bernoulli_distribution &dist)
    {
        return m_source(uniform_distribution<size_t>(0ull,dist.denominator()-1)) < dist.numerator();
    }

    template<std::integral T, T M, T N>
    auto operator()(const_bernoulli_distribution<T,M,N> &dist)
    {
        return m_source(const_uniform<0, N-1>{}) < M;
    }

    Source m_source;

    const auto & source() const { return m_source.source(); }
};

template<typename Source>
auto internal_entropy (const alias_method<Source> & source)
{
    return internal_entropy(source.source());
}

template<typename Source>
auto bits_fetched (const alias_method<Source> & source)
{
    return bits_fetched(source.source());
}


} // namespace entropy_store
