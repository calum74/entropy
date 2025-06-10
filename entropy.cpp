#include <iostream>
#include <vector>
#include <random>
#include <cassert>

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
class distribution
{
public:
    using uint32_t = std::uint32_t;
    distribution(std::initializer_list<uint32_t> weights) : distribution(std::vector(weights)) {}
    distribution(std::vector<uint32_t> w) : weights(std::move(w))
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

    uint32_t generate_uniform(uint32_t n)
    {
        for(;;)
        {
            // Preload entropy one bit at a time into U_s
            while(s < N)
            {
                U_s <<= 1;
                U_s |= fetch();
                s <<= 1;
            }
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

    uint32_t generate_distribution(const distribution & distribution)
    {
        return generate_distribution(
            distribution.outputs.size(), 
            distribution.weights.data(),
            distribution.outputs.data(),
            distribution.offsets.data());
    }

    void combine_distribution(const distribution & distribution, uint32_t i)
    {
        combine_distribution(i,
            distribution.outputs.size(), 
            distribution.weights.data(),
            distribution.offsets.data());
    }

    uint32_t convert_distribution(const distribution &dist_input, const distribution &dist_output, uint32_t i)
    {
        combine_distribution(dist_input, i);
        return generate_distribution(dist_output);
    }
};


int main()
{
    distribution d = {49,1};
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
}