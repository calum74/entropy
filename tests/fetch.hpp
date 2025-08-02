#pragma once
#include "entropy_store.hpp"

#include <memory>

// For algorithms that have a fixed "fetch" or "flip" backend

namespace entropy_store
{
class fetch_base
{
  public:
    virtual int fetch() = 0;
};

void install_fetch(std::shared_ptr<fetch_base> p);

template <typename Source> class fetch_source
{
  public:
    using distribution_type = binary_distribution;

    class fetch_impl : public fetch_base
    {
      public:
        fetch_impl(Source s) : m_source(std::move(s))
        {
        }
        Source m_source;
        int fetch() override
        {
            return m_source();
        }
    };

    std::shared_ptr<fetch_impl> m_source;

    fetch_source(Source src) : m_source(std::make_shared<fetch_impl>(std::move(src)))
    {
        install_fetch(m_source);
    }

    distribution_type distribution() const
    {
        return {};
    }
    std::size_t bits_fetched() const;

    const Source &source() const
    {
        return m_source->m_source;
    }

    int operator()()
    {
        return m_source->m_source();
    }
};

template <typename Source> double internal_entropy(const fetch_source<Source> &source)
{
    return internal_entropy(source.source());
}

template <typename Source> double bits_fetched(const fetch_source<Source> &source)
{
    return bits_fetched(source.source());
}
} // namespace entropy_store
