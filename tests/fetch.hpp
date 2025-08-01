#pragma once
#include "entropy_store.hpp"

// For algorithms that have a fixed "fetch" or "flip" backend

class fetch_base
{
public:
    virtual int fetch() = 0;

    void install();
};

template<typename Source>
class fetch_source : public fetch_base
{
  public:
    using distribution_type = entropy_store::binary_distribution;

    fetch_source(Source src) : m_source(std::move(src))
    {
        install();
    }

    fetch_source(fetch_source&&src) : m_source(src.source())
    {
        install();
    }

    distribution_type distribution() const
    {
        return {};
    }
    std::size_t bits_fetched() const;

    const Source &source() const
    {
        return m_source;
    }

    int operator()()
    {
        return m_source();
    }

    int fetch() override
    {
        return m_source();
    }

    Source m_source;
};

namespace entropy_store
{
    template<typename Source>
double internal_entropy(const fetch_source<Source> &source)
{
    return internal_entropy(source.source());
}

    template<typename Source>
double bits_fetched(const fetch_source<Source> &source)
{
    return bits_fetched(source.source());
}
} // namespace entropy_store
