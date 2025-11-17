# Entropy Store

A C++ library for generating random integer distributions using minimal entropy.

This library is distributed as a single C++ file [src/entropy_store.hpp], and is used by copying this file into your C++ project.

A demo and tutorial of this library can be found in [tests/sample.cpp].

## Citing this work

The theoretical background to this library can be found on [ArXiv].

## Building C++

Make sure you install CMake and C++, and you check out submodules (some of the tests submodule other projects for testing purposes).

```
$ git submodule --init
$ mkdir build
$ cd build
$ cmake ..
$ make
$ make test
```

## Building the paper

Requires a full Latex installation, for example `sudo dnf install texlive-scheme-full`

```
cd paper
biber paper.bcf
pdflatex paper.tex
```
