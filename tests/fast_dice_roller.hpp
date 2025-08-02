#pragma once

namespace entropy_store
{

template <binary_entropy_generator Source> class fast_dice_roller
{
  public:
    fast_dice_roller(const Source &source) : m_source(source)
    {
    }

    template <std::integral T> auto operator()(const uniform_distribution<T> &dist)
    {
        T r = 0, s = 1;
        const auto N = dist.size();
        for (;;)
        {
            while (s < N)
            {
                s = s << 1;
                r = (r << 1) | m_source();
            }
            if (r < N)
            {
                return r + dist.min();
            }
            r -= N;
            s -= N;
        }
    }

    const Source &source() const
    {
        return m_source;
    }

  private:
    Source m_source;
};

template <typename Source> auto internal_entropy(const fast_dice_roller<Source> &source)
{
    return internal_entropy(source.source());
}

template <typename Source> auto bits_fetched(const fast_dice_roller<Source> &g)
{
    return bits_fetched(g.source());
}

}