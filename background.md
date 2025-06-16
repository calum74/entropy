# Abstract

We present new algorithms for converting random numbers between different forms using an "entropy store". 


The method can generate random variables for any weighted integer distribution, and consume entropy from any such distribution, whilst losing almost no entropy in the process.

This is much more flexible than state of the art methods such as arithmetic coding, because the asymptotic entropy efficiency is much more easily achieved.

The central idea is to capture potentially lost entropy in an entropy store, implemented as a uniform random variable. By  ensuring that the stored entropy is above a minimum size, we guarantee that resampling the entropy store to a multiple of the output number will lose a miniscule quantity of entropy. Once we have the ability to generate and consume uniform integers efficiently, we can build on this to generate and consume entropy from other distributions.

To illustrate the efficiency of the method, using a 32-bit entropy store the algorithm can shuffle a deck of 52 cards with a loss of only XXXX bits of entropy.


# Introduction


# Related work



Distribution generation

Although Knuth-Yao and the interval algorithm are optimal for generating single integers, they are not optimal in the amortised case, because they always have an entropy loss, or toll, of up to 2 bits per integer generated. For example to roll a d6, the entropy required is $\log_26 \approxeq ...$, but the Knuth-Yau algorithm requires $...$ bits on average.

To achieve an efficiency closer to 1, various batching schemes have been devised.

