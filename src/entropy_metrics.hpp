#pragma once

#include "entropy_store.hpp"
#include <cassert>
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <iostream>

class fldr_source; // !! Move

namespace entropy_store
{
// !! Move this somehow
double internal_entropy(const fldr_source &);
int bits_fetched(const fldr_source &);

template <std::integral T> double P(const uniform_distribution<T> &dist, int i)
{
    if (i >= dist.min() && i <= dist.max())
        return 1.0 / dist.size();
    return 0;
}

template <std::integral T, T MIN, T MAX> double P(const const_uniform_distribution<T, MIN, MAX> &dist, int i)
{
    if (i >= dist.min() && i <= dist.max())
        return 1.0 / dist.size();
    return 0;
}

template <std::integral T> double entropy(const uniform_distribution<T> &dist)
{
    return std::log2(dist.size());
}

template <std::integral T, T Min, T Max> double entropy(const const_uniform_distribution<T, Min, Max> &dist)
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
        if(w>0)
        {
            auto p = w / dist.outputs().size();
            h -= p * std::log2(p);
        }
    }

    return h;
}

inline double P(const bernoulli_distribution &dist, int i)
{
    double p = double(dist.numerator()) / double(dist.denominator());
    return i ? p : (1.0 - p);
}

template <std::integral uint_t, uint_t M, uint_t N>
double P(const const_bernoulli_distribution<uint_t, M, N> &dist, int i)
{
    double p = double(dist.numerator()) / double(dist.denominator());
    return i ? p : (1.0 - p);
}

inline double entropy(const bernoulli_distribution &dist)
{
    double p = double(dist.numerator()) / double(dist.denominator());
    return -p * std::log2(p) - (1 - p) * std::log2(1 - p);
}

template <std::integral uint_t, uint_t M, uint_t N>
double entropy(const const_bernoulli_distribution<uint_t, M, N> &dist)
{
    double p = double(dist.numerator()) / double(dist.denominator());
    return -p * std::log2(p) - (1 - p) * std::log2(1 - p);
}

template <entropy_generator Source> struct counter
{
    using source_type = Source;
    using value_type = typename source_type::value_type;
    using distribution_type = typename source_type::distribution_type;

    counter(const Source &source = {}) : m_source(source), m_count(0), m_source_entropy(source.bits())
    {
    }

    counter(const counter &src) : m_source(src.source()), m_count(0), m_source_entropy(src.m_source_entropy)
    {
    }

    counter(counter &&src)
        : m_source(std::move(src.source())), m_count(src.m_count), m_source_entropy(src.m_source_entropy)
    {
    }

    auto distribution() const
    {
        return m_source.distribution();
    }

    auto operator()()
    {
        m_count += m_source_entropy;
        return m_source();
    }

    template <typename Distribution> auto operator()(const Distribution &dist)
    {
        m_count += m_source_entropy;
        return m_source(dist);
    }

    double internal_entropy() const
    {
        return 0;
    }

    const source_type &source() const
    {
        return m_source;
    }

    void reset()
    {
        m_count=0;
    }

    std::size_t m_count;
    std::size_t m_source_entropy;
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

void mean_and_sd(const distribution auto &dist, int sample, double total, double &mean, double &sd)
{
    auto w = P(dist, sample);
    mean = total * w;
    sd = std::sqrt(total * w * (1.0 - w));
}

void mean_and_sd(const distribution auto &dist, int x, int y, double total, double &mean, double &sd)
{
    auto w = P(dist, x) * P(dist, y);
    mean = total * w;
    sd = std::sqrt(total * w * (1.0 - w));
}

// Checks that outputs from a distribution meet statistical tests
template <entropy_generator Source> class check_distribution
{
  public:
    using size_type = std::size_t;
    using value_type = typename Source::value_type;
    using source_type = typename Source::source_type;
    using distribution_type = typename Source::distribution_type;

    check_distribution(Source source) : m_source(std::move(source))
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
    auto distribution() const
    {
        return m_source.distribution();
    }

    int fetch_bit()
    {
        return m_source.fetch_bit();
    }

    void read(size_type count)
    {
        for (auto i = 0; i < count; ++i)
            (*this)();
    }

    double output_entropy() const
    {
        return m_count * entropy(distribution());
    }

    double input_entropy() const
    {
        return bits_fetched(m_source) - internal_entropy(m_source);
    }

    double efficiency() const
    {
        return output_entropy() / input_entropy();
    }

    auto output_count() const
    {
        return m_count;
    }

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
        visit_counts([&](auto, auto, auto, auto, double sigma) {
            if (std::abs(sigma) > std::abs(max))
                max = sigma;
        });

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

    const auto &source() const
    {
        return m_source;
    }

  private:
    value_type m_previous = -1;
    Source m_source;

    std::vector<size_type> m_counts;
    std::vector<std::vector<size_type>> m_pair_counts;
    size_type m_count = 0;
};

template <typename Source> std::ostream &operator<<(std::ostream &os, const check_distribution<Source> &check)
{
    os << check.source().source().source().distribution() << " -> " << check.distribution() << ":\n";
    check.visit_counts([&](auto i, auto c, double, double, double sigma) {
        os << "    " << i << ": " << c << " = " << (100.0 * c / check.output_count()) << "% = " << std::setprecision(2)
           << std::showpos << sigma << std::noshowpos << "σ"
           << (std::abs(sigma) > 4 ? "  *** FAILED ***" : ""); //  << "\n";
    });

    check.visit_pairs([&](auto i, auto j, auto c, double, double, double sigma) {
        os << "   " << i << "," << j << ": " << c << " = " << (100.0 * c / check.output_count())
           << "% = " << std::setprecision(2) << std::showpos << sigma << std::noshowpos << "σ"
           << (std::abs(sigma) > 4 ? "  *** FAILED ***" : ""); // << "\n";
    });

    os << "\n  Distribution entropy = " << std::setprecision(4) << entropy(check.distribution()) << " bits\n";
    os << "  Bits fetched = " << bits_fetched(check) << " bits\n";
    os << "  Internal entropy = " << std::setprecision(6) << internal_entropy(check) << " bits\n";
    os << "  Output entropy = " << check.output_entropy() << " bits\n";
    os << "  Efficiency = " << std::setprecision(15) << check.efficiency() << std::endl;

    return os;
}

template <typename T> std::ostream &operator<<(std::ostream &os, const uniform_distribution<T> &u)
{
    return os << "Uniform{" << u.min() << "," << u.max() << "}";
}

template <typename T, T Min, T Max>
std::ostream &operator<<(std::ostream &os, const const_uniform_distribution<T, Min, Max> &u)
{
    return os << "Uniform{" << u.min() << "," << u.max() << "}";
}

inline std::ostream &operator<<(std::ostream &os, const bernoulli_distribution &b)
{
    return os << "Bernoulli{" << b.numerator() << "/" << b.denominator() << "}";
}

template <std::integral uint_t, uint_t M, uint_t N>
std::ostream &operator<<(std::ostream &os, const const_bernoulli_distribution<uint_t, M, N> &b)
{
    return os << "Bernoulli{" << b.numerator() << "/" << b.denominator() << "}";
}

inline std::ostream &operator<<(std::ostream &os, const weighted_distribution &w)
{
    bool first = true;
    auto comma = [&]() {
        if (first)
            first = false;
        else
            return ",";
        return "";
    };

    os << "Weighted{";
    for (auto &i : w.weights())
        os << comma() << i;
    return os << "}";
}

template <typename Source> double internal_entropy(const check_distribution<Source> &source)
{
    return internal_entropy(source.source());
}

template <typename Source> std::size_t bits_fetched(const check_distribution<Source> &source)
{
    return bits_fetched(source.source());
}

} // namespace entropy_store
