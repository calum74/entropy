#include "entropy_store.hpp"
#include <iostream>

int main()
{
    // Create a fetch function that returns one bit at a time
    entropy_store::random_bit_generator fetch;

    // Create our entropy store
    auto es = entropy_store::entropy_store{fetch};

    // Define some distributions we can generate
    // The const_distributions are compile-time constant and can lead to more efficient code
    // (for example by optimising integer divisions)
    entropy_store::const_uniform<1,6> d6;
    entropy_store::const_bernoulli<1,2> fair_coin;
    entropy_store::const_bernoulli<4,5> biassed_coin;

    // The non-const distributions 
    entropy_store::uniform_distribution d6_b{1,6};
    entropy_store::bernoulli_distribution fair_coin_b{1,2};
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

    // Shuffle a string
    std::string str = "abcdefghijklmnopqrstuvwxyz";
    entropy_store::shuffle(es, str);
    std::cout << "Shuffle: " << str << std::endl;
}
