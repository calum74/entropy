#pragma once

class fldr_source;

namespace entropy_source
{
    double internal_entropy(const fldr_source&);
    int bits_fetched(const fldr_source&s);
}