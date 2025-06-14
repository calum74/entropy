#include <iostream>
#include <vector>
#include <random>
#include <cassert>
#include <functional>

// Technically we could use std::uniform_int_distribution here
template<typename T>
struct uniform_distribution
{
    using value_type = T; 
    // Inclusive values, min<=max
    int min, max;
};

uniform_distribution<std::uint32_t> binary() { return {0,1}; }
uniform_distribution<std::uint32_t> empty() { return {0,0}; }

struct empty_source
{
    using value_type = std::uint32_t;
    using distribution_type = uniform_distribution<value_type>;
    distribution_type get_distribution() const { return empty(); }
};

struct random_device_source
{
    using value_type = std::uint32_t;
    using distribution_type = uniform_distribution<value_type>;
    using source_type = empty_source;
    std::random_device rd;
    random_device_source() {}
    random_device_source(const random_device_source&) {}

    value_type operator()()
    {
        return rd();
    }
};

template<typename Source>
struct binary_converter
{
    using source_type = Source;
    using value_type = typename Source::value_type;
    using distribution_type = uniform_distribution<value_type>;

    value_type m_value=0, m_shift=32;
    Source m_source;

    binary_converter(const source_type &source = {}) : m_source(source) {}

    value_type operator()()
    {
        if(++m_shift>=32)
        {
            m_value = m_source();
            m_shift = 0;
        }
        return (m_value>>m_shift)&1;
    }
};

template<typename Source>
binary_converter<Source> make_binary_converter(const Source &s = random_device_source{})
{
    return {s};
}

// For exposition

template<std::integral uint32_t>
void combine(uint32_t U_n, uint32_t n, uint32_t U_m, uint32_t m, uint32_t& U_nm, uint32_t &nm)
{
    U_nm = m * U_n + U_m;
    nm = m*n;
}

template<std::integral uint32_t>
void divide(uint32_t U_nm, uint32_t nm, uint32_t m, uint32_t &U_n, uint32_t &U_m, uint32_t &n)
{
    U_m = U_nm % m;
    U_n = U_nm / m;
    n = nm / m;
}

template<std::integral uint32_t, std::invocable<uint32_t&, uint32_t&> Fn>
uint32_t generate_uniform(uint32_t &U_s, uint32_t &s, uint32_t N, uint32_t n, Fn fetch_entropy)
{
    for(;;)
    {
        while (s < N)
            fetch_entropy(U_s, s);
        // Resample entropy s to a multiple of m
        uint32_t r = s / n;
        uint32_t c = s % n;
        if(s >= c)  [[likely]]
        {
            // Resample successful
            U_s -= c;
            auto U_n = U_s % n;
            U_s = U_s / n;
            s = r;
            return U_n;
        }
        else
        {
            // Resample unsuccessful
            s = c;
        }
     }
}





template<typename Source, typename Distribution>
struct entropy_converter
{
    using value_type = typename Distribution::value_type;
    using distribution_type = Distribution;
    using source_type = Source;

    entropy_converter(const Source &src, const Distribution & dist) : source(src), dist(dist)
    {
    }

    value_type operator()()
    {
        return generate(U_s, s, N, dist, source);
    }

private:
    value_type N=1<<31, U_s=0, s=1;
    distribution_type dist;
    source_type source;
};

template<typename Source, typename Distribution>
entropy_converter<Source, Distribution> make_entropy_converter(const Source &src, const Distribution & dist)
{
    return {src, dist};
}


// Binary entropy stream

template<typename Generator>
class binary_entropy_stream
{
public:
    using value_type = std::uint32_t;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type&;
    using iterator_category = std::input_iterator_tag;
    using generator_type = Generator;
    using distribution_type = uniform_distribution<uint32_t>;
    distribution_type dist = binary();

    distribution_type distribution() const { return dist; }

    binary_entropy_stream() : m_value(m_random()), m_shift(0) {}
    binary_entropy_stream(binary_entropy_stream&&other)
    {
        m_value = other.m_value;
        m_shift = other.m_shift;
    }

    binary_entropy_stream& operator=(binary_entropy_stream&&other) { return *this; }

    value_type operator*() const { return (m_value>>m_shift)&1; }

    binary_entropy_stream & operator++() {
        if(++m_shift>=32)
        {
            m_value = m_random();
            m_shift = 0;
        }
        return *this;
    }

    binary_entropy_stream & operator++(int)
    {
        return ++*this;
    }

    bool operator==(const binary_entropy_stream&) const { return true; }
    bool operator!=(const binary_entropy_stream&) const { return false; }

private:
    std::random_device m_random;

    // We should really handle arbitrary sizes,
    // but for now, assume 32-bit (and check it!)
    static_assert(generator_type::min()==0);
    static_assert(generator_type::max()==0xffffffff);
    value_type m_value, m_shift;
};


// Constructs the lookup tables for a weighted distribution
class weighted_distribution
{
public:
    using uint32_t = std::uint32_t;
    weighted_distribution(std::initializer_list<uint32_t> weights) : weighted_distribution(std::vector(weights)) {}
    weighted_distribution(std::vector<uint32_t> w) : weights(std::move(w))
    {
        for(int i=0; i<weights.size(); ++i)
        {
            offsets.push_back(outputs.size());
            for(int j=0; j<weights[i]; ++j)
            {
                outputs.push_back(i);
            }
        }
    }

    std::vector<std::uint32_t> weights, outputs, offsets;
};

// For exposition only
struct entropy_store
{
    using uint32_t = std::uint32_t;
    binary_entropy_stream<std::random_device> input;
    uint32_t bits_fetched = 0;

    uint32_t fetch()
    {
        ++bits_fetched;
        return *input++;
    }

    const uint32_t N = 1<<31;
    uint32_t U_s = 0;
    uint32_t s = 1;

    void fetch_entropy()
    {
        U_s <<= 1;
        U_s |= fetch();
        s <<= 1;
    }

    uint32_t generate_uniform(uint32_t n)
    {
        return ::generate_uniform(U_s, s, N, n, [&](uint32_t &, uint32_t&){ fetch_entropy(); });
    }

    void combine(uint32_t U_n, uint32_t n)
    {
        U_s = U_s * n + U_n;
        s = s * n;
    }

    uint32_t generate_distribution(uint32_t n, const uint32_t * weights, const uint32_t * outputs, const uint32_t * offsets)
    {
        uint32_t U_n = generate_uniform(n);
        uint32_t i = outputs[U_n];
        combine(U_n - offsets[i], weights[i]);
        return i;
    }

    void combine_distribution(uint32_t i, uint32_t n, const uint32_t * weights, const uint32_t * offsets)
    {
        combine(offsets[i] + generate_uniform(weights[i]), weights[i]);
    }

    uint32_t generate_distribution(const weighted_distribution & distribution)
    {
        return generate_distribution(
            distribution.outputs.size(), 
            distribution.weights.data(),
            distribution.outputs.data(),
            distribution.offsets.data());
    }

    void combine_distribution(const weighted_distribution & distribution, uint32_t i)
    {
        combine_distribution(i,
            distribution.outputs.size(), 
            distribution.weights.data(),
            distribution.offsets.data());
    }

    // !! Wrong!!
    uint32_t convert_distribution(const weighted_distribution &dist_input, const weighted_distribution &dist_output, uint32_t i)
    {
        combine_distribution(dist_input, i);
        return generate_distribution(dist_output);
    }
};


template<typename T>
int generate(const uniform_distribution<T> &, entropy_store&)
{
    return 0;
}

int generate(const weighted_distribution &, entropy_store&)
{
    return 0;
}

template<typename T>
void extract(const uniform_distribution<T> &, entropy_store&, int)
{
}

void extract(const weighted_distribution&, entropy_store&, int)
{
}


int main()
{
    weighted_distribution d = {49,1};
    entropy_store s;
    for(int i=0; i<100; i++)
        std::cout << s.generate_uniform(6);
    std::cout << std::endl;
    s.generate_uniform(1);  // Pre-cache entropy
    s.bits_fetched = 0;
    for(int i=0; i<100; i++)
        std::cout << s.generate_distribution(d);
    std::cout << std::endl;
    std::cout << "Bits fetched = " << s.bits_fetched << std::endl;

    auto bs = make_binary_converter(random_device_source());
    std::cout << bs() << std::endl;
}