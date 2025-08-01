#pragma once

#include "entropy_metrics.hpp"
#include "entropy_store.hpp"
#include "fetch.hpp"
#include "fwd.hpp"

extern "C"
{
#include "aldr.h"
}

template<typename Source>
class aldr_source
{
  public:
    using distribution_type = entropy_store::weighted_distribution;
    using value_type = int;
    using source_type = fetch_source<Source>;

    aldr_source(Source src, const distribution_type &dist) : m_source(src), m_distribution(dist), m_impl(std::make_shared<impl>(dist))
    {
    }

    auto operator()(...)
    {
        return aldr_sample(&m_impl->m_aldr);
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
    struct impl
    {
        impl(const distribution_type &dist)
        {
            m_aldr = aldr_preprocess((uint32_t *)dist.weights().data(), dist.weights().size());
        }
        ~impl()
        {
            aldr_free(m_aldr);
        }
        aldr_s m_aldr;
    };

    // Deal with copying etc
    std::shared_ptr<impl> m_impl;

    const distribution_type m_distribution;
    source_type m_source;
};

namespace entropy_store
{
template<typename S> 
    inline double internal_entropy(const aldr_source<S> &)
{
    return 0;
}

template<typename S> int bits_fetched(const aldr_source<S> &s)
{
    return bits_fetched(s.source());
}
} // namespace entropy_store
