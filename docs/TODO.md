To do:
- Read Kozen & Soloviev's paper
- Note that entropy store is not new
- Obviously talk about this in the related work
- Look at their open question and see if I answer it
- Adjust my "contribution" section
- Conclusion - is efficiency of $1 - \Theta(md^{-m})$ optimal


## Comparison with standard batching

Batching offers a way for algorithms to spread the 2-bit overhead of conversion across $k$ outputs, thereby reducing the entropy loss per output to $2/k$. Batching has an efficiency of $1-\Theta(m^{-1})$ [XX] (4.XX), which is optimal for a restart protocol where lost entropy is not preserved between batches.

Although both standard batching and ES both converge to an efficiency of 1 with an increase in buffered entropy, they do not converge at the same rate. ES converges to $1$ at a rate of $\Theta(m2^{-m})$ (see Theorem XX), which is obviously much faster than batching. It is an open question as to whether a faster rate of convergence is possible.

# XXX
$$\epsilon = \frac{2^{-k}(n-1)}{1-2^{-k}(n-1)}\log_2(2^{-k}(n-1)) + \log_2(1-2^{-k}(n-1))$$

$$ = 2^{-k}(\frac{n-1}{1-2^{-k}(n-1)})(\log_2(2^{-k}) + \log_2(n-1))) + \log_2(1-2^{-k}(n-1))$$



Reminder

# Next steps


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


📚 3. Alias Method for Sampling Discrete Distributions
Original paper:
Walker, A. J. (1974).
New fast method for generating discrete random numbers with arbitrary frequency distributions.
Electronics Letters, 10(8), 127–128.


ℹ️ 4. Optimal Approximate Sampling
Saad, Freer, Rinard & Mansinghka (2020, POPL)

Trevisan’s extractor and variants — more theoretical, but some practical constructions exist.

R. Canetti, D. Micciancio, M. Naor (2001) “Adaptive extraction and privacy amplification”

F. P. Calude (2002), “Information and Randomness: An Algorithmic Perspective”


Universal Hashing: Extract uniform bits from high-min-entropy blocks.

📘 Further Reading
Trevisan (1999), "Extractors and pseudorandom generators"

Guruswami, Umans, Vadhan (2007), "Unbalanced Expanders and Randomness Extractors from Parvaresh-Vardy Codes"

Bacher:

- [ ] This is a summary paper describing 
@article{DBLP:journals/corr/DevroyeG15,
  author       = {Luc Devroye and
                  Claude Gravel},
  title        = {Sampling with arbitrary precision},
  journal      = {CoRR},
  volume       = {abs/1502.02539},
  year         = {2015},
  url          = {http://arxiv.org/abs/1502.02539},
  eprinttype    = {arXiv},
  eprint       = {1502.02539},
  timestamp    = {Mon, 13 Aug 2018 16:46:32 +0200},
  biburl       = {https://dblp.org/rec/journals/corr/DevroyeG15.bib},
  bibsource    = {dblp computer science bibliography, https://dblp.org}
}

@misc{devroye2020randomvariategenerationusing,
      title={Random variate generation using only finitely many unbiased, independently and identically distributed random bits}, 
      author={Luc Devroye and Claude Gravel},
      year={2020},
      eprint={1502.02539},
      archivePrefix={arXiv},
      primaryClass={cs.IT},
      url={https://arxiv.org/abs/1502.02539}, 
}

Lumbrisco's PhD Thesis
[18] J´er´emie Lumbroso. Probabilistic Algorithms for Data Sreaming and Random Generation. PhD thesis, Universit´e Pierre et Marie Curie - Paris 6,
2012.

[15] Donald E. Knuth and Andrew C. Yao. Algorithms and Complexity: New
Directions and Recent Results, chapter The complexity of nonuniform random number generation, pages 357–428. Academic Press, New York, 1976.

Tidy up the name of Knuth Ervin

[7] Luc Devroye and Claude Gravel. The expected bit complexity of the Von
Neumann rejection algorithm. Statistics and Computing, 27(3):699–710,
2017.
    https://link.springer.com/article/10.1007/s11222-016-9648-z

Another citation for the optimality of Knuth-Yao
@article{horibe2003entropy,
  title={Entropy and an optimal random number transformation (Corresp.)},
  author={Horibe, Yasuichi},
  journal={IEEE Transactions on Information Theory},
  volume={27},
  number={4},
  pages={527--529},
  year={2003},
  publisher={IEEE}
}

[5] Thomas M. Cover and Joy A. Thomas. Elements of Information Theory,
Wiley, New-York, 1991.

[6] Luc Devroye. Non-Uniform Random Variate Generation. Springer-Verlag,
1986.

@article{durstenfeld1964algorithm,
  title={Algorithm 235: random permutation},
  author={Durstenfeld, Richard},
  journal={Communications of the ACM},
  volume={7},
  number={7},
  pages={420},
  year={1964},
  publisher={ACM New York, NY, USA}
}

Random article on generating perumutations
https://www.tandfonline.com/doi/abs/10.1080/00207168908803745
@article{kimble1989observations,
  title={Observations on the generation of permutations from random sequences},
  author={Kimble, Gerald W},
  journal={International Journal of Computer Mathematics},
  volume={29},
  number={1},
  pages={11--19},
  year={1989},
  publisher={Taylor \& Francis}
}

Another rejection-sampling algorithm - don't know how efficient it is?
@article{koo2014converting,
  title={Converting random bits into random numbers},
  author={Koo, Bonwook and Roh, Dongyoung and Kwon, Daesung},
  journal={The Journal of Supercomputing},
  volume={70},
  number={1},
  pages={236--246},
  year={2014},
  publisher={Springer}
}

https://dl.acm.org/doi/abs/10.1145/2669372
@article{langr2014algorithm,
  title={Algorithm 947: Paraperm---Parallel Generation of Random Permutations with MPI},
  author={Langr, Daniel and Tvrd{\'\i}k, Pavel and Dytrych, Tom{\'a}{\v{s}} and Draayer, Jerry P},
  journal={ACM Transactions on Mathematical Software (TOMS)},
  volume={41},
  number={1},
  pages={1--26},
  year={2014},
  publisher={ACM New York, NY, USA}
}

Random permutations - irrelevant?
https://scholar.google.com/scholar_lookup?doi=10.1111%2Fj.2517-6161.1968.tb00751.x
@article{plackett1968random,
  title={Random permutations},
  author={Plackett, Robin L},
  journal={Journal of the Royal Statistical Society Series B: Statistical Methodology},
  volume={30},
  number={3},
  pages={517--534},
  year={1968},
  publisher={Oxford University Press}
}


https://dl.acm.org/doi/abs/10.1145/363269.363619
@article{robson1969algorithm,
  title={Algorithm 362: Generation of random permutations [G6]},
  author={Robson, John Michael},
  journal={Communications of the ACM},
  volume={12},
  number={11},
  pages={634--635},
  year={1969},
  publisher={ACM New York, NY, USA}
}

[4] R. T. Kneusel, “Random and pseudorandom sequences,” in Random Numbers and Computers. Cham: Springer,
2018, ch. 1, pp. 1–25. DOI: 10.1007/978-3-319-77697-2_1.

[5] T. M. Cover and J. A. Thomas, Elements of Information Theory, 2nd ed. Hoboken: John Wiley & Sons, Inc.,
2006. DOI: 10.1002/047174882X

[9] P. Baidya, R. Paul, S. Mandal, and S. K. Debnath, “Efficient implementation of Knuth Yao sampler on
reconfigurable hardware,” IEEE Computer Architecture Letters, vol. 23, no. 2, pp. 195–198, Sep. 2024. DOI:
10.1109/LCA.2024.3454490.

[10] F. A. Saad, C. E. Freer, M. C. Rinard, and V. K. Mansinghka, “Optimal approximate sampling from discrete
probability distributions,” Proceedings of the ACM on Programming Languages, vol. 4, no. POPL, Jan. 2020.
DOI: 10.1145/3371104.

[11] L. Devroye, Non-Uniform Random Variate Generation. New York: Springer-Verlag, 1986. DOI: 10.1007/978-
1-4613-8643-8.

[13] W. Hörmann, J. Leydold, and G. Derflinger, Automatic Nonuniform Random Variate Generation (Statistics
and Computing). Berlin: Springer-Verlag, 2004. DOI: 10.1007/978-3-662-05946-3.

[15] J. Leydold, UNU.RAN—Universal non-uniform random number generators, Nov. 2009. [Online]. Available:
https://statmath.wu.ac.at/unuran/.

[19] J. von Neumann, “Various techniques used in connection with random digits,” in Monte Carlo Method,
ser. National Bureau of Standards Applied Mathematics Series 12, A. S. Householder, G. E. Forsythe, and
H. H. Germond, Eds., Washington, DC: U.S. Government Printing Office, Jun. 1951, ch. 13, pp. 36–38.

[22] F. A. Saad, C. E. Freer, M. C. Rinard, and V. K. Mansinghka, “The fast loaded dice roller: A near-optimal
exact sampler for discrete probability distributions,” in Proceedings of the 23rd International Conference on
Artificial Intelligence and Statistics, ser. Proceedings of Machine Learning Research, vol. 108, PMLR, 2020.

[23] A. J. Walker, “An efficient method for generating discrete random variables with general distributions,” ACM
Transactions on Mathematical Software, vol. 3, no. 3, pp. 253–256, Sep. 1977. DOI: 10.1145/355744.355749.

[24] M. D. Vose, “A linear algorithm for generating random numbers with a given distribution,” IEEE Transactions
on Software Engineering, vol. 17, no. 9, pp. 972–975, Sep. 1991. DOI: 10.1109/32.92917.

[25] T. Uyematsu and Y. Li, “Two algorithms for random number generation implemented by using arithmetic
of limited precision,” IEICE Transactions on Fundamentals of Electronics, Communications and Computer
Sciences, vol. 86, no. 10, pp. 2542–2551, Oct. 2003.

[26] J. Lumbroso, “Optimal discrete uniform generation from coin flips, and applications,” arXiv, no. 1304.1916,
Apr. 2013. DOI: 10.48550/arXiv.1304.1916.

[27] M. Huber and D. Vargas, “Optimal rolling of fair dice using fair coins,” arXiv, no. 2412.20700, Dec. 2024.
DOI: 10.48550/arXiv.2412.20700.

[28] F. A. Saad and W. Lee, “Random variate generation with formal guarantees,” Proceedings of the ACM on
Programming Languages, vol. 9, no. PLDI, Jun. 2025, Forthcoming.

[30] L. Devroye and C. Gravel, “Random variate generation using only finitely many unbiased, independently and
identically distributed random bits,” arXiv, no. 1502.02539v6, Nov. 2020. DOI: 10.48550/arXiv.1502.02539.

[34] S. Zimmerman, “An optimal search procedure,” The American Mathematical Monthly, vol. 66, no. 8, pp. 690–
693, Oct. 1959. DOI: 10.1080/00029890.1959.11989389.

[35] J. E. Norman and L. E. Cannon, “A computer program for the generation of random variables from any
discrete distribution,” Journal of Statistical Computation and Simulation, vol. 1, no. 4, pp. 331–348, 1972.
DOI: 10.1080/00949657208810026.

[38] P. Elias, “The efficient construction of an unbiased random sequence,” Annals of Mathematical Statistics,
vol. 43, no. 3, pp. 865–870, Jun. 1972. DOI: 10.1214/aoms/1177692552.

[39] J. Abrahams, “Generation of discrete distributions from biased coins,” IEEE Transactions on Information
Theory, vol. 42, no. 5, pp. 1541–1546, Sep. 1996. DOI: 10.1109/18.532895.

[40] J. R. Roche, “Efficient generation of random variables from biased coins,” in Proceedings of the IEEE
International Symposium on Information Theory, Piscataway: IEEE Press, 1991, pp. 169–169. DOI: 10.1109/
ISIT.1991.695225.

[42] D. Kozen, “Optimal coin flipping,” in Horizons of the Mind. A Tribute to Prakash Panangaden: Essays
Dedicated to Prakash Panangaden on the Occasion of His 60th Birthday, ser. Lecture Notes in Computer
Science, vol. 8464, Cham: Springer, 2014, pp. 407–426. DOI: 10.1007/978-3-319-06880-0_21.

43] S.-I. Pae, “A generalization of Peres’s algorithm for generating random bits from loaded dice,” IEEE Transactions on Information Theory, vol. 61, no. 2, pp. 751–757, Feb. 2015. DOI: 10.1109/TIT.2014.2381223

44] S.-I. Pae, “Binarization trees and random number generation,” IEEE Transactions on Information Theory,
vol. 66, no. 4, pp. 2581–2587, Apr. 2020. DOI: 10.1109/TIT.2019.2962480

++++ [45] D. Kozen and M. Soloviev, “Coalgebraic tools for randomness-conserving protocols,” in Proceedings of the
17th International Conference on Relational and Algebraic Methods in Computer Science, ser. Lecture Notes
in Computer Science, vol. 11194, Cham: Springer, 2018, pp. 298–313. DOI: 10.1007/978-3-030-02149-8_18.

@article{KOZEN2022100734,
title = {Coalgebraic tools for randomness-conserving protocols},
journal = {Journal of Logical and Algebraic Methods in Programming},
volume = {125},
pages = {100734},
year = {2022},
issn = {2352-2208},
doi = {https://doi.org/10.1016/j.jlamp.2021.100734},
url = {https://www.sciencedirect.com/science/article/pii/S2352220821000973},
author = {Dexter Kozen and Matvey Soloviev},
keywords = {Randomness, Entropy, Protocol, Reduction, Transducer, Coalgebra},
abstract = {We propose a coalgebraic model for constructing and reasoning about state-based protocols that implement efficient reductions among random processes. We provide basic tools that allow efficient protocols to be constructed in a compositional way and analyzed in terms of the tradeoff between state and loss of entropy. We show how to use these tools to construct various entropy-conserving reductions between processes.}
}


47] Q. F. Stout and B. Warren, “Tree algorithms for unbiased coin tossing with a biased coin,” The Annals of
Probability, vol. 12, no. 1, pp. 212–222, Feb. 1984. DOI: 10.1214/aop/1176993384.

++++ S. Pae, M. C. Loui, Optimal random number generation from a biased
coin, in: Proc. 16th ACM-SIAM Symposium on Discrete Algorithms, Vancouver, Canada, 2005, pp. 1079–1088.

++++ @article{gryszka2021biased,
  title={From biased coin to any discrete distribution},
  author={Gryszka, Karol},
  journal={Periodica Mathematica Hungarica},
  volume={83},
  number={1},
  pages={71--80},
  year={2021},
  publisher={Springer}
}

1. P. Elias, The efficient construction of an unbiased random sequence. Ann. Math. Statist. 43(3), 865–870
(1972)

T. Itoh, Simulating fair dice with biased coins. Inf. Comput. 126(1), 78–82 (1996)

 D. E. Knuth, A. C.-C. Yao, The Complexity of Nonuniform Random Number Generation. In: J. F. Traub
(eds), Algorithms and Complexity: New Directions and Recent Results Proceedings of a Symposium, New
York, NY, Carnegie-Mellon University, Computer Science Department, pp. 357–428 , Academic Press,
Cambridge (1976)

6. M. C. Loui, S.-I. Pae, Optimal random number generation from a biased coin. In: Proceedings of the
sixteenth annual ACM-SIAM symposium on Discrete algorithms. Society for Industrial and Applied
Mathematics, p. 1079–1088 (2005)

9. J. von Neumann, Various techniques used in connection with random digits. John von Neumann Collected
Works 5, 768–770 (1963)

14. Q. F. Stout, B. Warren, Tree algorithms for unbiased coin tossing with a biased coin. Ann. Probab. 12(1),
212–222 (1984)

[22] R. Durrett, Probability: Theory and Examples, Cambridge University
Press, 2010.

@article{jerrum2024fundamentals,
  title={Fundamentals of partial rejection sampling},
  author={Jerrum, Mark},
  journal={Probability Surveys},
  volume={21},
  pages={171--199},
  year={2024},
  publisher={The Institute of Mathematical Statistics and the Bernoulli Society}
}