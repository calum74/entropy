#include "entropy_store.hpp"
#include <string_view>
#include <iostream>
#include <cstdlib>

using namespace std::literals;

int usage()
{
    std::cout << "Usage: bench static|runtime count\n";
    return 1;
}

int main(int argc, const char **argv)
{
    if(argc!=3)
    {
        return usage();
    }
    bool static_type = argv[1] == "static"sv;
    bool dynamic_type = argv[1] == "runtime"sv;
    int count = std::atoi(argv[2]);

    if(static_type == dynamic_type)
    {
        return usage();
    }

    entropy_store::random_bit_generator fetch;
    auto es = entropy_store::entropy_store{fetch};

    if(static_type)
    {
        entropy_store::const_uniform<1,6> d6;
        for(int i=0; i<count; i++)
            es(d6);
    }
    else
    {
        entropy_store::uniform_distribution d6(1,6);
        for(int i=0; i<count; i++)
            es(d6);
    }

    return 0;
}