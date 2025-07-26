#include "entropy_store.hpp"
#include "testing.hpp"

#include <chrono>
#include <cstdlib>
#include <iostream>

void measure(const char *name, std::invocable<std::size_t> auto fn, std::size_t n)
{
    fn(n);
    auto start_time = std::chrono::high_resolution_clock::now();
    fn(n);
    auto end_time = std::chrono::high_resolution_clock::now();
    std::cout << name << ": Time per output = " << (std::chrono::duration<double>(end_time - start_time) / n)
              << std::endl;
}

int main(int argc, const char **argv)
{

#ifdef NDEBUG
    std::size_t N = 10000000;
#else
    std::cout << "*** Warning: This is a debug build ***\n";
    std::size_t N = 10000;
#endif
    std::cout << "Measuring for " << N << " outputs\n";

    entropy_store::random_device_generator rd_uncached;
    entropy_store::wrapped_source rd_cached{rd_uncached, 100000};
    entropy_store::bit_generator fetch_cached{rd_cached};
    entropy_store::random_bit_generator fetch_uncached;

    for (int i = 0; i < 3; i++)
    {
        std::cout << "\nIteration " << i << std::endl;

        measure(
            "uniform_distribution<1,6>",
            [&](auto N) {
                // This is a bit slower (3.23ns per output)
                auto es = entropy_store::entropy_store{fetch_cached};
                entropy_store::uniform_distribution dice(1, 6);
                for (int i = 0; i < N; i++)
                    es(dice);
            },
            N);

        measure(
            "uniform_distribution<1,6>",
            [&](auto N) {
                // This is a bit slower (3.23ns per output)
                auto es = entropy_store::entropy_store{fetch_cached};
                entropy_store::uniform_distribution dice(1, 6);
                for (int i = 0; i < N; i++)
                    es(dice);
            },
            N);

        measure(
            "const_uniform<1,6>",
            [&](auto N) {
                // const_uniform is faster (2.25ns vs 2.4ns vs per output)
                // entropy_store::uniform_distribution d6(1, 6);
                auto es = entropy_store::entropy_store{fetch_cached};
                entropy_store::const_uniform<1, 6> d6;
                for (int i = 0; i < N; i++)
                    es(d6);
            },
            N);

        measure(
            "volatile uniform_distribution<1,6>",
            [&](auto N) {
                // This is a bit slower (3.23ns per output)
                auto es = entropy_store::entropy_store{fetch_cached};
                volatile int d = 6;
                entropy_store::uniform_distribution dice(1, d);
                for (int i = 0; i < N; i++)
                    es(dice);
            },
            N);

        measure(
            "volatile uniform_distribution<1,6> 64-bit",
            [&](auto N) {
                // This is a bit slower (3.23ns per output)
                auto es = entropy_store::entropy_store64{fetch_cached};
                volatile int d = 6 + (argc >> 4); // Disable division optimizations
                entropy_store::uniform_distribution dice(1, d);
                for (int i = 0; i < N; i++)
                    es(dice);
            },
            N);

        measure(
            "fast dice roller",
            [&](auto N) {
                // This is a bit slower (3.23ns per output)
                auto fdr = entropy_store::fast_dice_roller{fetch_cached};
                entropy_store::uniform_distribution dice(1, 6);
                for (int i = 0; i < N; i++)
                    fdr(dice);
            },
            N);

        measure(
            "Von Neumann",
            [&](auto N) {
                // This is a bit slower (3.23ns per output)
                auto fdr = entropy_store::von_neumann{fetch_cached};
                entropy_store::uniform_distribution dice(1, 6);
                for (int i = 0; i < N; i++)
                    fdr(dice);
            },
            N);

        measure(
            "Von Neumann (uncached)",
            [&](auto N) {
                // This is a bit slower (3.23ns per output)
                auto fdr = entropy_store::von_neumann{fetch_uncached};
                entropy_store::uniform_distribution dice(1, 6);
                for (int i = 0; i < N; i++)
                    fdr(dice);
            },
            N);

        measure(
            "Lemire",
            [&](auto N) {
                entropy_store::lemire L{rd_cached};
                entropy_store::uniform_distribution dice(1, 6);
                for (int i = 0; i < N; i++)
                    L(dice);
            },
            N);

        measure(
            "Lemire (uncached)",
            [&](auto N) {
                entropy_store::lemire L{rd_uncached};
                entropy_store::uniform_distribution dice(1, 6);
                for (int i = 0; i < N; i++)
                    L(dice);
            },
            N);
    }

    return 0;
}