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

} // namespace entropy_store
