#pragma once


#include "entropy_metrics.hpp"
#include "entropy_store.hpp"
#include "fetch.hpp"
#include "fwd.hpp"

extern "C"
{
#include "aldr.h"
}

class aldr_source
{
  public:
    using distribution_type = entropy_store::weighted_distribution;
    using value_type = int;
    using source_type = fetch_source;

    aldr_source(const distribution_type &dist)
        : m_distribution(dist), m_aldr(aldr_preprocess((uint32_t*)dist.weights().data(), dist.weights().size()))
    {
    }

    aldr_source(const aldr_source &) = delete;
    aldr_source(aldr_source &&src) : m_distribution(std::move(src.distribution())), m_aldr(src.m_aldr)
    {
        // src.m_aldr = nullptr;
    }

    aldr_source &operator=(const aldr_source &) = delete;
    aldr_source &operator=(aldr_source &&) = delete;

    ~aldr_source()
    {
        // Don't free
    }

    auto operator()(...)
    {
        return aldr_sample(&m_aldr);
    }

    const distribution_type &distribution() const
    {
        return m_distribution;
    }

    const auto &source() const
    {
        return m_source;
    }

  private:
    const distribution_type m_distribution;
    aldr_s m_aldr;
    fetch_source m_source;
};

namespace entropy_store
{
inline double internal_entropy(const aldr_source &)
{
    return 0;
}

int bits_fetched(const aldr_source &s)
{
    return bits_fetched(s.source());
}
} // namespace entropy_store
