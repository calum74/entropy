#include "entropy_store.hpp"
#include <iostream>

int main()
{
    // Create a fetch function that returns one bit at a time
    entropy_store::random_bit_generator fetch;

    // Create our entropy store
    auto es = entropy_store::entropy_store{fetch};

    entropy_store::uniform_distribution d6(0, 5);

    entropy_store::debug_uniform d("d6");
    for (int i = 0; i < 1000000; i++)
    {
        auto x = es(d6);
        d.record(x, 6);
    }
    return 0;

    // Define some distributions we can generate or consume
    entropy_store::bernoulli_distribution coin1{1, 3};
    entropy_store::bernoulli_distribution coin2{1, 2};

    // Create a stream of coin1
    auto coins1 = entropy_store::entropy_converter{fetch, coin1};
    auto coins2 = entropy_store::entropy_converter{coins1, coin2};

    int counts[] = {0, 0};
    for (int i = 0; i < 1000000; ++i)
        counts[coins2()]++;
    std::cout << counts[0] << "," << counts[1] << std::endl;
}
