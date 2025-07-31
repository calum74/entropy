#pragma once
#include "entropy_store.hpp"

// For algorithms that have a fixed "fetch" or "flip" backend

class fetch_source
{
  public:
    using distribution_type = entropy_store::binary_distribution;

    distribution_type distribution() const
    {
        return {};
    }
    std::size_t bits_fetched() const;

    const fetch_source &source() const
    {
        return *this;
    }

    int operator()();
};

namespace entropy_store
{
double internal_entropy(const fetch_source &);
int bits_fetched(const fetch_source &);
} // namespace entropy_store
