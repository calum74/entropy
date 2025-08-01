#pragma once

template<typename S> class fldr_source;

namespace entropy_source
{
    template<typename S> double internal_entropy(const fldr_source<S>&);
    template<typename S> int bits_fetched(const fldr_source<S>&s);
}