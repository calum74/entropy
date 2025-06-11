# Related work

Knuth and Yao

Han and Hoshi \cite{han:1995} describe an algorithm to generate arbitrary random number distributions from binary entropy, using what is essentially an arithmetic coding scheme, where the potential outputs are sorted and mapped to the interval $[0,1)$. The binary input specifies a subregion in $[0,1)$, and if the subregion falls entirely within one output bin then that output is returned. Wanatabe and Han \cite{wanatabe:2019} analysed this algorithm using a spectral analysis and found that this algorithm was indeed optimal.


Amortising

# References

wanatabe:2019
Interval Algorithm for Random Number
Generation: Information Spectrum Approach
Shun Watanabe and Te Sun Han

han:1995
Interval Algoreithm for Random Number Generation

hao:
Interval algorithm for random number generation
Authors: Te Sun Hao, M. Hoshi
IEEE Transactions on Information Theory, Volume 43, Issue 2
Pages 599 - 611
https://doi.org/10.1109/18.556116


@article{article,
author = {Han, Te Sun and Hoshi, Mamoru},
year = {1997},
month = {03},
pages = {599-611},
title = {Interval algorithm for random number generation},
volume = {43},
journal = {IEEE Transactions on Information Theory}
}



Discussion
Entropy store supports Markov processes, because there is no constraint on the sequencing of random variable generation. This means that calls to $generate\_distribution$ will be within the bounds of $H_{loss}$ irrespective of the Markov sequence.



Lemma: Algorithm \ref{alg:generate-distribution} loses $H_{loss}(n)$ entropy, where $n$ is the sum of the weights.

Proof:
$n = \sum_i w_k$

Initial entropy = $H_{in} = \log_2 n$

Final entropy = $H_{out} = H_{X} + H_Y$ = sum -p_i\logp_i + 