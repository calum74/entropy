#pragma once

namespace entropy_store
{
template <typename PRNG> class prng_source
{
  public:
    prng_source(const PRNG &prng = {}) : m_prng(prng)
    {
    }

    using value_type = typename PRNG::result_type;
    auto max() const
    {
        return m_prng.max();
    }
    auto min() const
    {
        return m_prng.min();
    }

    value_type operator()()
    {
        return m_prng();
    }

    using distribution_type = const_uniform<PRNG::min(), PRNG::max()>;

    distribution_type distribution() const
    {
        return distribution_type{};
    }

  private:
    PRNG m_prng;
};

using mt19937_source = prng_source<std::mt19937>;

}