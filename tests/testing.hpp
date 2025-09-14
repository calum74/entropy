#pragma once

namespace entropy_store
{

template <entropy_generator Source> class stored_source
{
  public:
    using value_type = typename Source::value_type;
    using distribution_type = typename Source::distribution_type;
    using size_type = std::size_t;
    using source_type = typename Source::source_type;

    stored_source(const Source &source, size_type count) : m_source(source)
    {
        m_values.reserve(count);
        for (; count > 0; --count)
            m_values.push_back(m_source());
    }

    auto operator()()
    {
        if (m_values.empty())
            throw std::runtime_error("Out of values!");
        auto result = m_values.back();
        m_values.pop_back();
        return result;
    }

    auto distribution() const
    {
        return m_source.distribution();
    }

    int fetch_bit()
    {
        return m_source.fetch_bit();
    }

    std::vector<value_type> m_values;

  private:
    Source m_source;
};

template <entropy_generator Source> class wrapped_source
{
  public:
    using value_type = typename Source::value_type;
    using distribution_type = typename Source::distribution_type;
    using size_type = std::size_t;
    using source_type = Source;

    wrapped_source(const Source &source, size_type count) : m_source(source)
    {
        m_values.reserve(count);
        for (; count > 0; --count)
            m_values.push_back(m_source());
    }

    auto operator()()
    {
        ++m_index;
        if (m_index >= m_values.size())
            m_index = 0;
        return m_values[m_index];
    }

    auto distribution() const
    {
        return m_source.distribution();
    }

    int fetch_bit()
    {
        return m_source.fetch_bit();
    }

    std::vector<value_type> m_values;

  private:
    Source m_source;
    std::size_t m_index = 0;
};

template <typename Source, distribution Dist> class bound_entropy_generator
{
  public:
    bound_entropy_generator(Source source, const Dist &dist) : m_source(std::move(source)), m_dist(dist)
    {
    }

    auto operator()()
    {
        return m_source(m_dist);
    }

    using distribution_type = Dist;
    using value_type = typename distribution_type::value_type;
    using source_type = Source;

    const Dist &distribution() const
    {
        return m_dist;
    }

    const source_type &source() const
    {
        return m_source;
    }

  private:
    Source m_source;
    Dist m_dist;
};

template <typename Source, distribution Dist> auto bits_fetched(const bound_entropy_generator<Source, Dist> &g)
{
    return bits_fetched(g.source());
}

template <typename Source, distribution Dist> auto internal_entropy(const bound_entropy_generator<Source, Dist> &g)
{
    return internal_entropy(g.source());
}

inline double internal_entropy(const random_device_generator &)
{
    return 0;
}

} // namespace entropy_store
