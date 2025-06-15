
#include <iostream>

#include "entropy_store.hpp"

#include <cassert>
#include <functional>

using namespace entropy_store;

// For exposition



// Binary entropy stream

template <typename Generator>
class binary_entropy_stream
{
public:
    using value_type = std::uint32_t;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type *;
    using reference = value_type &;
    using iterator_category = std::input_iterator_tag;
    using generator_type = Generator;
    using distribution_type = uniform_distribution<uint32_t>;
    distribution_type dist = binary();

    distribution_type distribution() const { return dist; }

    binary_entropy_stream() : m_value(m_random()), m_shift(0) {}
    binary_entropy_stream(binary_entropy_stream &&other)
    {
        m_value = other.m_value;
        m_shift = other.m_shift;
    }

    binary_entropy_stream &operator=(binary_entropy_stream &&other) { return *this; }

    value_type operator*() const { return (m_value >> m_shift) & 1; }

    binary_entropy_stream &operator++()
    {
        if (++m_shift >= 32)
        {
            m_value = m_random();
            m_shift = 0;
        }
        return *this;
    }

    binary_entropy_stream &operator++(int)
    {
        return ++*this;
    }

    bool operator==(const binary_entropy_stream &) const { return true; }
    bool operator!=(const binary_entropy_stream &) const { return false; }

private:
    std::random_device m_random;

    // We should really handle arbitrary sizes,
    // but for now, assume 32-bit (and check it!)
    static_assert(generator_type::min() == 0);
    static_assert(generator_type::max() == 0xffffffff);
    value_type m_value, m_shift;
};

// For exposition only
struct entropy_store_test
{
    using uint32_t = std::uint32_t;
    binary_entropy_stream<std::random_device> input;
    uint32_t bits_fetched = 0;

    uint32_t fetch()
    {
        ++bits_fetched;
        return *input++;
    }

    const uint32_t N = 1 << 31;
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
        return ::generate_uniform(U_s, s, N, n, [&](uint32_t &, uint32_t &)
                                  { fetch_entropy(); });
    }

    void combine(uint32_t U_n, uint32_t n)
    {
        U_s = U_s * n + U_n;
        s = s * n;
    }

    uint32_t generate_distribution(uint32_t n, const uint32_t *weights, const uint32_t *outputs, const uint32_t *offsets)
    {
        uint32_t U_n = generate_uniform(n);
        uint32_t i = outputs[U_n];
        combine(U_n - offsets[i], weights[i]);
        return i;
    }

    void combine_distribution(uint32_t i, uint32_t n, const uint32_t *weights, const uint32_t *offsets)
    {
        combine(offsets[i] + generate_uniform(weights[i]), weights[i]);
    }

    uint32_t generate_distribution(const weighted_distribution &distribution)
    {
        return generate_distribution(
            distribution.outputs.size(),
            distribution.weights.data(),
            distribution.outputs.data(),
            distribution.offsets.data());
    }

    void combine_distribution(const weighted_distribution &distribution, uint32_t i)
    {
        combine_distribution(i,
                             distribution.outputs.size(),
                             distribution.weights.data(),
                             distribution.offsets.data());
    }

    // !! Wrong!!
    uint32_t convert_distribution(const weighted_distribution &dist_input, const weighted_distribution &dist_output, uint32_t i)
    {
        // !! While s < N
        combine_distribution(dist_input, i);
        return generate_distribution(dist_output);
    }
};


int main()
{
    weighted_distribution d = {49, 1};
    entropy_store_test s;
    for (int i = 0; i < 100; i++)
        std::cout << s.generate_uniform(6);
    std::cout << std::endl;
    s.generate_uniform(1); // Pre-cache entropy
    s.bits_fetched = 0;
    for (int i = 0; i < 100; i++)
        std::cout << s.generate_distribution(d);
    std::cout << std::endl;
    std::cout << "Bits fetched = " << s.bits_fetched << std::endl;

    // bs generates bits from the random device
    int bits_fetched = 0;
    auto bits = counted_bit_generator{bits_fetched};
    std::cout << bits() << std::endl;

    auto c1 = entropy_buffer(bits);
    auto u = uniform_distribution(3, 9);
    c1(u);
    bits_fetched = 0;
    for (int i = 0; i < 100; i++)
        std::cout << c1(u);
    std::cout << "\nFetched " << bits_fetched << " bits\n";

    // Convert
    auto s1 = entropy_converter{bits, uniform_distribution{'a', 'z'}};
    s1();
    bits_fetched = 0;
    for (int i = 0; i < 100; i++)
        std::cout << s1();
    std::cout << "\nFetched " << bits_fetched << " bits\n";

    auto s2 = entropy_converter{bits, weighted_distribution{1, 20}};
    s2();
    bits_fetched = 0;
    for (int i = 0; i < 100; i++)
        std::cout << s2();
    std::cout << "\nFetched " << bits_fetched << " bits\n";

    // Convert s2 back into binary
    // !! Bug: should be able to use any entropy source here (including a
    auto w1 = entropy_converter{bits, weighted_distribution{1, 20}};
    auto s4 = entropy_converter{w1, weighted_distribution{2, 1}};
    s4();
    s4();
    bits_fetched = 0;
    for (int i = 0; i < 100; ++i)
        std::cout << s4();
    std::cout << "\nFetched " << bits_fetched << " bits\n";
}