
#include "entropy_store.hpp"

#include <iostream>
#include <map>

using namespace entropy_store;

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

    // Demonstrating converting *from* weighted distributions
    // Create some weighted distributions to use as input:
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