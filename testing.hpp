#pragma once

#include "entropy_store.hpp"

#include <cstdint>
#include <cmath>
#include <iostream>
#include <cassert>

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

        debug_uniform(const char *place) : place(place) {}

        ~debug_uniform()
        {
            std::cout << place << ": uniform distribution bias = " << get_percentage_bias() << "%\n";
        }

    private:
        const char *place;
        std::uint64_t total_value = 0, total_size = 0, total_count = 0;
        double sum_s_squared = 0;
    };

    template <std::integral T>
    double p(const ::entropy_store::uniform_distribution<T> &dist, int sample)
    {
        if (sample >= dist.min() && sample <= dist.max())
            return 1.0 / dist.size();
        return 0;
    }

    inline double p(weighted_distribution dist, int sample)
    {
        return double(dist.weights()[sample]) / dist.outputs().size();
    }

    inline double p(bernoulli_distribution dist, int sample)
    {
        double p = double(dist.numerator()) / double(dist.denominator());
        return sample ? p : 1 - p;
    }

    void mean_and_sd(const distribution auto &dist, int sample, double total, double &mean, double &sd)
    {
        auto w = dist.p(sample);
        mean = total * w;
        sd = std::sqrt(total * w * (1.0 - w));
    }

    void mean_and_sd(const distribution auto &dist, int x, int y, double total, double &mean, double &sd)
    {
        auto w = p(dist, x) * p(dist, y);
        mean = total * w;
        sd = std::sqrt(total * w * (1.0 - w));
    }

    inline double entropy(const bernoulli_distribution &dist)
    {
        double p = double(dist.numerator()) / double(dist.denominator());
        return -p * std::log2(p) - (1 - p) * std::log2(1 - p);
    }

    inline double entropy(const weighted_distribution &dist)
    {
        double e = 0;
        for (double w : dist.weights())
        {
            auto p = w / dist.outputs().size();
            e -= p * std::log2(p);
        }

        return e;
    }

    template <std::integral T>
    double entropy(const uniform_distribution<T> &dist)
    {
        return std::log2(dist.size());
    }

    // Checks that outputs from a distribution meet statistical tests
    template<typename Source>
    class check_distribution
    {
    public:
        check_distribution(const Source &source) : m_source(source)
        {
        }

        auto operator()()
        {
            return m_source();
        }

        auto distribution() const { return m_source.distribution(); }

        int fetch_bit() { return m_source.fetch_bit(); }

        double internal_entropy() const { return m_source.internal_entropy(); }

    private:
        Source m_source;

    };
}
