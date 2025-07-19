#include "entropy_store.hpp"
#include "testing.hpp"
#include <iostream>

int main()
{
    // Create a fetch function that returns one bit at a time
    entropy_store::random_bit_generator fetch;

    // Create our entropy store
    auto es = entropy_store::entropy_store{fetch};

    // Define some distributions we can generate or consume
    entropy_store::bernoulli_distribution coin_in{1, 3};
    entropy_store::bernoulli_distribution coin_out{1, 2};

    // Create a stream of coin1
    auto coins_in = entropy_store::entropy_converter{fetch, coin_in};

    auto coins_out = entropy_store::entropy_converter{coins_in, coin_out};

    auto test_out = entropy_store::check_distribution {coins_out};

    int counts[] = {0, 0};
    for (int i = 0; i < 10000; ++i)
        counts[test_out()]++;

    std::cout << test_out;
    if(test_out.check_sigma())
    {
        std::cout << "Test passed!\n";
        return 0;
    }
    else
    {
        std::cout << "*** Test FAILED ***\n";
        return 1;
    }
}
