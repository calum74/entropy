
#include "entropy_store.hpp"

#include <iostream>
#include <map>

using namespace entropy_store;

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

template<entropy_store::entropy_generator Source>
void count_totals(int & bits_fetched, Source source, int count=1000)
{
    source();  // Warm up the cache
    bits_fetched = 0;
    std::map<int, int> totals;
    for(int i=0; i<count; i++)
    {
        totals[source()]++;
    }
    for(auto &[value,count] : totals)
        std::cout << "  " << value << ": " << count << std::endl;
    std::cout << "  Bits fetched: " << bits_fetched << std::endl;
}

int main()
{
    int bits_fetched = 0;
    auto bits = counted_bit_generator{bits_fetched};
    std::cout << "Unbiassed bits:\n";
    count_totals(bits_fetched, bits);

    std::cout << "Unbiassed coin as a uniform {0,1}:\n";
    count_totals(bits_fetched, entropy_converter{bits, uniform_distribution{0,1}});

    std::cout << "Unbiassed coin as a 1:1 distribution:\n";
    count_totals(bits_fetched, entropy_converter{bits, weighted_distribution{1,1}});

    std::cout << "Unbiassed D6:\n";
    count_totals(bits_fetched, entropy_converter{bits, uniform_distribution{1,6}});

    std::cout << "1:2 biassed coin:\n";
    count_totals(bits_fetched, entropy_converter{bits, weighted_distribution{1,2}});

    std::cout << "49:2 biassed coin:\n";
        count_totals(bits_fetched, entropy_converter{bits, weighted_distribution{49,2}});

    std::cout << "1:999 biassed coin:\n";
        count_totals(bits_fetched, entropy_converter{bits, weighted_distribution{1,999}});

    std::cout << "1:2:3:4 distribution:\n";
        count_totals(bits_fetched, entropy_converter{bits, weighted_distribution{1,2,3,4}});

    // Let's create a biassed input distribution
    auto uniform_input = entropy_converter{bits, uniform_distribution{1,6}};
    auto biassed_input = entropy_converter{bits, weighted_distribution{1,4}};
    auto distribution_input = entropy_converter{bits, weighted_distribution{4,3,2,1}};
    auto low_entropy_input = entropy_converter{bits, weighted_distribution{1,999}};

    std::cout << "Unbiassed coin from uniform input:\n";
    count_totals(bits_fetched, entropy_converter{uniform_input, weighted_distribution{1,1}});

    std::cout << "Unbiassed coin from 1:999 input:\n";
    count_totals(bits_fetched, entropy_converter{low_entropy_input, weighted_distribution{1,1}});

    std::cout << "Unbiassed coin from 1:4 input:\n";
    count_totals(bits_fetched, entropy_converter{biassed_input, weighted_distribution{1,1}});

    std::cout << "Unbiassed coin from 4:3:2:1 input:\n";
    count_totals(bits_fetched, entropy_converter{distribution_input, weighted_distribution{1,1}});

    std::cout << "4:1:5 distribution from 1:1 input:\n";
    count_totals(bits_fetched, entropy_converter{bits, weighted_distribution{4,1,5}}, 1000000);

    std::cout << "4:1:5 distribution from 4:3:2:1 input:\n";
    count_totals(bits_fetched, entropy_converter{distribution_input, weighted_distribution{4,1,5}}, 1000000);
}