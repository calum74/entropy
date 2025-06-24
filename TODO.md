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

# Eval2

When generating Bernoulli variables, we see in Figure \ref{fig:bernoulli-efficiency} that the entropy efficiency of IA drops off significantly with output entropy. IA must fetch at least 2 bits per output, irrespective of the output entropy. By contrast, ES algorithms to not necessily fetch any bits to generate an output, as there may be enough entropy in the store already, so ES just shrinks the size of its store to generate an output.

<Figure>

Figure \ref{fig:bernoulli-output} compares the rate of output per input bit of ES and IA. IA cannot generate more than 0.5 outputs per input bit XX. By illustration, to generate a $Bernoulli\{#frac{1}{12}\}$ variable, EA is XX times more productive than IA.

<Figure>

