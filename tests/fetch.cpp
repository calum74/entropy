#include "fetch.hpp"
#include "entropy_metrics.hpp"
#include "xoshiro128.hpp"

#include "mt19937.hpp"

// random_bit_generator bits;
auto random_bits = entropy_store::counter{entropy_store::random_bit_generator{}};

extern "C" int fetch()
{
    return random_bits();
}

extern "C" int flip()
{
    return random_bits();
}

int entropy_store::bits_fetched(const fetch_source &source)
{
    return bits_fetched(random_bits);
}

double entropy_store::internal_entropy(const fetch_source &)
{
    return 0;
}

fetch_source::fetch_source()
{
    random_bits.reset();
}