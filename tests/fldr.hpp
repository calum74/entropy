#pragma once

#include "entropy_metrics.hpp"
#include "entropy_store.hpp"
#include "fetch.hpp"
#include "fwd.hpp"

extern "C"
{
#include "fldr.h"
}

class fldr_source
{
  public:
    using distribution_type = entropy_store::weighted_distribution;
    using value_type = int;
    using source_type = fetch_source;

    fldr_source(const distribution_type &dist)
        : m_distribution(dist), m_fldr {fldr_preprocess((int *)dist.weights().data(), dist.weights().size()), fldr_free}
    {
    }

    auto operator()(...)
    {
        return fldr_sample(m_fldr.get());
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
    std::unique_ptr<fldr_preprocess_t, void(*)(fldr_preprocess_t*)> m_fldr;
    fetch_source m_source;
};

namespace entropy_store
{
inline double internal_entropy(const fldr_source &)
{
    return 0;
}

int bits_fetched(const fldr_source &s)
{
    return bits_fetched(s.source());
}
} // namespace entropy_store
