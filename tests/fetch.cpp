#include "fetch.hpp"
#include "entropy_metrics.hpp"

// #include "xoshiro128.hpp"
// #include "mt19937.hpp"
// entropy_store::random_bit_generator bits;
// auto random_bits = entropy_store::counter{entropy_store::random_bit_generator{}};

std::shared_ptr<fetch_base> fetch_instance;

void install_fetch(std::shared_ptr<fetch_base> p)
{
    fetch_instance = p;
}

extern "C" int fetch()
{
    return fetch_instance->fetch();
}

extern "C" int flip()
{
    auto b = fetch_instance->fetch();
    assert(b == 0 || b == 1);
    return b;
}

