#include "fetch.hpp"
#include "entropy_metrics.hpp"
#include "xoshiro128.hpp"

#include "mt19937.hpp"

entropy_store::random_bit_generator bits;

auto random_bits = entropy_store::counter{entropy_store::random_bit_generator{}};
fetch_base * fetch_instance = nullptr;

void fetch_base::install()
{
    fetch_instance = this;
}

extern "C" int fetch()
{
    return bits();
    return fetch_instance->fetch();
}

extern "C" int flip()
{
    return bits();
    return fetch_instance->fetch();
}

