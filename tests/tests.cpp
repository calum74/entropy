#include "aldr.hpp"
#include "entropy_metrics.hpp"
#include "entropy_store.hpp"
#include "fldr.hpp"
#include "fwd.hpp"
#include "testing.hpp"

using namespace entropy_store;

template <entropy_generator Source> void count_totals(Source src, int count, double min = 0.99, double max = 1.01)
{
    auto source = check_distribution{std::move(src)};

    source.read(count);
    std::cout << source;
    assert(source.check_sigma());

    double efficiency = source.efficiency();
    assert(efficiency >= min);
    assert(efficiency <= max);
}

int main(int argc, char **argv)
{
    int N = 1000;

    if (argc == 2)
        N = atoi(argv[1]);

    // This bit generator is based on std::random_device
    // For debugging purposes, we've wrapped it in a counter
    // so we can see the number of bits it's generated.
    auto rd = random_device_generator{};
    auto bits = counter{bit_generator{rd}}; // Count bits, not words
    auto uniform_input = entropy_converter{bits, uniform_distribution{1, 6}};
    auto const_uniform_input = entropy_converter{bits, const_uniform<1, 6>{}};
    auto xoshiro128_rd = counter{xoshiro128{rd}};

    // !! Test entropy_store_64

    count_totals(bits, N);
    count_totals(entropy_converter{bits, uniform_distribution{0, 1}}, N);
    count_totals(entropy_converter{bits, weighted_distribution{1, 1}}, N);
    count_totals(entropy_converter{bits, bernoulli_distribution{1, 2}}, N);
    count_totals(entropy_converter{bits, uniform_distribution{1, 6}}, N);
    count_totals(entropy_converter64{bits, uniform_distribution{1, 6}}, N);
    count_totals(entropy_converter{bits, weighted_distribution{1, 2}}, N, 0.96, 1.04);
    count_totals(entropy_converter{bits, bernoulli_distribution{1, 3}}, N, 0.96, 1.04);
    count_totals(entropy_converter{bits, weighted_distribution{49, 2}}, N, 0.7, 1.5);
    count_totals(entropy_converter{bits, weighted_distribution{1, 999}}, N, 0.25, 8.0);
    count_totals(entropy_converter{bits, weighted_distribution{1, 2, 3, 4}}, N, 0.97, 1.03);
    count_totals(entropy_converter{uniform_input, weighted_distribution{1, 1}}, N);
    count_totals(entropy_converter{const_uniform_input, weighted_distribution{1, 1}}, N);
    count_totals(entropy_converter{bits, weighted_distribution{4, 1, 5}}, N, 0.96, 1.05);
    count_totals(entropy_converter64{bits, weighted_distribution{4, 1, 5}}, N, 0.96, 1.05);

    count_totals(entropy_converter{bits, const_uniform<1, 3>{}}, N);
    count_totals(entropy_converter64{bits, const_uniform<1, 3>{}}, N);
    count_totals(entropy_converter{bits, const_bernoulli<1, 3>{}}, N, 0.96, 1.04);
    count_totals(entropy_converter64{bits, const_bernoulli<1, 3>{}}, N, 0.96, 1.04);

    std::cout << "Von Neumann: ";
    count_totals(bound_entropy_generator{von_neumann{bits}, uniform_distribution(1, 6)}, N, 0.62);
    std::cout << "Knuth-Yao: ";
    count_totals(bound_entropy_generator{fast_dice_roller{bits}, uniform_distribution(1, 6)}, N, 0.68);
    std::cout << "Lemire: ";
    count_totals(bound_entropy_generator{lemire{counter{random_device_generator{}}}, uniform_distribution(1, 6)}, N,
                 0.04);

    std::cout << "Lemire Xoshiro: ";
    count_totals(bound_entropy_generator{lemire{xoshiro128_rd}, uniform_distribution(1, 6)}, N, 0.04);

    std::cout << "Huber-Vargas Xoshiro: ";
    count_totals(bound_entropy_generator{huber_vargas{bits}, uniform_distribution(1, 6)}, N, 0.04);

    count_totals(bound_entropy_generator{c_code_source{bits}, uniform_distribution(1, 6)}, N);

    std::cout << "FLDR: ";
    count_totals(fldr_source{bits, uniform_distribution(1, 6)}, N, 0.69);
    std::cout << "ALDR: ";
    count_totals(aldr_source{bits, uniform_distribution(1, 6)}, N, 0.69);

    std::cout << "ALDR: ";
    count_totals(aldr_source{bits, weighted_distribution{1, 99}}, N, 0.69);

    std::cout << "ALDR: ";
    count_totals(aldr_source{bits, weighted_distribution{1, 2, 3, 4, 5}}, N, 0.69);

    std::cout << "\nAll tests passed!\n";
}