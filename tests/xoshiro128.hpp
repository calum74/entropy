
namespace entropy_store
{
// Code copied from https://prng.di.unimi.it/xoshiro128plusplus.c
class xoshiro128
{
  public:
    xoshiro128(entropy_generator auto &seed)
    {
        for (int i = 0; i < 4; i++)
            s[i] = seed();
    }

    using value_type = uint32_t;
    using distribution_type = const_uniform_distribution<value_type, std::numeric_limits<value_type>::min(),
                                                         std::numeric_limits<value_type>::max()>;

    constexpr distribution_type distribution() const
    {
        return {};
    }

    constexpr value_type min() const
    {
        return 0;
    }
    constexpr value_type max() const
    {
        return std::numeric_limits<value_type>::max();
    }

    value_type operator()()
    {
        const uint32_t result = rotl(s[0] + s[3], 7) + s[0];

        const uint32_t t = s[1] << 9;

        s[2] ^= s[0];
        s[3] ^= s[1];
        s[1] ^= s[2];
        s[0] ^= s[3];

        s[2] ^= t;

        s[3] = rotl(s[3], 11);

        return result;
    }

    int bits() const
    {
        return 32;
    }

  private:
    uint32_t rotl(const uint32_t x, int k)
    {
        return (x << k) | (x >> (32 - k));
    }

    uint32_t s[4];
};

inline double internal_entropy(const xoshiro128 &s)
{
    return 0;
}

}