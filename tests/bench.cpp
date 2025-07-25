#include "entropy_store.hpp"
#include "testing.hpp"

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <string_view>

using namespace std::literals;

int usage()
{
    std::cout << "Usage: bench static|$output $count\n";
    return 1;
}

int main(int argc, const char **argv)
{
    if (argc != 3)
    {
        return usage();
    }
    bool static_type = argv[1] == "static"sv;
    int count = std::atoi(argv[2]);

#if 1
    // Buffer the entropy
    entropy_store::random_device_generator rd;
    entropy_store::wrapped_source source{rd, 100000};
    entropy_store::bit_generator fetch{source};
#else
    // Read from std::random_device
    entropy_store::random_bit_generator fetch;
#endif

    auto es = entropy_store::entropy_store{fetch};
    auto start_time = std::chrono::high_resolution_clock::now();

    if (argv[1] == "static"sv)
    {
        entropy_store::const_uniform<1, 6> d6;
        for (int i = 0; i < count; i++)
            es(d6);
    }
    else if (int d = atoi(argv[1]))
    {
        entropy_store::uniform_distribution d6(1, d);
        for (int i = 0; i < count; i++)
            es(d6);
    }

    auto end_time = std::chrono::high_resolution_clock::now();

    std::cout << "Elapsed " << std::chrono::duration<double>(end_time - start_time) << "\n";

    std::cout << "Per output: " << (std::chrono::duration<double>(end_time - start_time) / count) << "\n";
    return 0;
}