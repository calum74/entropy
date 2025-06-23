# Abstract

We present new algorithms for converting random numbers between different forms using an "entropy store". 


The method can generate random variables for any weighted integer distribution, and consume entropy from any such distribution, whilst losing almost no entropy in the process.

This is much more flexible than state of the art methods such as arithmetic coding, because the asymptotic entropy efficiency is much more easily achieved.

The central idea is to capture potentially lost entropy in an entropy store, implemented as a uniform random variable. By  ensuring that the stored entropy is above a minimum size, we guarantee that resampling the entropy store to a multiple of the output number will lose a miniscule quantity of entropy. Once we have the ability to generate and consume uniform integers efficiently, we can build on this to generate and consume entropy from other distributions.

To illustrate the efficiency of the method, using a 32-bit entropy store the algorithm can shuffle a deck of 52 cards with a loss of only XXXX bits of entropy.


# Introduction


# Contribution

We have developed a new algorithms for

- uniform integer generation
- generating an integer weighted distribution
- extracting entropy from an integer weighted distribution

We have shown that these algorithms generate perfect distributions and are asymptotically optimal. The main benefit of these algorithms are their relative simplicity and flexibility, avoiding the need for batching or complex data structures.

# Proof of bijection

The problem with the recombine is that the number we combine must be independent of the RV in the store. However this is only true if you multiply the variable back in a certain way.

Let $f_{GD}: X \rightarrow Y,Z$ where $X \sim Uniform\{0,n-1\}$, $Y \sim Weighted$ and $Z \sim Uniform{0,W_{Y}-1}$, as implemented in Algorithm \ref{alg:generate-distribution}.



