#include "entropy_store.hpp"
#include <iostream>

int main()
{
    // Create a fetch function that returns one bit at a time
    entropy_store::random_bit_generator fetch;

    // Create our entropy store
    auto es = entropy_store::entropy_store{fetch};

    // Define some distributions we can generate or consume
    entropy_store::bernoulli_distribution coin13{1, 3};
    entropy_store::bernoulli_distribution coin12{1, 2};

    // Create a stream of coin1
    auto coins13 = entropy_store::entropy_converter{fetch, coin13};
    auto coins12 = entropy_store::entropy_converter{coins13, coin12};

    int counts[] = {0, 0};
    for (int i = 0; i < 10000; ++i)
        counts[coins12()]++;
    std::cout << counts[0] << "," << counts[1] << std::endl;
}
