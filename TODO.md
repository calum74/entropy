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
