#pragma once

#include "entropy_store.hpp"

#include <cstdint>
#include <cmath>
#include <iostream>
#include <cassert>
#include <iomanip>

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

    void mean_and_sd(const distribution auto &dist, int sample, double total, double &mean, double &sd)
    {
        auto w = dist.P(sample);
        mean = total * w;
        sd = std::sqrt(total * w * (1.0 - w));
    }

    void mean_and_sd(const distribution auto &dist, int x, int y, double total, double &mean, double &sd)
    {
        auto w = dist.P(x) * dist.P(y);
        mean = total * w;
        sd = std::sqrt(total * w * (1.0 - w));
    }

    // Checks that outputs from a distribution meet statistical tests
    template <entropy_generator Source>
    class check_distribution
    {
    public:
        using size_type = std::size_t;
        using value_type = typename Source::value_type;
        using source_type = typename Source::source_type;
        using distribution_type = typename Source::distribution_type;
        // static_assert(entropy_generator<check_distribution>);

        check_distribution(const Source &source) : m_source(source)
        {
            m_counts.resize(distribution().max() + 1, 0);
            m_pair_counts.resize(distribution().max() + 1, m_counts);
        }

        auto operator()()
        {
            auto value = m_source();
            ++m_counts[value];
            if (m_previous != -1)
                ++m_pair_counts[m_previous][value];
            m_previous = value;
            ++m_count;
            return value;
        }

        // !! This is inefficient
        auto distribution() const { return m_source.distribution(); }

        int fetch_bit() { return m_source.fetch_bit(); }

        double internal_entropy() const { return m_source.internal_entropy(); }

        void visit_counts(std::invocable<size_type, value_type, double, double, double> auto fn) const
        {
            auto dist = distribution();
            for (auto i = 0; i < m_counts.size(); ++i)
            {
                auto s = m_counts[i];
                if (s > 0)
                {
                    double mean, sd;
                    mean_and_sd(dist, i, m_count, mean, sd);
                    auto sigma = (s - mean) / sd;
                    fn(i, s, mean, sd, sigma);
                }
            }
        }

        double max_sigma() const
        {
            double max = 0.0;
            visit_counts([&](auto, auto, auto, auto, double sigma)
                         {
                if(std::abs(sigma) > std::abs(max))
                    max = sigma; });

            return max;
        }

        bool check_sigma(double max = 4.0) const
        {
            return std::abs(max_sigma()) < max;
        }

        template <std::invocable<value_type, value_type, value_type, double, double, double> Fn>
        void visit_pairs(Fn fn) const
        {
            auto dist = distribution();
            for (auto i = 0; i < m_counts.size(); ++i)
            {
                for (auto j = 0; j < m_counts.size(); ++j)
                {
                    auto s = m_pair_counts[i][j];
                    if (s > 0)
                    {
                        double mean, sd;
                        mean_and_sd(dist, i, j, m_count, mean, sd);
                        auto sigma = (s - mean) / sd;
                        fn(i, j, s, mean, sd, sigma);
                    }
                }
            }
        }

        const auto &source() const { return m_source.source(); }

    private:
        value_type m_previous = -1;
        Source m_source;

        std::vector<size_type> m_counts;
        std::vector<std::vector<size_type>> m_pair_counts;
        size_type m_count = 0;
    };

    template <typename Source>
    std::ostream &operator<<(std::ostream &os, const check_distribution<Source> &check)
    {
        check.visit_counts([&](auto i, auto c, double, double, double sigma)
                           { os << "    " << i << ": " << c << " = " << std::setprecision(2) << std::showpos << sigma << std::noshowpos << "σ" << (std::abs(sigma) > 4 ? "  *** FAILED ***" : "") << "\n"; });

        check.visit_pairs([&](auto i, auto j, auto c, double, double, double sigma)
                          { os << "   " << i << j << ": " << c << " = " << std::setprecision(2) << std::showpos << sigma << std::noshowpos << "σ" << (std::abs(sigma) > 4 ? "  *** FAILED ***" : "") << "\n"; });

        return os;
    }
}
