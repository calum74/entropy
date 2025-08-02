#include "fetch.hpp"
#include "entropy_metrics.hpp"

std::shared_ptr<entropy_store::fetch_base> fetch_instance;

void entropy_store::install_fetch(std::shared_ptr<fetch_base> p)
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

