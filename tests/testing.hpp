#include <iostream>

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
        assert(value < size);
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
        return total_size / 2.0;
    }

    double get_relative_bias() const
    {
        return (total_value - mean()) / sd();
    }

    double get_percentage_bias() const
    {
        return 100.0 * (total_value - mean()) / total_value;
    }

    debug_uniform(const char *place) : place(place)
    {
    }

    ~debug_uniform()
    {
        std::cout << place << ": uniform distribution bias = " << get_percentage_bias() << "%\n";
    }

  private:
    const char *place;
    std::uint64_t total_value = 0, total_size = 0, total_count = 0;
    double sum_s_squared = 0;
};

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

} // namespace entropy_store
