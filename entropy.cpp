// Debugging !!
#include <iostream>

#include "entropy_store.hpp"

#include <iostream>
#include <map>

using namespace entropy_store;

template<std::integral T>
void mean_and_sd(const uniform_distribution<T> &dist, int sample, double total, double &mean, double & sd)
{
    auto w = 1.0/dist.size();
    mean = total *w;
    sd = std::sqrt(total * w * (1.0-w));
}

void mean_and_sd(weighted_distribution dist, int sample, double total, double &mean, double & sd)
{
    auto w = double(dist.weights[sample]) / dist.outputs.size();
    mean = total *w;
    sd = std::sqrt(total * w * (1.0-w));
}

double entropy(const weighted_distribution &dist)
{
    double e = 0;
    for(double w : dist.weights)
    {
        auto p = w/dist.outputs.size();
        e -= p * std::log2(p);
    }

    return e;
}

template<std::integral T>
double entropy(const uniform_distribution<T> & dist)
{
    return std::log2(dist.size());
}

template<entropy_store::entropy_generator Source>
void count_totals(int & bits_fetched, Source source, double min = 0.99, double max = 1.01, int count=1000)
{
    source();  // Warm up the cache
    bits_fetched = 0;
    std::map<int, int> totals;
    for(int i=0; i<count; i++)
    {
        totals[source()]++;
    }
    for(auto &[value,s] : totals)
    {
        double mean, sd;
        mean_and_sd(source.distribution(), value, count, mean, sd);
        auto sigma = (s-mean)/sd;
        std::cout << "  " << value << ": " << s << " " << sigma << "σ" << std::endl;
        assert( std::abs(sigma)<4 );
    }

    double de = entropy(source.distribution()) ;
    double efficiency = de * count / bits_fetched;

    std::cout<< "  Distribution entropy = " << entropy(source.distribution()) << std::endl;
    std::cout << "  Bits fetched: " << bits_fetched << ", " << (bits_fetched/(double)count) << " per sample, efficiency = " << efficiency << std::endl;

    assert(efficiency >=min);
    assert(efficiency <= max);
}

int main()
{
    int bits_fetched = 0;

    // This bit generator is based on std::random_device
    // For debugging purposes, we've wrapped it in a counter
    // so we can see the number of bits it's generated.
    auto bits = counted_bit_generator{bits_fetched};

    // To generate a fixed distribution
    auto d6 = entropy_converter{bits,  uniform_distribution{1,6}};    
    std::cout << "Here is a d6 roll: " << d6() << std::endl;

    // To generate using multiple distributions, use an entropy_buffer
    auto gen = entropy_buffer {bits};
    std::cout << "Here is a d6 roll: " << gen(uniform_distribution{1,6}) << std::endl;
    std::cout << "Here is a coin flip: " << gen(uniform_distribution{0,1}) << std::endl;

    std::cout << "Fair coin:\n";
    count_totals(bits_fetched, bits);

    std::cout << "Fair coin as a uniform {0,1}:\n";
    count_totals(bits_fetched, entropy_converter{bits, uniform_distribution{0,1}});

    std::cout << "Fair coin as a 1:1 distribution:\n";
    count_totals(bits_fetched, entropy_converter{bits, weighted_distribution{1,1}});

    std::cout << "Fair d6:\n";
    count_totals(bits_fetched, entropy_converter{bits, uniform_distribution{1,6}});

    std::cout << "1:2 biassed coin:\n";
    count_totals(bits_fetched, entropy_converter{bits, weighted_distribution{1,2}}, 0.96, 1.04);

    std::cout << "49:2 biassed coin:\n";
        count_totals(bits_fetched, entropy_converter{bits, weighted_distribution{49,2}}, 0.7, 1.5);

    std::cout << "1:999 biassed coin:\n";
        count_totals(bits_fetched, entropy_converter{bits, weighted_distribution{1,999}}, 0.25, 5.8);

    std::cout << "1:2:3:4 distribution:\n";
        count_totals(bits_fetched, entropy_converter{bits, weighted_distribution{1,2,3,4}}, 0.97, 1.03);

    // Demonstrating converting *from* weighted distributions
    // Create some weighted distributions to use as input:
    auto uniform_input = entropy_converter{bits, uniform_distribution{1,6}};
    auto biassed_input = entropy_converter{bits, weighted_distribution{1,4}};
    auto distribution_input = entropy_converter{bits, weighted_distribution{4,3,2,1}};
    auto low_entropy_input1 = entropy_converter{bits, weighted_distribution{1,99}};
    auto low_entropy_input2 = entropy_converter{bits, weighted_distribution{1,999}};
    auto low_entropy_input3 = entropy_converter64{bits, weighted_distribution{1,999}};

    std::cout << "Fair coin from uniform input:\n";
    count_totals(bits_fetched, entropy_converter{uniform_input, weighted_distribution{1,1}});

    std::cout << "Fair coin from 1:999 input (poor efficiency due to low N):\n";
    count_totals(bits_fetched, entropy_converter{low_entropy_input2, weighted_distribution{1,1}}, 0.7);

    std::cout << "Fair coin from 1:999 input (using 64-bit buffer):\n";
    count_totals(bits_fetched, entropy_converter64{low_entropy_input3, weighted_distribution{1,1}}, 0.65, 1.01);

    std::cout << "Fair coin from 1:99 input:\n";
    count_totals(bits_fetched, entropy_converter{low_entropy_input1, weighted_distribution{1,1}}, 0.8);

    std::cout << "Fair coin from 1:4 input:\n";
    count_totals(bits_fetched, entropy_converter{biassed_input, weighted_distribution{1,1}});

    std::cout << "Fair coin from 4:3:2:1 input:\n";
    count_totals(bits_fetched, entropy_converter{distribution_input, weighted_distribution{1,1}});

    std::cout << "4:1:5 distribution from 1:1 input:\n";
    count_totals(bits_fetched, entropy_converter{bits, weighted_distribution{4,1,5}}, 0.96, 1.03);

    std::cout << "4:1:5 distribution from 4:3:2:1 input:\n";
    count_totals(bits_fetched, entropy_converter{distribution_input, weighted_distribution{4,1,5}}, 0.95, 1.04);
    std::cout << "\nAll tests passed!\n";
}