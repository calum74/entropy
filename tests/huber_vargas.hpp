#pragma once

namespace entropy_store
{

template <binary_entropy_generator Source> class huber_vargas
{
  public:
    huber_vargas(Source source) : m_source(source)
    {
    }

    template <std::integral T> auto operator()(const uniform_distribution<T> &dist)
    {
        // See Huber & Vargas (2024), "Optimal rolling of fair dice using fair coins"
        T m = 1, X = 1;
        T n = dist.size();
        while (m < n)
        {
            auto B = m_source();
            X = X + B * m;
            m = m << 1;
            if (m >= n && X <= n)
                m = n;
            if (m >= n && X > n)
            {
                X = X - n;
                m = m - n;
            }
        }

        return X + dist.min() - 1;
    }

    const auto &source() const
    {
        return m_source;
    }

  private:
    Source m_source;
};

template <binary_entropy_generator Source> auto bits_fetched(const huber_vargas<Source> &src)
{
    return bits_fetched(src.source());
}

template <typename Source> double internal_entropy(const huber_vargas<Source> &s)
{
    return internal_entropy(s.source());
}

}