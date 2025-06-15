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
        int min, max;  // Inclusive values, min<=max
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
        using uint32_t = std::uint32_t;
        using value_type = std::uint32_t;
        weighted_distribution(std::initializer_list<uint32_t> weights) : weighted_distribution(std::vector(weights)) {}
        weighted_distribution(std::vector<uint32_t> w) : weights(std::move(w))
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

        std::vector<std::uint32_t> weights, outputs, offsets;
        std::uint32_t bits;
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
        source.distribution();  // !! Check types
        source();       // !! Check types
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

        int &m_count;
    };

    using counted_bit_generator = counter<random_bit_generator>;

    template <std::integral uint32_t>
    void validate(uint32_t U_n, uint32_t n)
    {
        assert(U_n < n);
    }


    template <std::integral uint32_t>
    void combine(uint32_t U_n, uint32_t n, uint32_t U_m, uint32_t m, uint32_t &U_nm, uint32_t &nm)
    {
        validate(U_n, n);
        validate(U_m, m);
        U_nm = m * U_n + U_m;
        nm = m * n;
        assert(n <= nm);
        assert(m <= nm);
        validate(U_nm, nm);
    }

    template <std::integral uint32_t>
    void divide(uint32_t U_nm, uint32_t nm, uint32_t m, uint32_t &U_n, uint32_t &U_m, uint32_t &n)
    {
        U_m = U_nm % m;
        U_n = U_nm / m;
        n = nm / m;
    }

    template <std::integral uint32_t, std::invocable<uint32_t &, uint32_t &> Fn>
    uint32_t generate_uniform(uint32_t &U_s, uint32_t &s, uint32_t N, uint32_t n, Fn fetch_entropy)
    {
        assert(N >= n);
        validate(U_s, s);
        for (;;)
        {
            while (s < N)
                fetch_entropy(U_s, s);
            validate(U_s, s);
            auto debug_U_s = U_s;
            auto debug_s = s;
            // Resample entropy s to a multiple of m
            uint32_t r = s / n;
            uint32_t c = s % n;
            if (U_s >= c) [[likely]]
            {
                // Resample successful
                U_s -= c;
                s -= c;
                s /= n;
                auto U_n = U_s % n;
                U_s = U_s / n;
                //assert( r == (s-c)/n );
                // s = r;
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

    template <std::integral uint32_t, entropy_generator Source, std::integral U, std::integral T>
    T generate(uint32_t &U_s, uint32_t &s, uint32_t N, Source &source, const uniform_distribution<U> &source_dist, const uniform_distribution<T> &output_dist)
    {
        auto fetch_entropy = [&](uint32_t &U_s, uint32_t &s)
        {
            combine(U_s, s, uint32_t(source() - source_dist.min), uint32_t(source_dist.size()), U_s, s);
        };
        return T(generate_uniform(U_s, s, N / source_dist.size(), output_dist.size(), fetch_entropy)) + output_dist.min;
    }

    template <std::integral uint32_t, entropy_generator Source, distribution SourceDist>
    int generate(uint32_t &U_s, uint32_t &s, uint32_t N, Source &source, const SourceDist &source_dist, const weighted_distribution &output_dist)
    {
        int n = generate(U_s, s, N, source, source_dist, uniform_distribution{0u, uint32_t(output_dist.outputs.size() - 1)});
        int i = output_dist.outputs[n];
        combine(U_s, s, n - output_dist.offsets[i], output_dist.weights[i], U_s, s);
        return i;
    }

    template <std::integral uint32_t, entropy_generator Source, std::integral T>
    T generate(uint32_t &U_s, uint32_t &s, uint32_t N, Source &source, const weighted_distribution &source_dist, const uniform_distribution<T> &output_dist)
    {
        N = 1<<(8 * sizeof(uint32_t) - source_dist.bits);

        // If fetch_entropy fails, we'll need to fall back to fetch_binary:

        auto fetch_binary = [&](uint32_t &U_s, uint32_t &s)
        {
            s <<= 1;
            auto b = source.fetch_bit();
            assert(b==0 || b==1);
            U_s = (U_s << 1) | b;
        };

        auto fetch_entropy = [&](uint32_t &U_s, uint32_t &s)
        {
            validate(U_s, s);
            auto i = source();
            uint32_t n = source_dist.outputs.size();
            uint32_t U_n = source_dist.offsets[i] + generate_uniform(U_s, s, N>>source_dist.bits, source_dist.weights[i], fetch_binary);
            combine(U_s, s, U_n, n, U_s, s);
        };

        return generate_uniform(U_s, s, N, output_dist.size(), fetch_entropy) + output_dist.min;
    }

    template <entropy_generator Source>
    struct entropy_buffer
    {
        using value_type = std::uint32_t;
        using source_type = Source;

        entropy_buffer(const Source &src) : source(src)
        {
        }

        auto operator()(const distribution auto&dist)
        {
            return generate(U_s, s, N, source, source.distribution(), dist);
        }

        int fetch_bit() { return source.fetch_bit(); }

    private:
        value_type N = 1 << 31, U_s = 0, s = 1;
        source_type source;
    };

    template <entropy_generator Source, distribution Distribution>
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

        entropy_buffer<source_type> m_source;
        distribution_type m_distribution;
    };

}
