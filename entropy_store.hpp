#pragma once
#include <cstdint>
#include <vector>
#include <cmath>
#include <random>
#include <cassert>

namespace entropy_store
{
    template <std::integral T>
    struct uniform_distribution
    {
        using value_type = T;
        int min, max;       // Inclusive values, min<=max
        std::uint32_t bits; // Number of bits capacity required to represent this

        uniform_distribution(T a, T b) : min(a), max(b), bits(std::ceil(std::log2(size()))) {}

        uint32_t size() const { return max - min + 1; }
    };

    uniform_distribution<std::uint32_t> binary() { return {0, 1}; }
    uniform_distribution<std::uint32_t> empty() { return {0, 0}; }

    // Constructs the lookup tables for a weighted distribution
    class weighted_distribution
    {
    public:
        using uint_t = std::uint32_t;
        using value_type = std::uint32_t;
        weighted_distribution(std::initializer_list<uint_t> weights) : weighted_distribution(std::vector(weights)) {}
        weighted_distribution(std::vector<uint_t> w) : weights(std::move(w))
        {
            for (int i = 0; i < weights.size(); ++i)
            {
                offsets.push_back(outputs.size());
                for (int j = 0; j < weights[i]; ++j)
                {
                    outputs.push_back(i);
                }
            }
            bits = std::ceil(std::log2(outputs.size()));
        }

        std::vector<uint_t> weights, outputs, offsets;
        std::uint32_t bits;
    };

    class Bernoulli_distribution
    {
        // TODO.
    };

    template <typename Distribution>
    concept distribution = requires(Distribution dist) {
        typename Distribution::value_type;
        dist.bits;
    };

    template <typename Source>
    concept entropy_generator = requires(Source source) {
        typename Source::value_type;
        typename Source::distribution_type;
        source.distribution(); // !! Check types
        source();              // !! Check types
        source.fetch_bit();

        // !! also check iterator and * properties
    };

    class random_device_generator
    {
    public:
        using value_type = std::uint32_t;
        using distribution_type = uniform_distribution<value_type>;
        random_device_generator() {}
        random_device_generator(const random_device_generator &) {}

        value_type operator()()
        {
            return rd();
        }

        distribution_type distribution() const { return {rd.min(), rd.max()}; }

        int fetch_bit(); // Unused

    private:
        std::random_device rd;
    };

    template <entropy_generator Source>
    struct bit_generator
    {
        using source_type = Source;
        using value_type = typename Source::value_type;
        using distribution_type = uniform_distribution<value_type>;

        value_type m_value = 0, m_shift = 32;
        Source m_source;

        bit_generator(const source_type &source = {}) : m_source(source) {}

        distribution_type distribution() const { return binary(); }

        value_type operator()()
        {
            return fetch_bit();
        }

        int fetch_bit()
        {
            if (++m_shift >= 32)
            {
                m_value = m_source();
                m_shift = 0;
            }
            return (m_value >> m_shift) & 1;
        }

        double internal_entropy() const { return 32-m_shift; }
    };

    using random_bit_generator = bit_generator<random_device_generator>;

    template <entropy_generator Source>
    struct counter
    {
        using source_type = Source;
        using value_type = typename source_type::value_type;
        using distribution_type = typename source_type::distribution_type;

        counter(int &count, const Source &source = {}) : m_source(source), m_count(count) {}
        Source m_source;

        auto distribution() const { return m_source.distribution(); }

        auto operator()()
        {
            ++m_count;
            return m_source();
        }

        template <typename Distribution>
        auto operator()(const Distribution &dist)
        {
            ++m_count;
            return m_source(dist);
        }

        auto fetch_bit()
        {
            ++m_count;
            return m_source.fetch_bit();
        }

        double internal_entropy() const { return 0; }

        int &m_count;
    };

    using counted_bit_generator = counter<random_bit_generator>;

    template <std::integral uint_t>
    void validate(uint_t U_n, uint_t n)
    {
        assert(U_n < n);
    }

    template <std::integral uint_t>
    void combine(uint_t U_n, uint_t n, uint_t U_m, uint_t m, uint_t &U_nm, uint_t &nm)
    {
        validate(U_n, n);
        validate(U_m, m);
        U_nm = n * U_m + U_n;
        nm = m * n;
        assert(n <= nm);
        assert(m <= nm);
        validate(U_nm, nm);
    }

    template <std::integral uint_t>
    void divide(uint_t U_nm, uint_t nm, uint_t m, uint_t &U_n, uint_t &U_m, uint_t &n)
    {
        U_m = U_nm % m;
        U_n = U_nm / m;
        n = nm / m;
    }

    template <std::integral uint_t, std::invocable<uint_t &, uint_t &> Fn>
    uint_t generate_uniform(uint_t &U_s, uint_t &s, uint_t N, uint_t n, Fn fetch_entropy)
    {
        assert(N >= n);
        validate(U_s, s);
        for (;;)
        {
            while (s < N)
                fetch_entropy(U_s, s);
            assert(s >= n);
            validate(U_s, s);
            // Resample entropy s to a multiple of m
            uint_t r = s / n;
            uint_t c = s % n;
            if (U_s >= c) [[likely]]
            {
                // Resample successful
                U_s -= c;
                uint_t U_n = U_s % n;
                U_s = U_s / n;
                s = r;
                validate(U_s, s);
                return U_n;
            }
            else
            {
                // Resample unsuccessful
                s = c;
            }
        }
    }

    template <std::integral uint_t, entropy_generator Source, std::integral U, std::integral T>
    T generate(uint_t &U_s, uint_t &s, uint_t N, Source &source, const uniform_distribution<U> &source_dist, const uniform_distribution<T> &output_dist)
    {
        auto fetch_entropy = [&](uint_t &U_s, uint_t &s)
        {
            combine(U_s, s, uint_t(source() - source_dist.min), uint_t(source_dist.size()), U_s, s);
        };
        return T(generate_uniform(U_s, s, N >> source_dist.bits, uint_t(output_dist.size()), fetch_entropy)) + output_dist.min;
    }

    template <std::integral uint_t, entropy_generator Source, distribution SourceDist>
    uint_t generate(uint_t &U_s, uint_t &s, uint_t N, Source &source, const SourceDist &source_dist, const weighted_distribution &output_dist)
    {
        uint_t n = generate(U_s, s, N, source, source_dist, uniform_distribution{uint_t(0), uint_t(output_dist.outputs.size() - 1)});
        uint_t i = output_dist.outputs[n];
        combine(U_s, s, n - output_dist.offsets[i], uint_t(output_dist.weights[i]), U_s, s);
        return i;
    }

    template <std::integral uint_t, entropy_generator Source, std::integral T>
    T generate(uint_t &U_s, uint_t &s, uint_t N, Source &source, const weighted_distribution &source_dist, const uniform_distribution<T> &output_dist)
    {
        N >>= source_dist.bits;

        auto fetch_binary = [&](uint_t &U_s, uint_t &s)
        {
            // If fetch_entropy fails, we'll need to fall back to fetch_binary.
            // We want to avoid getting here by except during the bootstrap.
            assert(s < (uint_t(1) << (8 * sizeof(uint_t) - 1)));
            s <<= 1;
            auto b = source.fetch_bit();
            assert(b == 0 || b == 1);
            U_s = (U_s << 1) | b;
            validate(U_s, s);
        };

        auto fetch_entropy = [&](uint_t &U_s, uint_t &s)
        {
            validate(U_s, s);
            auto i = source();
            uint_t n = source_dist.outputs.size();
            uint_t U_n = source_dist.offsets[i] + generate_uniform(U_s, s, uint_t(N >> source_dist.bits), uint_t(source_dist.weights[i]), fetch_binary);
            // Subtle: We need to be careful about the order of n and s
            // in the following combine, as it interacts with generate_uniform:
            combine(U_s, s, U_n, n, U_s, s);
        };

        return generate_uniform(U_s, s, N, uint_t(output_dist.size()), fetch_entropy) + output_dist.min;
    }

    template <entropy_generator Source, std::integral Buffer = std::uint32_t>
    struct entropy_buffer
    {
        using value_type = Buffer;
        using source_type = Source;

        entropy_buffer(const Source &src) : source(src)
        {
        }

        auto operator()(const distribution auto &dist)
        {
            return generate(U_s, s, N, source, source.distribution(), dist);
        }

        int fetch_bit() { return source.fetch_bit(); }

        double internal_entropy() const
        {
            return std::log2(s) + source.internal_entropy();
        }

    private:
        value_type N = value_type(1) << (sizeof(value_type) * 8 - 1), U_s = 0, s = 1;
        source_type source;
    };

    template <entropy_generator Source, distribution Distribution, std::integral Buffer = std::uint32_t>
    struct entropy_converter
    {
        using source_type = Source;
        using distribution_type = Distribution;
        using value_type = typename distribution_type::value_type;
        entropy_converter(const source_type &source, const distribution_type &distribution) : m_source(source), m_distribution(distribution)
        {
        }

        auto operator()()
        {
            return m_source(m_distribution);
        }

        auto distribution() const { return m_distribution; }

        int fetch_bit() { return m_source.fetch_bit(); }

        double internal_entropy() const { return m_source.internal_entropy(); }

        entropy_buffer<source_type, Buffer> m_source;
        distribution_type m_distribution;
    };

    template <entropy_generator Source, distribution Distribution>
    using entropy_converter64 = entropy_converter<Source, Distribution, std::uint64_t>;

    template <entropy_generator Source>
    using entropy_buffer64 = entropy_buffer<Source, std::uint64_t>;
}
