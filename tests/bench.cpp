#include "entropy_store.hpp"
#include "testing.hpp"

#include <chrono>
#include <cstdlib>
#include <iostream>

static int grand_total = 0;

auto measure(auto generator, entropy_store::distribution auto dist, std::size_t N)
{
    int total = 0;
    auto start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < N; i++)
        total += generator(dist);
    auto end_time = std::chrono::high_resolution_clock::now();
    grand_total += total;
    return std::chrono::duration<double>(end_time - start_time) / N;
}

void report(auto i, auto generator, auto dist, auto source, auto time, auto relative)
{
    std::cout << i << ", " << generator << ", " << dist << ", " << source << ", " << time << ", " << (time / relative)
              << std::endl;
}

void benchmark_rng(auto source, int i, std::size_t N, const char * source_name)
{
    auto fetch = entropy_store::bit_generator{source};
    auto es32 = entropy_store::entropy_store32{fetch};
    auto es64 = entropy_store::entropy_store64{fetch};
    auto fdr = entropy_store::fast_dice_roller{fetch};
    auto huber_vargas = entropy_store::huber_vargas{fetch};
    auto von_neumann = entropy_store::von_neumann{fetch};
    auto lemire = entropy_store::lemire{source};

    // Distributions
    const entropy_store::const_uniform<1, 6> cd6;
    const entropy_store::uniform_distribution d6(1, 6);
    const entropy_store::uniform_distribution vd6(1, 6 + (errno >> 6)); // Disable some compiler optimizations

    measure(es32, vd6, N);
    auto benchmark = measure(es32, vd6, N);
    report(i, "ES32", "d6", source_name, measure(es32, d6, N), benchmark);
    report(i, "ES32", "cd6", source_name, measure(es32, cd6, N), benchmark);
    report(i, "ES32", "vd6", source_name, measure(es32, vd6, N), benchmark);
    report(i, "ES64", "d6", source_name, measure(es64, d6, N), benchmark);
    report(i, "ES64", "cd6", source_name, measure(es64, cd6, N), benchmark);
    report(i, "ES64", "vd6", source_name, measure(es64, vd6, N), benchmark);
    report(i, "Von Neumann", "vd6", source_name, measure(von_neumann, vd6, N), benchmark);
    report(i, "Fast Dice Roller", "vd6", source_name, measure(fdr, vd6, N), benchmark);
    report(i, "Huber-Vargas", "vd6", source_name, measure(huber_vargas, vd6, N), benchmark);
    report(i, "Lemire", "vd6", source_name, measure(lemire, vd6, N), benchmark);
}


int main(int argc, const char **argv)
{

#ifdef NDEBUG
    std::size_t N = 1000000;
#else
    std::cout << "*** Warning: This is a debug build ***\n";
    std::size_t N = 10000;
#endif
    std::cout << "Measuring for " << N << " outputs\n";

    // 32-bit sources
    entropy_store::random_device_generator rd_uncached;
    entropy_store::wrapped_source rd_cached{rd_uncached, 100000};
    entropy_store::prng_source prng;
    entropy_store::xoshiro128 xoshiro128{rd_uncached};

    // 1-bit sources
    entropy_store::bit_generator fetch_uncached{rd_uncached};
    entropy_store::bit_generator fetch_mt19937{prng};
    entropy_store::bit_generator fetch_cached{rd_cached};
    entropy_store::bit_generator fetch_xoshiro128{xoshiro128};

    // ES Generators
    auto es32_cached = entropy_store::entropy_store32{fetch_cached};
    auto es32_uncached = entropy_store::entropy_store32{fetch_uncached};
    auto es64_cached = entropy_store::entropy_store64{fetch_cached};
    auto es64_uncached = entropy_store::entropy_store64{fetch_uncached};
    auto es64_mt19937 = entropy_store::entropy_store64{fetch_mt19937};
    auto es32_mt19937 = entropy_store::entropy_store{fetch_mt19937};
    auto es32_xoshiro128 = entropy_store::entropy_store{fetch_xoshiro128};
    auto es64_xoshiro128 = entropy_store::entropy_store64{fetch_xoshiro128};

    // Other generators
    auto fdr_uncached = entropy_store::fast_dice_roller{fetch_uncached};
    auto fdr_cached = entropy_store::fast_dice_roller{fetch_cached};
    auto fdr_mt19937 = entropy_store::fast_dice_roller{fetch_mt19937};
    auto fdr_xoshiro128 = entropy_store::fast_dice_roller{fetch_xoshiro128};

    auto hv_cached = entropy_store::huber_vargas{fetch_cached};
    auto hv_uncached = entropy_store::huber_vargas{fetch_uncached};
    auto hv_mt19937 = entropy_store::huber_vargas{fetch_mt19937};
    auto hv_xoshiro128 = entropy_store::huber_vargas{fetch_xoshiro128};

    auto vn_cached = entropy_store::von_neumann{fetch_cached};
    auto vn_uncached = entropy_store::von_neumann{fetch_uncached};
    auto vn_mt19937 = entropy_store::von_neumann{fetch_mt19937};
    auto vn_xoshiro128 = entropy_store::von_neumann{fetch_xoshiro128};

    auto lemire_mt19937 = entropy_store::lemire{prng};
    auto lemire_cached = entropy_store::lemire{rd_cached};
    auto lemire_uncached = entropy_store::lemire{rd_uncached};
    auto lemire_xoshiro128 = entropy_store::lemire{xoshiro128};

    // Distributions
    const entropy_store::const_uniform<1, 6> cd6;
    const entropy_store::uniform_distribution d6(1, 6);
    const entropy_store::uniform_distribution vd6(1, 6 + (argc >> 6)); // Disable some compiler optimizations

    std::cout << "Iteration, Generator, Distribution, Source, Time per output, Relative output\n";
    for (int i = 0; i < 3; i++)
    {
        benchmark_rng(rd_uncached, i, N, "random_device");
        benchmark_rng(rd_cached, i, N, "cached");
        benchmark_rng(prng, i, N, "mt_19937");
        benchmark_rng(xoshiro128, i, N, "xoshiro128");
    }

    return 0;
}