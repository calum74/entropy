# Introduction

In this paper, we will study the generation of perfectly distributed integers from an entropy source. A typical example of this is using fair coin flips to roll a fair die or to perform a perfect shuffle of a deck of cards, but there are practical applications of this such as XX.

The general problem is one of "entropy conversion", where entropy in one form needs to be converted to entropy in a different form using a function $f$ on a discrete random variable $X$ having the Shannon entropy $H(X)$.  A fundamental result is that you cannot get more entropy out than you put in, or $H(X) \ge H(f(X))$ \ref{entropy}. We are fundamentally interested in improving the efficiency of entropy conversion, defined as $\eta = \frac{H(f(X))}{H(X)}$.

Whilst this problem has been studied extensively for generating integers, there are theoretical limits on the entropy efficiency when generating integers one at a time, because algorithms must always fetch at least a whole number of bits. In fact we must fetch up to 2 extra bits of entropy per integer generated \ref{xx}.  For example to roll a fair die requires $...$ coin flips on average, but the entropy contained in the dice roll is $\log26 \approxeq XX$. When shuffling a deck of 52 cards, fetching integers one at a time requires XX bits of entropy, against a theoretical entropy of $\log_2(52!) \approxeq XX$.

To mitigate these entropy losses, it's possible to generate uniform integers in batches. The major drawback with existing batching schemes is that they are somewhat complex to implement, and require you to know beforehand which numbers you need to generate. Whilst they improve entropy efficiency they still incur inefficiencies.

In this paper we will explore a fundamentally different approach. We will allow every entropy conversion algorithm to have access to an "entropy store" in the form of a large uniformly distributed integer (of the order of 32 bits), and allow the algorithm to put back any unused entropy into the store. This scheme bypasses the Knuth-Yao limit for entropy efficiency, and in fact allows a near-perfect entropy conversion efficiency through restructuring entropy-transforming steps to minimise entropy loss. 

This idea allows us to create near-optimal conversion algorithms to convert binary entropy to a uniform integer, and we can use the entropy store to create near-optimal conversion algorithms to and from integer weighted integer distributions.

The entropy efficiency depends only on the ratio of the size of the store $N$ to the size $n$ of the number generated, and a precise bound is calculated in Theorem \ref{xx}. For example to roll a 6-sided die with a 32-bit entropy store ($N\ge2^{31}) has an entropy efficiency of .... To shuffle a deck of 52 cards with a 32-bit entropy store has an entropy efficiency of XXX. By increasing the size of the store, we can achieve entropy efficiency arbitrarily close to 1.

The algorithms are easy to implement, require O(1) time and memory, and use two integer divmod operations per uniform integer generated. To convert between distributions, and additional $O(n)$ of lookup tables is required. The main benefit of these algorithms is their simplicity, flexibility and high efficiency.

# Related work



Distribution generation

Although Knuth-Yao and the interval algorithm are optimal for generating single integers, they are not optimal in the amortised case, because they always have an entropy loss, or toll, of up to 2 bits per integer generated. For example to roll a d6, the entropy required is $\log_26 \approxeq ...$, but the Knuth-Yau algorithm requires $...$ bits on average.

To achieve an efficiency closer to 1, various batching schemes have been devised.

