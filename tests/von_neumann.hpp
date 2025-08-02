#pragma once

namespace entropy_store
{
template <binary_entropy_generator Source> class von_neumann
{
  public:
    von_neumann(const Source &source) : m_source(source)
    {
    }

    template <std::integral T> auto operator()(const uniform_distribution<T> &dist)
    {
        const auto N = dist.size();
        for (;;)
        {
            T r = 0, s = 1;
            while (s < N)
            {
                s = s << 1;
                r = (r << 1) | m_source();
            }
            if (r < N)
            {
                return r + dist.min();
            }
        }
    }

    const Source &source() const
    {
        return m_source;
    }

  private:
    Source m_source;
};


template <typename Source> auto internal_entropy(const von_neumann<Source> &source)
{
    return internal_entropy(source.source());
}


template <typename Source> auto bits_fetched(const von_neumann<Source> &g)
{
    return bits_fetched(g.source());
}

}