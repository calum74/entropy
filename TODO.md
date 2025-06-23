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

Figure \ref{fig:uniform} shows the calculated entropy loss (toll) for generating uniform integers, comparing entropy store (ES) methods with unbatched von Neumann (vN) and Knuth-Yao (KY) samplers. Note that these graphs show exact calculation, not simulation. We can observe that for vN and KY, the entropy loss is heavily dependent on the number being generated. ES is not dependent on the generated integer, but loses efficiency at high $n$ as it approaches its store size of $N$. We see that the observed entropy loss is less than the calculated upper bound $\epsilon$, but does wobble depending on the previous size of the store. We can also confirm that increasing the store size to 64-bit reduces the entropy loss.

Figure \ref{fig:shuffle} shows the calculated entropy loss for shuffling a deck of $n$ cards using the Fisher-Yates algorithm \cite{fisheryates}. The overall output is smoother because more numbers are fetched. Again, we observe that ES is orders of magitude more entropy efficient than vN or KY.

Figure \ref{fig:bernoulli} shows the calculated entropy loss for Bernoulli ($p=\frac{1}{n}$) outputs when using aliasing, interval algorithm and ES. ES is able to generate exact outputs without truncation or approximation, at a much greater efficiency.

