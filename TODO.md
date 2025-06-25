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

*Definition:* $f_{combine}: [0,n) \times [0,m) \rightarrow [0,mn): f_{combine}(X,Y) = mX+Y$.

*Lemma:* $f_{combine}$ is a bijection with inverse $f^{-1}_{combine}(Z) = (\lfloor \frac{Z}{m} \rfloor, Z \mod m)$





# Contributions

A new way to extract 

A new way to generate Bernoulli variables, yielding an entropy output 
