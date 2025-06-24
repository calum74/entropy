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

Evaluation
- Look at the achived efficiency vs theoretical on a log-log graph
- Uniform distribution
- Card shuffling

Graphs:
1. Efficiency of uniform variate
2. Efficiency of card shuffling
3. Entropy loss of uniform variable generation
    - X: Size of uniform (up to 2^31), log
    - Y: Log entropy loss
    - Series:
        - von Neumann
        - KnuthYau
        - $\epsilon$ 32-bit store
        - $\epsilon$ 64-bit store
        - ES 32-bit actual
        - ES 64-bit actual
4. Entropy loss of card shuffling
    X: Size of deck
    Y: Log entropy loss
    Series:
        - von Neumann
        - KnuthYau
        - $\epsilon$
        - Entropy Store
5. Entropy loss for other workloads
    - X: Numerator 1-1000
    - Y: Entropy loss (log bits)
    - Series:
        - epsilon 32-bit
        - ES 32-bit
        - epsilon 64-bit
        - ES 64-bit
        - KY aliasing
        - Need an arithmetic method.

Simulate bits needed for HY.

# Evaluation


# Conclusion

Giving entropy conversion algorithms access to an entropy store can greatly increase their efficiency, even beyond "optimal" algorithms like Knuth-Yao or the Interval algorithm.




A major drawback with the algorithmsOne drawback

There are other advantages to the entropy store algorithms as well, for example by being able to generate exact Bernoulli distributions without truncation


how entropy store algorithms can overcome the theoretical limits of entropy efficiency when generating discrete distributions


Entropy Store algorithms 

Further work
