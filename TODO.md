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

Function $f$    $X$        $Y$     $Z = f(X,Y)$

combine         $X=Z \text{ div } m$,     $Y=Z \mod m$,    $Z=mX+Y$

sample          $Z<m$         $Z-Xm        $mX+Y$
distribute      $max(j:\sum_{i<j}w_i<Z)$.   $Z - \sum_{i<X}w_i$     $\sum_{i<Y}w_i + Z$



# Missing proofs:

Proof of Lemma 8:

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
