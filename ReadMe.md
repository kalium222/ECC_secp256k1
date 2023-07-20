# ECC

This is an implementation of the ECC (Advanced Encryption Standard) cipher. Learn about ECC [here](https://andrea.corbellini.name/2015/05/17/elliptic-curve-cryptography-a-gentle-introduction/). 

## Setup

On a ECC curve, the points and the defined operation form a group.  For key, use a random number $d$ as private key, and generate public key as $Q = dG$, where $G$ is the generator. For plantext that incoded as a point $M$, encrypt it as a point pair $(rG, M(rQ))$, where $r$ is a random number generated at runtime. When decrypting, calculate $M(rQ) - d(rG) = M$.

We used `secp256k1` for curve. Check the set up at [sec2-v2.pdf](https://www.secg.org/sec2-v2.pdf).

## Encoding and Decoding

To transform a string to a point, we use the following method. First, we transform the string to a hexdecimal integer `m`. To make sure we can find a point on the curve, set $x = Km + j$, where K is a normal interger such as 20, and $j = 1, 2, ..., K-1$. This is called `Kobits` methed. Then, we use the Cipolla's algorithm to calculate the quadratic residue, $y$. Check this on [wiki](https://en.wikipedia.org/wiki/Cipolla%27s_algorithm).

But this can not guarantee the uniqueness of the point because bot $y$ and $-y$ can be the corresponding $y$ axis. We choose the smaller one. This can guarantee the uniqueness of the point.


## Compilation

Run `make`. The compiled binary can be found as `g2`.

`make clean` removes the binary file.

## Key

Private key is a random hexdecimal integer. Public key is a point on the curve. We decode the public key to a hexdecimal integer and put both of them into the key.txt.

## Alphabet

The alphabet is `abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789,.;?!()`.