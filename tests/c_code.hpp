namespace entropy_store
{
extern "C" uint32_t generate_uniform32(uint32_t n);

extern "C" uint32_t s_range;

template <typename Source> class c_code_source
{
  public:
    c_code_source(Source src) : m_source(src)
    {
    }

    template <std::integral T> auto operator()(const uniform_distribution<T> &dist)
    {
        return dist.min() + generate_uniform32(dist.size());
    }

    auto distribution() const
    {
        return binary_distribution{};
    }

    const auto &source() const
    {
        return m_source;
    }

    fetch_source<Source> m_source;
};

template <typename S> inline double internal_entropy(const c_code_source<S> &s)
{
    return std::log2(s_range);
}

template <typename S> inline int bits_fetched(const c_code_source<S> &s)
{
    return bits_fetched(s.source());
}

} // namespace entropy_store
