#pragma once
#include <cstdint>
#include <vector>
#include <cmath>
#include <random>
#include <span>

// Debugging only
#include <cassert>
#include <iostream>

namespace entropy_store
{
    template <std::integral T>
    class uniform_distribution
    {
    public:
        using value_type = T;
        using size_type = std::size_t;

        uniform_distribution(value_type a, value_type b) : m_min(a), m_max(b), m_bits(std::ceil(std::log2(size()))) {}

        size_type size() const { return max() - min() + 1; }
        size_type bits() const { return m_bits; }
        value_type min() const { return m_min; }
        value_type max() const { return m_max; }

    private:
        int m_min, m_max;       // Inclusive values, min<=max
        std::uint32_t m_bits;   // Number of bits capacity required to fetch this
    };

    uniform_distribution<std::uint32_t> binary() { return {0, 1}; }
    uniform_distribution<std::uint32_t> empty() { return {0, 0}; }

    // Constructs the lookup tables for a weighted distribution
    class weighted_distribution
    {
    public:
        using value_type = std::uint32_t;
        using size_type = std::size_t;

        weighted_distribution(std::initializer_list<value_type> weights) : weighted_distribution(std::vector(weights)) {}
        weighted_distribution(std::vector<value_type> w) : m_weights(std::move(w))
        {
            for (int i = 0; i < m_weights.size(); ++i)
            {
                m_offsets.push_back(m_outputs.size());
                for (int j = 0; j < m_weights[i]; ++j)
                {
                    m_outputs.push_back(i);
                }
            }
            m_bits = std::ceil(std::log2(m_outputs.size()));
        }
        size_type bits() const { return m_bits; }

        std::span<const value_type> weights() const { return m_weights; }
        std::span<const value_type> outputs() const { return m_outputs; }
        std::span<const value_type> offsets() const { return m_offsets; }

    private:
        std::uint32_t m_bits;
        std::vector<value_type> m_weights, m_outputs, m_offsets;
    };

    class bernoulli_distribution
    {
    public:
        using size_type = std::size_t;
        using value_type = std::uint32_t;

        bernoulli_distribution(size_type numerator, size_type denominator) :
            m_numerator(numerator), m_denominator(denominator)
        {
            m_bits = std::ceil(std::log2(denominator));
        }

        size_type bits() const { return m_bits; }
        size_type numerator() const { return m_numerator; }
        size_type denominator() const { return m_denominator; }

    private:
        size_type m_bits;
        size_type m_numerator, m_denominator;
    };

    template <typename Distribution>
    concept distribution = requires(Distribution dist) {
        typename Distribution::value_type;
        dist.bits();
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

        value_type fetch_bit()
        {
            if (++m_shift >= 32)
            {
                m_value = m_source();
                m_shift = 0;
            }
            return (m_value >> m_shift) & 1;
        }

        double internal_entropy() const { return 32 - m_shift; }
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
    uint_t generate_multiple(uint_t &U_s, uint_t &s, uint_t N, uint_t n, Fn fetch_entropy)
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
            uint_t k = s / n;
            uint_t r = s % n;
            s -= r;
            if (U_s < s) [[likely]]
            {
                // Resample successful
                validate(U_s, s);
                return k;
            }
            else
            {
                // Resample unsuccessful
                U_s -= s;
            }
        }
    }

    template <std::integral uint_t, std::invocable<uint_t &, uint_t &> Fn>
    uint_t generate_uniform(uint_t &U_s, uint_t &s, uint_t N, uint_t n, Fn fetch_entropy)
    {
        uint_t k = generate_multiple(U_s, s, N, n, fetch_entropy);
        uint_t U_n;
        divide(U_s, s, n, U_s, U_n, s);
        validate(U_s, s);
        validate(U_n, n);
        return U_n;
    }

    auto fetch_bit_from_source(entropy_generator auto &source)
    {
        return [&](auto &U_s, auto &s)
        {
            // Put this in here to check we aren't fetching binary entropy more than necessary
            std::cout << "b";

            // If fetch_entropy fails, we'll need to fall back to fetch_binary.
            // We want to avoid getting here by except during the bootstrap.
            assert(s < (1ull << (8 * sizeof(s) - 1)));
            s <<= 1;
            auto b = source.fetch_bit();
            assert(b == 0 || b == 1);
            U_s = (U_s << 1) | b;
            validate(U_s, s);
        };
    }

    template<std::integral uint_t>
    auto fetch_from_source(entropy_generator auto &source, const bernoulli_distribution &source_dist, uint_t N)
    {
        N >>= source_dist.bits();
        return [&, N](uint_t &U_s, uint_t &s)
        {
            auto b = source();
            uint_t n = source_dist.denominator();
            uint_t k;

            if(b)
            {
                k = generate_multiple(U_s, s, N, uint_t(source_dist.numerator()), fetch_bit_from_source(source));
            }
            else
            {
                k = generate_multiple(U_s, s, N, uint_t(source_dist.denominator() - source_dist.numerator()), fetch_bit_from_source(source));
                U_s += k * source_dist.numerator();
            }
            s = k * source_dist.denominator();
        };
    }

    template<std::integral uint_t, std::integral uint2>
    auto fetch_from_source(entropy_generator auto &source, const uniform_distribution<uint2> &source_dist, uint_t N)
    {
        N >>= source_dist.bits();
        return [&, N](uint_t &U_s, uint_t &s)
        {
            combine(U_s, s, uint_t(source() - source_dist.min()), uint_t(source_dist.size()), U_s, s);
        };
    }

    template<std::integral uint_t>
    auto fetch_from_source(entropy_generator auto &source, const weighted_distribution &source_dist, uint_t N)
    {
        N >>= source_dist.bits();
        return [&,N](uint_t &U_s, uint_t &s)
        {
            auto W = source();
            auto k = generate_multiple(U_s, s, N, uint_t(source_dist.weights()[W]), fetch_bit_from_source(source));
            U_s += k * source_dist.offsets()[W];
            s = k * source_dist.outputs().size();
        };
    }

    template <std::integral uint_t, entropy_generator Source, std::integral T>
    T generate(uint_t &U_s, uint_t &s, uint_t N, Source &source, const distribution auto &source_dist, const uniform_distribution<T> &output_dist)
    {
        N>>=source_dist.bits();
        auto fetch_entropy = fetch_from_source(source, source_dist, N);
        return T(generate_uniform(U_s, s, N, uint_t(output_dist.size()), fetch_entropy)) + output_dist.min();
    }

    template <std::integral uint_t>
    uint_t generate(uint_t &U_s, uint_t &s, uint_t N, entropy_generator auto &source, const distribution auto &source_dist, const bernoulli_distribution &output_dist)
    {
        N>>=source_dist.bits();
        uint_t k = generate_multiple(U_s, s, N, (uint_t)output_dist.denominator(), fetch_from_source(source, source_dist, N));
        uint32_t M = k * output_dist.numerator();
        if(U_s < M)
        {
            s = M;
            validate(U_s, s);
            return 1;
        }
        else
        {
            U_s -= M;
            s = s - M;
            validate(U_s, s);
            return 0;
        }
    }

    template <std::integral uint_t, entropy_generator Source, distribution SourceDist>
    uint_t generate(uint_t &U_s, uint_t &s, uint_t N, Source &source, const SourceDist &source_dist, const weighted_distribution &output_dist)
    {
        uint_t n = generate(U_s, s, N, source, source_dist, uniform_distribution{uint_t(0), uint_t(output_dist.outputs().size() - 1)});
        uint_t i = output_dist.outputs()[n];
        combine(U_s, s, n - output_dist.offsets()[i], uint_t(output_dist.weights()[i]), U_s, s);
        return i;
    }


    template <entropy_generator Source, std::integral Buffer = std::uint32_t>
    struct entropy_store
    {
        using value_type = Buffer;
        using source_type = Source;

        entropy_store(const Source &src) : source(src)
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

        entropy_store<source_type, Buffer> m_source;
        distribution_type m_distribution;
    };

    template <entropy_generator Source, distribution Distribution>
    using entropy_converter64 = entropy_converter<Source, Distribution, std::uint64_t>;

    template <entropy_generator Source>
    using entropy_store32 = entropy_store<Source, std::uint32_t>;

    template <entropy_generator Source>
    using entropy_store64 = entropy_store<Source, std::uint64_t>;

    template <entropy_generator Source, std::integral Buffer, std::random_access_iterator It>
    void shuffle(entropy_store<Source, Buffer> &store, It a, It b)
    {
        auto size = std::distance(a, b);
        for(int i=1; i<size; ++i)
            std::swap(a[i], a[store(uniform_distribution{0, i})]);
    }

    template <entropy_generator Source, std::integral Buffer>
    void shuffle(entropy_store<Source, Buffer> &store, std::ranges::random_access_range auto& c)
    {
        return shuffle(store, c.begin(), c.end());
    }
}
