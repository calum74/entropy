#pragma once

namespace entropy_store
{
// Lemire's nearly divisionless

uint64_t nearlydivisionless(uint64_t s, auto random64)
{
    uint64_t x = random64();
    __uint128_t m = (__uint128_t)x * (__uint128_t)s;
    uint64_t l = (uint64_t)m;
    if (l < s) [[unlikely]]
    {
        uint64_t t = -s % s;
        while (l < t)
        {
            x = random64();
            m = (__uint128_t)x * (__uint128_t)s;
            l = (uint64_t)m;
        }
    }
    return m >> 64;
}

template <entropy_generator Source> class lemire
{
  public:
    lemire(const Source &source) : m_source(source)
    {
    }

    template <std::integral T> auto operator()(const uniform_distribution<T> &dist)
    {
        return nearlydivisionless(dist.size(), [&]() { return (uint64_t(m_source()) << 32) | m_source(); }) +
               dist.min();
    }

    template <std::integral T, T M, T N> auto operator()(const const_uniform_distribution<T, M, N> &dist)
    {
        return nearlydivisionless(dist.size(), [&]() { return (uint64_t(m_source()) << 32) | m_source(); }) +
               dist.min();
    }

    const auto &source() const
    {
        return m_source;
    }

  private:
    Source m_source;
};

template <typename Source> auto internal_entropy(const lemire<Source> &source)
{
    return internal_entropy(source.source());
}

template <entropy_generator Source> auto bits_fetched(const lemire<Source> &l)
{
    return bits_fetched(l.source());
}

} // namespace entropy_converter