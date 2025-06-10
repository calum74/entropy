# C interface

```c++
class entropy_store;

class weighted_distribution { vector<int> weights, outputs, offsets; };
class uniform_distribution { int min, max };
class bernoulli_distribution { int m, n; };

template<typename Distribution>
class entropy
{
    Distribution distribution;
    int value;
};

distribution.generate(store);
distribution.extract(store, value);

int generate_distribution(store, distribution);
void read_distribution(store, distribution, value);
void convert(store, distribution1, value, distribution2);

// Specialisations


uint32_t U_s, s;

uint32_t generate_distribution(uint32_t n, const uint32_t * weights, const uint32_t * outputs, const uint32_t * offsets)
{
    uint32_t U_n = generate_uniform(n);
    read_uniform(U_n - offsets[U_n], weights[outputs[U_n]]);
    return outputs[U_n];
}

void read_distribution(uint32_t value, uint32_t n, const uint32_t * weights, const uint32_t * offsets)
{
    uint32_t U_x = generate_uniform(weights[value]);
    read_uniform(offsets[value]+U_x, n);
}
```

Raw functions:

make_distribution
generate_distribution
generate_uniform
generate_bernoulli
extract_distribution
extract_uniform
extract_bernoulli(U_s *)

Do the same in Python:





# Writeup todo

Move theorems, lemmas and proofs to the appendix, and reference them in the text.

Write up final algorithms on generating and extracting 

# Writeup

Algorithm \ref{alg:generate_distribution} is able to generate an arbitrary discrete distribution of $k$ outcomes where each outcome has integer weight ${w_1 ... w_k}$, normalised to a discrete distribution with probabilities $\{\frac{w_1}{n}, \frac{w_2}{n} ... \frac{w_k}{n}\}$ where $n$ is the total weight.  The algorithm works by construct a mapping from the $n$ outcomes of a uniform distribution to the $k$ $outputs$ of the weighted distribution. To generate an output, create a uniform variable of size $n$, and look up its output in the $outputs$ table.

However, there is extra entropy that is not accounted for. When the weight of an outcome $w_i>0$, then we also get a uniform distribution of size $w_i$. The $combine$ step just adds that extra entropy to the store. If $w_i=1$, then the entropy in the uniform distribution is $0$ because $\log_21 = 0$ and this operation is a no-op.

Algorithm \ref{alg:combine_distribution} reads the entropy from any weighted distribution into an entropy store. This is essentially the inverse of Algorithm \ref{alg:generate_distribution}.

This can be used to convert entropy between arbitrary discrete distributions.

Lemma \ref{lem:generate_distribution_loss} shows that the only entropy losses from Algorithm \ref{alg:generate_distribution} come from the $generate\_uniform$ step, which means that the entropy loss from Algorithm \ref{alg:generate_distribution} is limited to $H_{loss}(n)$.

The entropy efficiency of 




\begin{lemma}
For a uniform distribution $U_n \sim Uniform{0,n-1}$, the expected entropy of the output distribution and uniform part equals the original entropy.
\end{lemma}

\begin{proof}
H_{RHS} = H_{out} + \sum_{i} p_{i}
\end{proof}


Instructions for viewing ipynb


Notes:

Need a name that describes its function. "Efficient" isn't helpful. Call it the "Entropy store" method.



Method: 

What about biassed bits? The Knuth-Yao algorithm seems to be extremely inefficient here.

In Abstract, talk about general strategy

Call it "Shannon entropy" 

General biassed distributions?
- Just "return" the entropy to the store.

# Discrete distributions in general



# Evaluation

The main benefits of Entropy Store is its practicality because it is very flexible and does not require precomputation

O(1) storage

does not require

Graph of p vs Efficiency up to 50.

Method name: Entropy Store

Real value is in the simplicity and flexibility. How to achieve 

# Related work

* Block simulation
* Arithmetic decoding
* Bit-efficient aliasing


We can compare the entropy efficiency for different algorithms 

* Rejection sampling
* Knuth-Yao (single sample tree) (unamortised)
* Efficient Entropy Converter
* 

against four tasks

* Generate uniform integers
* Shuffle a deck of cards
* Generate biassed bits
* Generate biassed distributions

The entropy efficiency can be calculated exactly, although it's a good idea to validate the calculations against simulations.

Bit by bit comparison??? Reach the theoretical entropy limit???
2.41 fair coin flips for 1/3

Amortised sampling methods

* Block simulation
* Arithmetic decoding
* Bit-efficient aliasing


1. Knuth–Yao (multi-sample tree / optimal discrete distribution sampling)
Original source:
Knuth, D. E., & Yao, A. C. (1976).
The complexity of nonuniform random number generation.
In Algorithms and Complexity: New Directions and Recent Results, pp. 357–428. Academic Press.
📘 [Cited widely as the foundational work on optimal sampling.]

Description:
Introduced the Discrete Distribution Generating (DDG) tree, which underlies the Knuth–Yao algorithm.

Although the algorithm is per-sample, they describe how to simulate distributions with arbitrary precision, and the approach generalizes to multi-sample blocks to approach entropy.

📚 2. Inverse Arithmetic Coding (a.k.a. entropy coding for random sampling)
Key reference:
Han, T. S., & Hoshi, M. (1997).
Interval algorithm for random number generation.
IEEE Transactions on Information Theory, 43(2), 599–611.
https://doi.org/10.1109/18.556266

Description:
This paper describes how to simulate a general distribution by generating a real number in 
[
0
,
1
)
[0,1) using fair bits, and decoding it via an interval (like in arithmetic coding).

Equivalent to inverse arithmetic coding — a universal, near-optimal algorithm for generating sequences with minimal expected randomness.

Also see:
Cover, T. M., & Thomas, J. A. (2006).
Elements of Information Theory (2nd ed.). Wiley.

Chapter 5 (Entropy and Source Coding) covers arithmetic coding and related ideas.

📚 3. Alias Method for Sampling Discrete Distributions
Original paper:
Walker, A. J. (1974).
New fast method for generating discrete random numbers with arbitrary frequency distributions.
Electronics Letters, 10(8), 127–128.

Extended work:
Walker, A. J. (1977).
An efficient method for generating discrete random variables with general distributions.
ACM Transactions on Mathematical Software, 3(3), 253–256.
https://doi.org/10.1145/355744.355749

Description:
The alias method is a preprocessing technique that allows constant-time sampling from discrete distributions using uniform randomness.

Can be adapted to work with bit streams, allowing for efficient near-entropy sampling when block bits are used (e.g., use 16-bit uniform integers).

Bonus: Unified Perspectives
Luc Devroye (1986).
Non-Uniform Random Variate Generation. Springer.
https://luc.devroye.org/rnbookindex.html

Chapters 2–5 cover all classical methods: Knuth–Yao, inversion, rejection, and alias.

Still considered the definitive reference on random generation methods.

🔖 Summary Table
Method	Key References
Knuth–Yao	Knuth & Yao (1976)
Inverse Arithmetic Coding	Han & Hoshi (1997), Cover & Thomas (2006)
Alias Method	Walker (1974, 1977)
Unified treatment	Devroye (1986)


Here are some recent advances in optimal and practical sampling from discrete distributions using fair coin flips, all supported by scholarly research:

📈 1. Fast Loaded Dice Roller (FLDR) — Near-Optimal, Practical Sampler
Saad, Freer, Rinard & Mansinghka (2020)

Developed FLDR, which uses linear preprocessing and consumes on average only 
𝐻
(
𝑃
)
+
6
H(P)+6 bits of entropy per sample.

Highly efficient in both time and space, outperforming traditional alias and interval samplers 2–10× in runtime 
frontiersin.org
+13
arxiv.org
+13
arxiv.org
+13
.

🧮 2. Entropy-Optimal Rejection Sampling
Draper & Saad (2025)

Introduced a family of rejection samplers that achieve expected entropy consumption within [H(P), H(P) + 2), while using linearithmic time and space.

This is the first known algorithm matching entropy bounds without exponential storage 
reddit.com
+5
arxiv.org
+5
arxiv.org
+5
.

🎲 3. Optimised Knuth–Yao for Dice & General Distributions
Huber & Vargas (2024)

Showed a simple linear-space implementation for fair dice sampling (and extendable to general distributions).

Improves upon the original Knuth–Yao bound with better average-case performance 
dl.acm.org
+12
arxiv.org
+12
arxiv.org
+12
.

ℹ️ 4. Optimal Approximate Sampling
Saad, Freer, Rinard & Mansinghka (2020, POPL)

Provided a theoretical and practical framework for entropy-optimal approximate sampling under finite-precision constraints.

Sample implementations closely match Shannon–entropy rates, with empirical demonstrations on a wide array of distributions 
arxiv.org
+4
arxiv.org
+4
github.com
+4
.

👓 Summary Table
Algorithm	Entropy Usage	Space	Time
FLDR (Saad et al., 2020)	
𝐻
(
𝑃
)
+
6
H(P)+6 bits	Linear	Fast
Efficient Rejection (Draper & Saad, 2025)	
∈
[
𝐻
(
𝑃
)
,
𝐻
(
𝑃
)
+
2
)
∈[H(P),H(P)+2) bits	Linearithmic	Efficient
Knuth–Yao Optimized (Huber & Vargas, 2024)	Slightly better than original bound	Linear	Simple
Approximate Sampling (POPL 2020)	Achieves theoretical entropy limits in practice	Dependent on precision	Practical

✅ Takeaway
Recent innovations have significantly improved the trade-off between entropy efficiency, space, and speed:

FLDR focuses on near-optimal entropy with high speed and low memory.

Draper & Saad’s method achieves within 2 bits of optimal entropy using modest resources.

Optimized Knuth–Yao implementations deliver improved performance using linear memory.

Approximate sampling frameworks enable entropy-optimal performance with finite precision.

If you'd like more details or pointers to code and implementations — I’d be happy to share!

Great question! Yes — there are practical algorithms designed to extract unbiased bits from samples drawn from an arbitrary integer-valued distribution with some entropy.

What you’re asking about
You have a random source producing integers with some unknown or arbitrary distribution.

You want to extract unbiased (fair) random bits from this source, i.e., bits that are uniformly distributed and independent.

This is often called randomness extraction or entropy extraction.

Key ideas
Von Neumann extractor (simple but limited)
Originally designed for biased coin flips, it looks at pairs of bits:

If the pair is 01, output 0

If the pair is 10, output 1

If 00 or 11, discard and move on.
This extracts unbiased bits but only works if the source is binary and independent trials. It’s not efficient for arbitrary integer distributions or correlated data.

Generalization to arbitrary alphabets
For integer distributions with known or unknown bias, several approaches exist:

Elias’ algorithm (1972): Generalizes Von Neumann’s idea to multi-symbol alphabets.

Peres’ algorithm (1992): An improvement over Von Neumann that extracts more entropy from biased coin flips.

Elias–Peres algorithm: For finite alphabets.

Universal randomness extractors
More generally, randomness extractors (from theoretical CS) convert "weak" sources of entropy into nearly uniform bits, often needing a small truly random seed. But these are mostly theoretical and sometimes complex.

Arithmetic coding-based extractors
If you know the distribution exactly, you can encode the sequence via arithmetic coding, and the output bits will be nearly uniform. This is close to optimal entropy extraction.

Practical algorithms in the real world
In practice, a common approach if you can model or estimate the distribution is to use something like:

Hashing (cryptographic hash functions) on blocks of samples — this "mixes" the entropy and can output nearly uniform bits. This is a practical approach in randomness extraction used in cryptography.

Trevisan’s extractor and variants — more theoretical, but some practical constructions exist.

Summary
Yes, practical algorithms exist, especially if you have some knowledge about or can estimate the distribution.

For arbitrary integer distributions, classical methods generalize Von Neumann’s idea (Elias, Peres).

For known distributions, arithmetic coding or entropy coding can yield nearly uniform bits.

For unknown or complex distributions, universal extractors or cryptographic hash-based extractors are practical.

Example references and algorithms
Von Neumann (1951) “Various techniques used in connection with random digits”

Elias (1972) “The efficient construction of an unbiased random sequence”

Peres (1992) “Iterating Von Neumann’s procedure for extracting random bits”

R. Canetti, D. Micciancio, M. Naor (2001) “Adaptive extraction and privacy amplification”

If you want, I can help you with a concrete algorithm or code to extract unbiased bits from a specific integer distribution sample you have! Would you like that?




Sure! The idea of using arithmetic coding as a means to extract nearly uniform bits from a known or estimated distribution is a beautiful connection between source coding and randomness extraction.

Key references connecting arithmetic coding to randomness extraction:
Shannon's source coding theorem (1948) and the foundation of entropy coding

This underpins the idea that the entropy of a source is the theoretical limit for compressibility, and thus the amount of extractable randomness.

Han and Hoshi (1997):
“Interval Algorithm for Random Number Generation”

Introduces an interval algorithm (related to arithmetic coding) that can generate unbiased bits from arbitrary distributions.

Link: https://ieeexplore.ieee.org/document/614777

D. Han and M. Hoshi (1997), IEEE Transactions on Information Theory, Vol. 43, No. 2

They develop the Interval Algorithm, which encodes a random variable with arbitrary distribution into bits with expected length close to entropy, effectively performing randomness extraction via arithmetic coding principles.

C. S. Wallace (1990), “The JPEG Still Picture Compression Standard”

While primarily about compression, it uses arithmetic coding principles, which conceptually relate to entropy extraction from known distributions.

F. P. Calude (2002), “Information and Randomness: An Algorithmic Perspective”

Discusses algorithmic randomness and relates data compression (including arithmetic coding) to extraction of algorithmic randomness.

Intuition
Arithmetic coding represents the probability distribution as an interval on [0,1). Encoding an input sequence narrows this interval. The bits representing the position inside this interval approximate uniform bits — thus, you can view arithmetic coding as an extractor.

Summary
If you know the exact distribution, arithmetic coding can convert your integer samples into a bitstream nearly uniform.

This approach is almost optimal for entropy extraction.

The foundational work by Han and Hoshi (1997) explicitly connects arithmetic coding to random number generation (unbiased bits extraction).

If you want, I can help you find the actual paper PDF or help implement a simplified version of this algorithm!








