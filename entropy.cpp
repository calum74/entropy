// Debugging !!
// #include <iostream>

#include "entropy_store.hpp"

#include <iostream>
#include <iomanip>
#include <map>

using namespace entropy_store;

template <std::integral T>
void mean_and_sd(const uniform_distribution<T> &dist, int sample, double total, double &mean, double &sd)
{
    auto w = 1.0 / dist.size();
    mean = total * w;
    sd = std::sqrt(total * w * (1.0 - w));
}

void mean_and_sd(weighted_distribution dist, int sample, double total, double &mean, double &sd)
{
    auto w = double(dist.weights[sample]) / dist.outputs.size();
    mean = total * w;
    sd = std::sqrt(total * w * (1.0 - w));
}

template <std::integral T>
void mean_and_sd(const uniform_distribution<T> &dist, int x, int y, double total, double &mean, double &sd)
{
    auto w = 1.0 / dist.size();
    w = w*w;
    mean = total * w;
    sd = std::sqrt(total * w * (1.0 - w));
}

void mean_and_sd(weighted_distribution dist, int x, int y, double total, double &mean, double &sd)
{
    auto w1 = double(dist.weights[x]) / dist.outputs.size();
    auto w2 = double(dist.weights[y]) / dist.outputs.size();
    auto w = w1*w2;
    mean = total * w;
    sd = std::sqrt(total * w * (1.0 - w));
}


double entropy(const weighted_distribution &dist)
{
    double e = 0;
    for (double w : dist.weights)
    {
        auto p = w / dist.outputs.size();
        e -= p * std::log2(p);
    }

    return e;
}

template <std::integral T>
double entropy(const uniform_distribution<T> &dist)
{
    return std::log2(dist.size());
}

template <entropy_store::entropy_generator Source>
void count_totals(int &bits_fetched, Source source, int count, double min = 0.99, double max = 1.01)
{
    source(); // Warm up the cache
    bits_fetched = 0;
    double internal_before = source.internal_entropy();

    std::array<int, 10> totals = {};
    std::array<std::array<int,10>, 10> pairs = {};

    int prev = 0;
    int first = 0;

    for (int i = 0; i < count; i++)
    {
        auto x = source();
        assert(x>=0 && x<totals.size());
        totals[x]++;
        if(i>0) pairs[prev][x]++;
        if(i==0) first = x;
        prev = x;
    }
    pairs[prev][first]++; // Just to be pedantic

    for(int value=0; value<totals.size(); ++value)
    {
        auto s = totals[value];
        if(s>0)
        {
            double mean, sd;
            mean_and_sd(source.distribution(), value, count, mean, sd);
            auto sigma = (s - mean) / sd;
            std::cout << "  " << value << ": n=" << s << " σ=" << std::setprecision(2) << sigma << std::endl;
            assert(std::abs(sigma) < 4);
        }
    }

    for(int x=0; x<totals.size(); ++x)
    {
        for(int y=0; y<totals.size(); ++y)
        {
            auto s = pairs[x][y];
            if(s>0)
            {
                double mean, sd;
                mean_and_sd(source.distribution(), x, y, count, mean, sd);
                auto sigma = (s - mean) / sd;
                std::cout << "  " << x << y << ": n=" << s << " σ=" << std::setprecision(2) << sigma << std::endl;
                assert(std::abs(sigma) < 4);
            }
        }
    }


    double de = entropy(source.distribution());
    double internal_after = source.internal_entropy();
    double net_input = bits_fetched - (internal_after - internal_before);
    double net_output = de * count;
    double efficiency = net_output / net_input;

    std::cout << std::setprecision(10);
    std::cout << "  Distribution entropy = " << entropy(source.distribution()) << std::endl;
    std::cout << "  Net output entropy = " << net_output << std::endl;
    std::cout << "  Bits fetched = " << bits_fetched << std::endl;
    std::cout << "  Internal entropy delta = " << (internal_after - internal_before) << std::endl;
    std::cout << "  Net input entropy = " << net_input << std::endl;
    std::cout << "  Net efficiency = " << efficiency << std::endl;

    assert(efficiency >= min);
    assert(efficiency <= max);
}

int main(int argc, char **argv)
{
    int N = 1000;

    if (argc == 2)
        N = atoi(argv[1]);

    int bits_fetched = 0;

    // This bit generator is based on std::random_device
    // For debugging purposes, we've wrapped it in a counter
    // so we can see the number of bits it's generated.
    auto bits = counted_bit_generator{bits_fetched};

    // To generate a fixed distribution
    auto d6 = entropy_converter{bits, uniform_distribution{1, 6}};
    std::cout << "Here is a d6 roll: " << d6() << std::endl;

    // To generate using multiple distributions, use an entropy_buffer
    auto gen = entropy_buffer{bits};
    std::cout << "Here is a d6 roll: " << gen(uniform_distribution{1, 6}) << std::endl;
    std::cout << "Here is a coin flip: " << gen(uniform_distribution{0, 1}) << std::endl;

    std::cout << "Fair coin:\n";
    count_totals(bits_fetched, bits, N);

    std::cout << "Fair coin as a uniform {0,1}:\n";
    count_totals(bits_fetched, entropy_converter{bits, uniform_distribution{0, 1}}, N);

    std::cout << "Fair coin as a 1:1 distribution:\n";
    count_totals(bits_fetched, entropy_converter{bits, weighted_distribution{1, 1}}, N);

    std::cout << "Fair d6:\n";
    count_totals(bits_fetched, entropy_converter{bits, uniform_distribution{1, 6}}, N);

    std::cout << "1:2 biassed coin:\n";
    count_totals(bits_fetched, entropy_converter{bits, weighted_distribution{1, 2}}, N, 0.96, 1.04);

    std::cout << "49:2 biassed coin:\n";
    count_totals(bits_fetched, entropy_converter{bits, weighted_distribution{49, 2}}, N, 0.7, 1.5);

    std::cout << "1:999 biassed coin:\n";
    count_totals(bits_fetched, entropy_converter{bits, weighted_distribution{1, 999}}, N, 0.25, 8.0);

    std::cout << "1:2:3:4 distribution:\n";
    count_totals(bits_fetched, entropy_converter{bits, weighted_distribution{1, 2, 3, 4}}, N, 0.97, 1.03);

    // Demonstrating converting *from* weighted distributions
    // Create some weighted distributions to use as input:
    auto uniform_input = entropy_converter{bits, uniform_distribution{1, 6}};
    auto biassed_input = entropy_converter{bits, weighted_distribution{1, 4}};
    auto distribution_input = entropy_converter{bits, weighted_distribution{4, 3, 2, 1}};
    auto low_entropy_input1 = entropy_converter{bits, weighted_distribution{1, 99}};
    auto low_entropy_input2 = entropy_converter{bits, weighted_distribution{1, 999}};
    auto low_entropy_input3 = entropy_converter64{bits, weighted_distribution{1, 999}};

    std::cout << "Fair coin from uniform input:\n";
    count_totals(bits_fetched, entropy_converter{uniform_input, weighted_distribution{1, 1}}, N);

    std::cout << "Fair coin from 1:999 input (using 64-bit buffer):\n";
    count_totals(bits_fetched, entropy_converter64{low_entropy_input3, weighted_distribution{1, 1}}, N, 0.95, 1.01);

    std::cout << "Fair coin from 1:999 input (poor efficiency due to low buffer size):\n";
    count_totals(bits_fetched, entropy_converter{low_entropy_input2, weighted_distribution{1, 1}}, N, 0.5);

    std::cout << "Fair coin from 1:99 input:\n";
    count_totals(bits_fetched, entropy_converter{low_entropy_input1, weighted_distribution{1, 1}}, N, 0.8);

    std::cout << "Fair coin from 1:4 input:\n";
    count_totals(bits_fetched, entropy_converter{biassed_input, weighted_distribution{1, 1}}, N);

    std::cout << "Fair coin from 4:3:2:1 input:\n";
    count_totals(bits_fetched, entropy_converter{distribution_input, weighted_distribution{1, 1}}, N);

    std::cout << "4:1:5 distribution from 1:1 input:\n";
    count_totals(bits_fetched, entropy_converter{bits, weighted_distribution{4, 1, 5}}, N, 0.96, 1.05);

    std::cout << "4:1:5 distribution from 4:3:2:1 input:\n";
    count_totals(bits_fetched, entropy_converter{distribution_input, weighted_distribution{4, 1, 5}}, N, 0.95, 1.04);

    std::cout << "\nAll tests passed!\n";
}