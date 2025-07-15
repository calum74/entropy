#pragma once

#include <cstdint>
#include <cmath>
#include <iostream>
#include <cassert>

namespace entropy_store
{
    // Checks that multiple samplings of a uniform distribution
    // occur in the expected range. According to the central limit theorem
    // However, they are not independent.     
    class debug_uniform
    {
    public:
        using value_type = std::uint64_t;
        void record(value_type value, value_type size)
        {
            assert(value<size);
            total_value += value;
            total_size += size - 1;
            total_count++;
            sum_s_squared += size * size - 1;
        }

        double sd() const
        {
            return std::sqrt(sum_s_squared / 12.0);
        }

        double mean() const
        {
            return total_size / 2;
        }

        double get_relative_bias() const
        {
            return (total_value - mean()) / sd();
        }

        debug_uniform(const char *place) : place(place) {}

        ~debug_uniform()
        {
            auto d = get_relative_bias();
            if (std::abs(d) > 4)
            {
                std::cout << place << ": uniform distribution failed with SD=" << d << std::endl;
            }
        }

    private:
        const char *place;
        std::uint64_t total_value = 0, total_size = 0, total_count = 0;
        double sum_s_squared = 0;
    };
}
