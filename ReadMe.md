# ECC

Implemented `secp256k1`. Learn about ECC [here](https://andrea.corbellini.name/2015/05/17/elliptic-curve-cryptography-a-gentle-introduction/). Check the setup for `secp256k1` at [sec2-v2.pdf](https://www.secg.org/sec2-v2.pdf). 

For incryption, $c = (G^r, m \oplus Hash(Q^r))$. But for convenient, just set $Hash(Q^r) = (Q^r).x$.