#include "entropy_metrics.hpp"
#include "entropy_store.hpp"

using namespace entropy_store;

template <entropy_generator Source> void count_totals(Source src, int count, double min = 0.99, double max = 1.01)
{
    auto source = check_distribution{src};

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
    auto bits = counted_bit_generator{};
    auto uniform_input = entropy_converter{bits, uniform_distribution{1, 6}};

    count_totals(bits, N);
    count_totals(entropy_converter{bits, uniform_distribution{0, 1}}, N);
    count_totals(entropy_converter{bits, weighted_distribution{1, 1}}, N);
    count_totals(entropy_converter{bits, bernoulli_distribution{1, 2}}, N);
    count_totals(entropy_converter{bits, uniform_distribution{1, 6}}, N);
    count_totals(entropy_converter{bits, weighted_distribution{1, 2}}, N, 0.96, 1.04);
    count_totals(entropy_converter{bits, bernoulli_distribution{1, 3}}, N, 0.96, 1.04);
    count_totals(entropy_converter{bits, weighted_distribution{49, 2}}, N, 0.7, 1.5);
    count_totals(entropy_converter{bits, weighted_distribution{1, 999}}, N, 0.25, 8.0);
    count_totals(entropy_converter{bits, weighted_distribution{1, 2, 3, 4}}, N, 0.97, 1.03);
    count_totals(entropy_converter{uniform_input, weighted_distribution{1, 1}}, N);
    count_totals(entropy_converter{bits, weighted_distribution{4, 1, 5}}, N, 0.96, 1.05);

    std::cout << "\nAll tests passed!\n";
}