#pragma once

#include "entropy_store.hpp"

namespace entropy_store
{
template <std::integral T> double P(const uniform_distribution<T> &dist, int i)
{
    if (i >= dist.min() && i <= dist.max())
        return 1.0 / dist.size();
    return 0;
}

template <std::integral T> double entropy(const uniform_distribution<T> &dist)
{
    return std::log2(dist.size());
}

inline double P(const weighted_distribution &dist, int i)
{
    return double(dist.weights()[i]) / dist.outputs().size();
}

inline double entropy(const weighted_distribution &dist)
{
    double h = 0;
    for (double w : dist.weights())
    {
        auto p = w / dist.outputs().size();
        h -= p * std::log2(p);
    }

    return h;
}

inline double P(const bernoulli_distribution &dist, int i)
{
    double p = double(dist.numerator()) / double(dist.denominator());
    return i ? p : (1.0 - p);
}

inline double entropy(const bernoulli_distribution &dist)
{
    double p = double(dist.numerator()) / double(dist.denominator());
    return -p * std::log2(p) - (1 - p) * std::log2(1 - p);
}

template <entropy_generator Source> struct counter
{
    using source_type = Source;
    using value_type = typename source_type::value_type;
    using distribution_type = typename source_type::distribution_type;

    counter(const Source &source = {}) : m_source(source), m_count(0)
    {
    }

    counter(const counter&src) : m_source(src.source()), m_count(0) {}

    counter(counter &&src) : m_source(std::move(src)), m_count(src.m_count)
    {
    }

    auto distribution() const
    {
        return m_source.distribution();
    }

    auto operator()()
    {
        ++m_count;
        return m_source();
    }

    template <typename Distribution> auto operator()(const Distribution &dist)
    {
        ++m_count;
        return m_source(dist);
    }

    auto fetch_bit()
    {
        ++m_count;
        return m_source.fetch_bit();
    }

    double internal_entropy() const
    {
        return 0;
    }

    const source_type &source() const
    {
        return m_source;
    }

    std::size_t m_count;
    Source m_source;
};

using counted_bit_generator = counter<random_bit_generator>;

template <typename Source> std::size_t bits_fetched(const counter<Source> &source)
{
    return source.m_count;
}

template <entropy_generator Source, std::integral Buffer = std::uint32_t>
double internal_entropy(const entropy_store<Source, Buffer> &es)
{
    return std::log2(es.size()) + internal_entropy(es.source());
}

template <entropy_generator Source, distribution Distribution, std::integral Buffer>
double internal_entropy(const entropy_converter<Source, Distribution, Buffer> &ec)
{
    return internal_entropy(ec.source());
}

template <typename Source, typename Distribution, typename Buffer>
std::size_t bits_fetched(const entropy_converter<Source, Distribution, Buffer> &source)
{
    return bits_fetched(source.source());
}

template <typename Source, typename Buffer> std::size_t bits_fetched(const entropy_store<Source, Buffer> &source)
{
    return bits_fetched(source.source());
}

template <entropy_generator Source> double internal_entropy(const counter<Source> &c)
{
    return internal_entropy(c.source());
}

template <typename Source> inline std::size_t internal_entropy(const bit_generator<Source> &)
{
    return 0;
}

} // namespace entropy_store
