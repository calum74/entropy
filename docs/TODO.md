# The Bug

I think the answer is that when you read an "unlikely" input, it creates a much larger uniform distribution, which is used for much longer. This biases the output towards unlikely inputs, which means that "reading" entropy from biassed sources is not correct.

However, only certain distributions (such as Bernoulli output) expose this error.

Basically, it means we can't read from arbitrary distributions as we were doing, and we need to delete the code.

# Thoughts

- [ ] Talk about "pairs (x,y)" and the bijection more.

## More ideas


# Repartitioning

- Talk about repartitioning
- Talk about needing to bootstrap the entropy store from binary.
- Define "generation" and "extraction"

Rename algorithms?
 - conv_uu_to_u
 - conv_u_to_uu
 - conv_u_to_ub
 - conv_ub_to_u
 - conv_u_to_u
 - conv_ubin_to_u

- Have a table of conversions?

At the heart of ESA is the ability to manipulate a uniform variable, implemented as a pair of integers (its value and its size). To do this we can \em partition \em the range $[0,n)$ into smaller subranges, where each subrange $i$ has a uniform distribution $[0,w_i)$, and the subranges are distributed according to a weighted distribution.

    Picture here

We have two special cases of this. The first is where the range is a composite number $nm$, in which case we can partition the range $[0,nm)$ into $n$ equal ranges of $m$ as follows:

    Picture here

This allows us to partition a uniform variable $Uniform\{nm\}$ into two independent

The second case is where we have two subranges, and we can partition the range $[0,n)$ into subranges $n$ and $n-m$ as follows:


Crucially, these partitionings are reversible, meaning you can take two 



Idea: Under a particular workload, one of the divmods can be avoided



Fundamentally, the size of the distribution leaks information and causes correlations.


Set up the README



- sample.cpp - how to use the library

Paper:
- Continue proof of $f_{sample}$ and delete other proofs.
- Put brace underneath showing the other distribution

Definition: $f_{distribute}: [0,n) \rightarrow [0,k) \times [0,w_i)$ is defined as $f_{distribute}(Z) = (\max i : s_{i+1} < Z, Z - s_i)$.

Lemma: If $X \sim Weighted\{w_0, ..., w_{k-1}\}$ and $Y \sim Uniform\{w_X\}$, then $f^{-1}_{distribute}(X,Y) = s_X + Y$.

Proof:
$f^{-1}_{distribute}(f_{distribute}(Z)) = f^{-1}_{distribute}(\max i : s_{i+1} < Z, Z - s_i) = s_i + Z - s_i = Z$. $f_{distribute}(f^{-1}_{distribute}(X,Y)) = f_{distribute}(s_X+Y) = (\max i : s_{i+1} < s_X+Y, s_X + Y - s_i) = (X,Y)$.

Lemma: If $Z \sim Uniform\{n\}$ then $X \sim Weighted\{w_0, ..., w_{k-1}\}$ and $Y \sim Uniform \{w_X\}$. They are independent.






Lemma: $f_{distribute}$ is a bijection.


Unused references:
Arxiv:
    Huber \cite{huber2024optimalrollingfairdice}
    \cite{baidya24}
    \cite{saad2025}
    \cite{roche91}

Unclassified:
    \cite{norman1972computer}
    \cite{stout1984tree}
    \cite{Hoeffding1994}
    Randomness recycler: \cite{fill2000randomness}
    \cite{norman1972computer}

idiv:
Cite as {Abel19a}

Treat Bernoulli distribitions separately.
Maybe mention Bernoulli distributions can be generated in O(1) space.

Data on CPU instructions shows 

            Alder Lake-P     
32-bit      10-15
64-bit      14-18

Random Number Generation and Monte Carlo Methods" by James E. Gentle (2nd ed., 2003)
Chapter 1 introduces methods for generating uniform deviates over a finite set.

Section 1.7: Combining Generators delves into techniques—such as forming linear combinations like 
𝑋
+
𝑚
𝑌
X+mY or 
𝑚
𝑋
+
𝑌
mX+Y—to produce new uniform variates from independent uniform sources 
arxiv.org
+15
mason.gmu.edu
+15
link.springer.com
+15
link.springer.com
+4
link.springer.com
+4
link.springer.com
+4
.

Gentle discusses both the theory behind these constructions and practical implementation details.



TODO: Ef(X)  = f(EX) if f is a convex function. This is called Jensen's inequality.





We can split up a $Z \sim Uniform\{n\}$ into two independent distributions $Y$ and $Z$.

\begin{equation}
    123
\end{equation}


\[
\overbrace{
\underbrace{a_0 ... a_{m-1}}_m\underbrace{a_m ... a_{n-1}}_{n-m}}^n
\]

Splitting up $s$:

XX Use r as the remainder

\[
\overbrace{
    \underbrace{
        \underbrace{a_0 ... a_{m-1}}_m
        \underbrace{a_n ... a_{2m-1}}_m
        ...        
        \underbrace{a_{n(m-1)} ... a_{nm-1}}_m
        }_{nm}
    \underbrace{a_{nm} ... a_{nm+r-1}}_r
    }^{nm+r}
\]



Paper:
- Background work: More about batching, particularly multiplication, FLDR, 
    More about Bernoulli distributions
- Contributions:
  - Layout the table a bit better
  - Flexibility in reading entropy, particularly from Markov processes
  - Efficient for outputting exact Bernoulli distributions
  - Achieve entropy-optimality
  - Generates precise output distributions without floating point approximations

Algorithms:
- Define $f_{sample}$ and its inverse.
- Note that $f_{distribute}$ generalises $f_{combine}$ and $f_{sample}$.
- Proof of $f^{-1}_{distribute}$

*Definition:* $f_{sample}: [0,n) \rightarrow \{0,1\}\times\mathbb{N}_0:$

$$
f_{sample}(X) = 
\begin{cases}
(0,X), & \text{if } X < m \\
(1,X-m), & \text{otherwise}
\end{cases}
$$

*Lemma:* $f_{sample}$ is a bijection with inverse 

$$
f^{-1}_{sample}(Y,Z) = 
\begin{cases}
Z, & \text{if } Y=0 \\
Z+m, & \text{otherwise}
\end{cases}
$$

*Lemma:* If $f_{sample}(X) \rightarrow (Y, Z)$, and $X \sim Uniform\{n\}$, then $Y \sim Bernoulli\{\frac{m}{n}\}$ and $Z \sim Uniform\{w\}$ for some $w$.

*Lemma:* If $f^{-1}_{sample}(Y,Z) \rightarrow X$, and $Y \sim Bernoulli\{\frac{m}{n}\}$ and $Z \sim Uniform\{w\}$, then $X \sim Uniform\{n\}$.


*Definition:* $f_{combine}: [0,n) \times [0,m) \rightarrow [0,mn): f_{combine}(X,Y) = mX+Y$.

*Lemma:* $f_{combine}$ is a bijection with inverse $f^{-1}_{combine}(Z) = (\lfloor \frac{Z}{m} \rfloor, Z \mod m)$.



*Lemma:* $f_{distribute}$ is a bijection.

The inverse of $f_{distribute}$ is given by $f^{-1}_{distribute}(Y,Z) = \sum_{i<Y}w_i + Z$

## Three bijections

The functions $f_{combine}$, $f_{sample}$, $f_{distribute}$ provide different ways of splitting up a uniform distribution of size $n$ into two smaller distributions containing the same entropy as the original distribution.

$f_{divide,m}$:         $X=Z \text{ div } m \sim Uniform\{\frac{n}{m}\}$,     $Y=Z \mod m \sim Uniform\{m\}$,    $Z=mX+Y \sim Uniform\{n\}$.

$f_{sample,m}$:          $X = Z \ge m \sim Bernoulli\{\frac{m}{n}\}$,        $Y=Z-Xm \sim Uniform$,        $Z=mX+Y \sim Uniform\{n\}$.

$f_{distribute,w}$:      $X=max(j:\sum_{i<j}w_i<Z) \sim Weighted$,   $Y=Z - \sum_{i<X}w_i \sim Uniform\{w_i\}$,     $Z=\sum_{i<Y}w_i + Z \sim Uniform\{n\}$.

$f_{divide}$ splits a uniform distribution of size $n$ into a uniform distribution of size $m$ and uniform distribution of size $\frac{n}{m}$. $m$ must divide into $n$ exactly.

$f_{sample}$ splits a uniform distribution of size $n$ into a Bernoulli distribution and a uniform distribution. $f_{sample}$ is used as the basis of Lumbrusco's fast dice roller and von Neumann's rejection sampling algorithms.

$f_{distribute}$ splits a uniform distributions into a weighted distribution and a uniform distribution. 

Because these functions are bijections, we have the inverse functions $f^{-1}_{divide}$ which combines two uniform distributions, $f^{-1}_{sample}$ and $^{-1}_{distribute}$ which combines smaller uniform distributions with entropy to create a larger uniform distribution.

# Missing proofs

*Definition:* $f_{divide,m}(Z) = (Z \text{ div } m, Z \text{ mod } m)$.

*Lemma:* $f^{-1}_{divide,m}(X,Y) = mX+Y$.

Proof: $f^{-1}_{divide}(f_{divide}(Z)) = f^{-1}_{divide}(Z \text{ div } m, Z \text{ mod } m) = m(Z\text{ div } m) + (Z \text{ mod } m) = Z$. $f_{divide}(f^{-1}_{divide}(X,Y)) = f_{divide}(mX+Y) = ((mX+Y)\text{ div } m, (mX+Y)\text{ mod } m) = (X,Y)$.

*Lemma:* If $f_{divide}: Z \mapsto (X,Y)$, and $Z \sim Uniform\{n\}$, then $X \sim Uniform\{\frac{n}{m}\}$ and $Y \sim Uniform\{m\}$, and $X$ and $Y$ are independent.

*Lemma:* If $f^{-1}_{divide}: (X,Y) \mapsto Z$, and $X \sim Uniform\{\frac{n}{m}\}$ and $Y \sim Uniform\{m\}$, then $Z \sim Uniform\{n\}$.


*Definition:* $f_{sample,m}(Z) = (Z \ge m, Z-(Z\ge m)m)$. This is shorthand for saying that $Z\ge m \equiv 1$ if $Z\ge m$, $0$ otherwise.

*Lemma:* $f^{-1}_{sample,m}(X,Y) = mX+Y$

Proof: $f^{-1}_{sample}(f_{sample}(Z)) = f^{-1}_{sample}(Z \ge m, Z-(Z\ge m)m) = m(Z \ge m) + Z-(Z\ge m)m = Z$. $f_{sample}(f^{-1}_{sample}(X,Y)) = f_{sample}(mX+Y) = ((mX+Y) \ge m, (mX+Y)-((mX+Y)\ge m)m) = (X, Y).$

*Definition:* $f_{distribute}(Z) = (\max j : \sum_{i<j} w_i < Z, Z - \sum_{i<j} w_i)$

*Lemma:* $f^{-1}_{distribute} (X,Y) = \sum_{i<X}w_i + Y$

Proof: $f^{-1}_{distribute}(f_{distribute}(Z)) = f^{-1}_{distribute}(\max j : \sum_{i<j} w_i < Z, Z - \sum_{i<j} w_i) = Z - \sum_{i<j} w_i + \sum_{i<j} w_i = Z.$

$f_{distribute}(f^{-1}_{distribute}(X,Y)) = f_{distribute}(\sum_{i<X}w_i + Y) = (\max j : \sum_{i<j} w_i < (\sum_{i<X}w_i + Y), (\sum_{i<X}w_i + Y) - \sum_{i<j} w_i) = (\max j : \sum_{i<j} w_i < (\sum_{i<X}w_i), Y) = (X,Y).$






# Contributions

Outline: Three bijections f_{sample}, f_{distribute} and f_{divide} give us a way to reorganise a uniform distribution into smaller uniform and Bernoulli distributions. Because they are bijections, then are invertible and therefore entropy-preserving.





A new way to extract 

A new way to generate Bernoulli variables, yielding an entropy output 


Hard-coded division algorithm




template<int N>
struct divider
{
    static std::pair<int,int> divmod(int x)
    {
        int div = (x * inverse<N>::value)>>32;
        auto mod = x - div*N;
        return {div, mod};
    }
};






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




🎯 1. Trevisan's Extractor (1999)
✨ Key Idea:
Uses hard functions and pseudorandom generators (PRGs) to avoid needing a lot of seed randomness. It's incredibly seed-efficient.

🔧 High-Level Structure:
Input: A weak random source 
𝑥
∈
{
0
,
1
}
𝑛
x∈{0,1} 
n
  with high min-entropy (say ≥ 
𝑘
k).

Seed: A short truly random string 
𝑠
∈
{
0
,
1
}
𝑑
s∈{0,1} 
d
 .

Output: 
𝑚
m nearly uniform bits.

How it works:
Trevisan's method relies on the fact that if you have a hard-to-compute function, you can use it to fool a predictor, thus simulating randomness.

It constructs 
𝑚
m output bits by:

Using the seed 
𝑠
s to select subsets of bits from the input 
𝑥
x,

Applying a hardcore predicate (based on a one-bit extractor or error-correcting code) on each subset to generate an output bit.

Building blocks:
Error-Correcting Codes (ECC): To spread out entropy.

Hard-Core Predicates: Securely extract a single bit.

Combinatorial Designs: Used to select input subsets for each bit while minimizing overlap (to ensure independence).

Complexity:
Seed length 
𝑑
=
𝑂
(
log
⁡
2
𝑛
)
d=O(log 
2
 n)

Output length 
𝑚
=
𝑘
Ω
(
1
)
m=k 
Ω(1)
  (polynomial fraction of entropy)

🎯 2. Guruswami–Umans–Vadhan (GUV) Extractor (2007)
✨ Key Idea:
Constructs explicit, high-rate extractors using algebraic techniques, especially list-decodable codes and lossless condensers.

🔧 High-Level Structure:
It uses algebraic function fields, Reed-Solomon codes, and polynomial evaluation to efficiently condense and extract entropy.

The GUV extractor first condenses the weak source into a smaller, high-entropy source using a lossless condenser, then extracts using a simple extractor like a universal hash function.

Building blocks:
Lossless Condensers: Compress the entropy without losing it (i.e., 
(
𝑛
,
𝑘
)
→
(
𝑛
′
,
𝑘
)
(n,k)→(n 
′
 ,k), with 
𝑛
′
<
𝑛
n 
′
 <n).

Universal Hashing: Extract uniform bits from high-min-entropy blocks.

List-Decodable Codes: Help guarantee low statistical distance from uniform.

Properties:
Seed length: 
𝑑
=
𝑂
(
log
⁡
𝑛
)
d=O(logn)

Extracts almost all of the entropy: 
𝑚
≈
𝑘
m≈k

Efficient and explicit (i.e., constructible in poly time)

Works for high min-entropy (e.g., 
𝑘
≥
polylog
(
𝑛
)
k≥polylog(n))

🔬 Key Comparison
Feature	Trevisan’s Extractor	GUV Extractor
Seed length	
𝑂
(
log
⁡
2
𝑛
)
O(log 
2
 n)	
𝑂
(
log
⁡
𝑛
)
O(logn)
Efficiency	Less practical, more theoretical	Highly efficient
Output quality	Good (polynomial entropy)	Almost optimal (close to 
𝑘
k)
Tools used	ECC, combinatorial designs	Lossless condensers, polynomials
Min-entropy required	
𝑘
=
polylog
(
𝑛
)
k=polylog(n)	
𝑘
=
polylog
(
𝑛
)
k=polylog(n)
Seeded?	Yes	Yes

🤖 Why Are These Useful?
These extractors are crucial in:

Cryptography (e.g., key generation from weak sources),

Distributed computing,

Secure hardware,

Simulating randomness in deterministic environments.

They are especially useful when:

You have a source of weak randomness (like a biased coin),

You can afford a short uniform seed (from hardware RNG or external source),

You need many high-quality random bits.

📘 Further Reading
Trevisan (1999), "Extractors and pseudorandom generators"

Guruswami, Umans, Vadhan (2007), "Unbalanced Expanders and Randomness Extractors from Parvaresh-Vardy Codes"

Lecture notes by Salil Vadhan and Luca Trevisan





Certainly! Split-source extractors are a fundamental concept in randomness extraction, especially in scenarios where two independent sources of weak randomness are available. They are a subclass of multi-source extractors, and are often the simplest and most studied case.

🔖 Key References on Split-Source Extractors
Here are the foundational and influential papers:

🔹 1. "Extracting Randomness from Two Independent Sources"
Authors: Anup Rao
Conference: STOC 2006
Link: DOI

First construction of a two-source extractor that works with polylogarithmic entropy from each source.

Introduces key techniques that influenced future extractors.

🔹 2. "A New Condenser/Extractor and its Applications"
Authors: Anup Rao
Conference: CCC 2008
Link: DOI

Gives improved constructions of two-source and split-source extractors.

Uses graph-based methods to amplify randomness.

🔹 3. "Explicit Two-Source Extractors and Resilient Functions"
Authors: Gil Cohen
Conference: CCC 2015
Link: arXiv

First explicit extractor for two sources with logarithmic min-entropy.

🔹 4. Survey: "Extractors and Pseudorandom Generators"
Author: Salil Vadhan
Book Chapter: Handbook of Randomized Computing, 2001
PDF Link

Excellent overview of extractors, including split-source and multi-source extractors.

Explains connections to dispersers, expanders, and condensers.

🔹 5. "An Optimal-Entropy Lossless Condenser"
Authors: Dvir, Gabizon, and Wigderson
Conference: IEEE CCC 2007
Link: DOI

Related work: discusses how to condense randomness from multiple sources, which is useful in building extractors.

📘 Definition: Split-Source Extractor
A split-source extractor is a function:

Ext
:
{
0
,
1
}
𝑛
×
{
0
,
1
}
𝑛
→
{
0
,
1
}
𝑚
Ext:{0,1} 
n
 ×{0,1} 
n
 →{0,1} 
m
 
such that if 
𝑋
X and 
𝑌
Y are two independent sources with min-entropy 
𝑘
k, then:

Ext
(
𝑋
,
𝑌
)
≈
𝜖
Uniform
(
{
0
,
1
}
𝑚
)
Ext(X,Y)≈ 
ϵ
​
 Uniform({0,1} 
m
 )
That is, even though 
𝑋
X and 
𝑌
Y are not uniformly random individually, the output is statistically close to uniform, assuming enough entropy.

💡 Applications
Cryptography (especially privacy amplification)

Distributed systems where two parties have independent randomness

Secure multiparty computation

