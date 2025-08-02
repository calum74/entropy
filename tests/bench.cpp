#include "aldr.hpp"
#include "entropy_store.hpp"
#include "fldr.hpp"
#include "fwd.hpp"
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

void benchmark_rng(auto source, int i, std::size_t N, const char *source_name)
{
    auto fetch = entropy_store::bit_generator{source};
    auto es32 = entropy_store::entropy_store32{fetch};
    auto es64 = entropy_store::entropy_store64{fetch};
    auto fdr = entropy_store::fast_dice_roller{fetch};
    auto huber_vargas = entropy_store::huber_vargas{fetch};
    auto von_neumann = entropy_store::von_neumann{fetch};
    auto lemire = entropy_store::lemire{source}; // !! Add alias wrapper

    // Distributions
    const entropy_store::const_uniform<1, 6> fast_d6;
    const entropy_store::uniform_distribution d6(1, 6 + (errno >> 6)); // Disable some compiler optimizations
    const entropy_store::weighted_distribution weighted_bernoulli{1, 99};
    const entropy_store::const_bernoulli<1, 100> fast_bernoulli;
    const entropy_store::bernoulli_distribution bernoulli(1, 100 + (errno >> 6));
    const entropy_store::weighted_distribution weighted{1, 2, 3, 4, 5};
    const entropy_store::weighted_distribution weighted_d6{1, 1, 1, 1, 1, 1}; // FLDR cannot handle weight 0

    measure(es32, d6, N);
    auto benchmark_d6 = measure(es32, d6, N);
    measure(es32, bernoulli, N);
    auto benchmark_bernoulli = measure(es32, bernoulli, N);
    measure(es32, weighted, N);
    auto benchmark_weighted = measure(es32, weighted, N);

    report(i, "ES32", "d6", source_name, measure(es32, d6, N), benchmark_d6);
    report(i, "ES32 optimized", "cd6", source_name, measure(es32, fast_d6, N), benchmark_d6);
    report(i, "ES64", "d6", source_name, measure(es64, d6, N), benchmark_d6);
    report(i, "ES64 optimized", "d6", source_name, measure(es64, fast_d6, N), benchmark_d6);
    report(i, "VN", "d6", source_name, measure(von_neumann, d6, N), benchmark_d6);
    report(i, "Fast Dice Roller", "d6", source_name, measure(fdr, d6, N), benchmark_d6);
    report(i, "FLDR", "d6", source_name, measure(fldr_source{fetch, weighted_d6}, weighted_d6, N), benchmark_d6);
    report(i, "ALDR", "d6", source_name, measure(aldr_source{fetch, weighted_d6}, weighted_d6, N), benchmark_d6);
    report(i, "Huber-Vargas", "d6", source_name, measure(huber_vargas, d6, N), benchmark_d6);
    report(i, "Lemire", "d6", source_name, measure(lemire, d6, N), benchmark_d6);

    report(i, "ES32", "Bernoulli", source_name, measure(es32, bernoulli, N), benchmark_bernoulli);
    report(i, "ES32 optimized", "Bernoulli", source_name, measure(es32, fast_bernoulli, N), benchmark_bernoulli);
    report(i, "FLDR", "Bernoulli", source_name, measure(fldr_source{fetch, weighted_bernoulli}, weighted_bernoulli, N),
           benchmark_bernoulli);
    report(i, "ALDR", "Bernoulli", source_name, measure(aldr_source{fetch, weighted_bernoulli}, weighted_bernoulli, N),
           benchmark_bernoulli);

    report(i, "Lemire+alias", "Bernoulli", source_name, measure(entropy_store::alias_method{lemire}, bernoulli, N),
           benchmark_bernoulli);

    report(i, "ES32", "Weighted", source_name, measure(es32, weighted, N), benchmark_weighted);
    report(i, "FLDR", "Weighted", source_name, measure(fldr_source{fetch, weighted}, weighted, N), benchmark_weighted);
    report(i, "ALDR", "Weighted", source_name, measure(aldr_source{fetch, weighted}, weighted, N), benchmark_weighted);

    report(i, "Lemire+alias", "Weighted", source_name, measure(entropy_store::alias_method{lemire}, weighted, N),
           benchmark_weighted);
}

int main(int argc, const char **argv)
{

#ifdef NDEBUG
    std::size_t N = 1000000;
#else
    std::cout << "*** Warning: This is a debug build ***\n";
    std::size_t N = 10000;
#endif

    // 32-bit sources
    entropy_store::random_device_generator rd_uncached;
    entropy_store::wrapped_source rd_cached{rd_uncached, 100000};
    entropy_store::prng_source prng;
    entropy_store::xoshiro128 xoshiro128{rd_uncached};

    std::cout << "Iteration, Generator, Distribution, Source, Time per output, Relative time\n";
    for (int i = 0; i < 3; i++)
    {
        benchmark_rng(rd_uncached, i, N, "random_device");
        // benchmark_rng(rd_cached, i, N, "cached");
        benchmark_rng(prng, i, N, "mt19937");
        benchmark_rng(xoshiro128, i, N, "xoshiro128");
    }

    return 0;
}