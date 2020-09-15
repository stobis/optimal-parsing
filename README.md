# optimal-parsing
Implementation of Optimal Parsing method for dynamic dictionary based data compression. 

## Contents
The project contains a sample implementation of so-called Flexible Parser described in:

Yossi Matias and Suleyman Cenk Sahinalp.
_On the Optimality of Parsing in Dynamic Dictionary Based Data Compression. 1998_ 
([link](http://theory.stanford.edu/~matias/papers/fp.pdf))

which turns out to be optimal in specific class of dictionary-based data compressors.

The reference compression algorithm chosen for testing validity and usage was the Lempel–Ziv–Welch (LZW), as described in paper above. 

For just an overview of the idea behind this work you can also see _OP_description_pl.pdf_ (polish only).

## Cloning and building instructions
Cloning this repo
```shell
    git clone https://github.com/stobis/optimal-parsing.git
    cd optimal-parsing
```

Build and run test app
```shell
    mkdir build
    cd build
    cmake ..
    make
    ./optimal_parsing
```

In order to use your own test cases you have to modify existing runner located in `app/app.cpp` or create separate one. 
All necessary header files are in `include/` directory.
