# Prove complexity class

## Theorem

The efficiency of an entropy store of $m$ bits is $1 - \Theta(m2^{-m})$.

## Proof

This follows from Theorem XX, where we keep $n$ constant. The number of bits $m$ required by the ES is $\log_2s_{max}$, where $s_{max}$ is the maximum value in the store. The maximum value is $b$ times the minimum value $s_{min}$, therefore $m = \log_2(s_{max}) = \log_2(bs_{min})$, therefore $s_{min} = \frac{2^m}{b}$.

From the definition of $p = \frac{n-1}{s_{min}} = \frac{b(n-1)}{2^m} = 2^{-m}b(n-1) = \Theta(2^{-m})$.

$$-\log_2 p = -\log_2 (2^{-m}b(n-1))$$

$$= -\log_2 (2^{-m}) - \log_2(b(n-1)) $$
$$= m - \log_2(b(n-1)) = \Theta(m)$$

We also have

$$1-p = \Theta(1)$$

and an the expansion of $\log(1+p)$ yields

$$\frac{p}{1+p} \le \log (1+p) \le p$$

from which it follows that

$$-\log_2(1-p) = \Theta(p) = \Theta(2^{-m})$$

From our definition of $\epsilon_{max}$, 

$$\epsilon_{max} = -\frac{p}{1-p}\log_2p - \log_2(1-p) $$

$$= \frac{\Theta(2^{-m})}{\Theta(1)}\Theta(m) + \Theta(2^{-m})$$

$$ = \Theta(m2^{-m}) + \Theta(2^{-m}) $$
$$ = \Theta(m2^{-m})$$

The efficiency

$$\eta_{min} = \frac{H_{out}}{H_{out} + \epsilon} = 1 - \frac{\epsilon}{H_{out} + \epsilon} = 1 - \frac{\Theta(m2^{-m})}{\Theta(1)} = 1 - \Theta(m2^{-m})$$

QED

## Corollary

Asymptotic efficiency of 1.

## Comparison with batching

Batching is a technique whereby many outputs are generated at once, which has a better efficiency than generating outputs one at a time.
Whilst batching and entropy store both have an asymptotic efficiency of 1, the question is how quickly the efficiency converges to 1.

Kozen and Soloviev [XX] give a mathematical analysis of batching, where the efficiency of batching is $1-\Theta(m^{-1})$ for $m$ bits of stored entropy, so converges to $1$ at a rate of $\Theta(m^{-1})$. ES converges to $1$ at a rate of $\Theta(m2^{-m})$ (see Theorem XX), which is obviously converges much more quickly. It is an open question as to whether a faster rate of convergence is possible.

