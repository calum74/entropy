#include "entropy_store.hpp"
#include <iostream>

int main()
{
    // Create a fetch function that returns one bit at a time
    entropy_store::random_bit_generator fetch;

    // Create our entropy store
    auto es = entropy_store::entropy_store{fetch};

    // Define some distributions we can generate or consume
    entropy_store::uniform_distribution d6{1,6};
    entropy_store::bernoulli_distribution fair_coin{1,2};
    entropy_store::bernoulli_distribution biassed_coin{4,10};
    entropy_store::weighted_distribution biassed_d6{0,1,1,1,1,1,10};

    // Generate some random numbers
    std::cout << "Here is a d6 roll: " << es(d6) << std::endl;
    std::cout << "Here is a fair coin flip: " << es(fair_coin) << std::endl;
    std::cout << "Here is a biassed coin flip: " << es(biassed_coin) << std::endl;
    std::cout << "Here is a biassed d6 roll: " << es(biassed_d6) << std::endl;

    // Create a stream of d6
    auto d6s = entropy_store::entropy_converter{fetch, d6};

    // Test the stream
    std::cout << "Here are 20 dice rolls: ";
    for(int i=0; i<20; ++i) std::cout << d6s();
    std::cout << std::endl;

    // Extract entropy from the d6 stream
    auto es2 = entropy_store::entropy_store{d6s};
    
    // Flip a coin from the d6 stream
    std::cout << "Coin flip: " << es2(fair_coin) << std::endl;
}
