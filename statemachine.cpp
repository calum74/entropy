#include <iostream>
#include <random>

template<int N, int V=1, typename Src>
int fast_dice_roller(Src&& src, int c=0)
{
    c = 2*c + src();
    if constexpr ((2*V)>=N)
    {
        if (c<N)
            return c;
        else 
            return fast_dice_roller<N, 2*V-N>(src, c-N);
    }
    else
    {
        return fast_dice_roller<N,2*V>(src, c);
    }
}

int main()
{
    std::random_device d;
    auto entropy = [&] { return d()%2 ? 1 : 0; };
    std::cout << fast_dice_roller<47>(entropy) << std::endl;
}