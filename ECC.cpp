#include <vector>
#include <random>
#include <stdexcept>
#include <string>
#include <sstream>
#include <iostream>
#include <gmp.h>

#include "ECC.h"
#include "BigInt.hpp"

using std::string, std::vector, std::cout, std::endl;

// ------------------- Type Definition -------------------

typedef BigInt<256> T;
typedef struct Point {
    T x, y;
    Point() {}
    Point(T x, T y) : x(x), y(y) {}
} Point;
typedef vector<Point> Messages;




// ------------------- secp256k1 -------------------
// G: compressed form
static const T a = 0, b = 7;
static const T field("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F");
static const Point G(T("79BE667EF9DCBBAC55A06295CE870B07"), T("029BFCDB2DCE28D959F2815B16F81798"));

// Block size, because secp256k1 is for 256-bit, so BLOCK_SIZE = 256 / 8 = 32
const size_t BLOCK_SIZE = 32;

// ------------------- Tool function -------------------
T getRandom() {
    return T::Random() % field;
}

// as + bt = gcd(a, b)
void extendedEuclid(T a, T b, T& s, T& t, T& gcd) {
    T r_1 = b, r_0 = a;
    T s_0 = 0, s_1 = 1;
    T t_0 = 1, t_1 = 0;
    while (r_1 != 0) {
        T q = r_0 / r_1;
        T tmp = r_1;
        r_1 = r_0 - q * r_1;
        r_0 = tmp;
        tmp = s_0;
        s_0 = s_1 - q * s_0;
        s_1 = tmp;
        tmp = t_0;
        t_0 = t_1 - q * t_0;
        t_1 = tmp;
    }
    gcd = r_0;
    s = s_1;
    t = t_1;
}


// get the inverse of a mod p
// return -1 if not exist
T inverse(T a, T p) {
    T s, t, gcd;
    extendedEuclid(a, p, s, t, gcd);
    if (gcd != 1) return -1;
    return (s % p + p) % p;
}

bool isEqual(Point p1, Point p2) {
    return p1.x == p2.x && p1.y == p2.y;
}

bool isSymetric(Point p1, Point p2) {
    return p1.x == p2.x && p1.y == -p2.y;
}

Point inverse(Point p) {
    Point res;
    res.x = p.x;
    res.y = -p.y;
    return res;
}

// Multiply two points on the curve
Point multiply(Point p1, Point p2) {
    T k;
    Point res;
    if (isSymetric(p1, p2)) {
        throw std::invalid_argument("Symetric point");
    }
    if (isEqual(p1, p2)) {
        k = ((p1.x * p1.x * 3 + a) * inverse(p1.y * 2, field)) % field;
    } else {
        k = ((p2.y - p1.y) * inverse(p2.x - p1.x, field)) % field;
    }
    res.x = (k * k - p1.x - p2.x) % field;
    if (res.x < 0) res.x += field;
    res.y = (k * (p1.x - res.x) - p1.y) % field;
    if (res.y < 0) res.y += field;
    return res;
}

// use binary exponentiation
Point powerECC(Point point, T n) {
    string n_2 = n.get2String();
    Point res = point;
    size_t digit = 0; // start from the most significant bit
    while (n_2[digit] == '0') {
        digit--;
    }
    while (digit < n_2.size()) {
        res = multiply(res, res);
        if (n_2[digit] == '1') {
            res = multiply(res, point);
        }
        digit++;
    }
    return res;
}

// generate a random r in [1, p - 1] as private key
// generate a public key Q = G^r;
// take the reference
void generateKeySet(T& d, Point& Q) {
    d = getRandom();
    d = d % field;
    Q = powerECC(G, d);
}

vector<string> splitByChar(string input, char c) {
    std::stringstream ss(input);
    std::vector<std::string> result;
    while (ss.good()) {
        std::string substr;
        getline(ss, substr, c);
        result.push_back(substr);
    }
    return result;
}

// key = ( T d, T G.x, T G.y ）
void keyDecoder(string key, T& d, Point& Q) {
    std::vector<std::string> result;
    result = splitByChar(key, ';');
    if (result.size() != 3) {
        cout << result.size() << endl;
        throw std::invalid_argument("Invalid key");
    }
    d = T(result[0].c_str());
    Q.x = T(result[1].c_str());
    Q.y = T(result[2].c_str());
}

// generate a random r in [1, p - 1]
// C2 = G^r, C2 = message xor Q^r.x
// the ciphertext is (C1, C2)
void encrypt(T message, Point Q, T& C1, Point& C2) {
    T r = getRandom();
    C2 = powerECC(G, r);
    C1 = message ^ powerECC(Q, r).x;
}

// M = C1 xor C2^d
T decrypt(T C1, Point C2, T d) {
    T res = C1 ^ powerECC(C2, d).x;
    return res;
}


// ------------------- Coder -------------------
// meaningful plaintext string to T
// FIrst to binary code, then to T
T plaintextDecoder(string plaintext) {
    if (plaintext.size()>(int)BLOCK_SIZE) {
        throw std::invalid_argument("Plaintext is too long");
    }
    T res;
    res.fromMessage(plaintext);
    return res;
}

// Inverse of above
string plaintextEncoder(T M) {
    string res;
    res = M.toMessage();
    return res;
}


// binary code string to to T C1, T C2.x, T C2.y
void ciphertextDecoder(string ciphertext, T& C1, Point& C2) {
    std::stringstream ss(ciphertext);
    std::vector<std::string> result;
    while (ss.good()) {
        std::string substr;
        getline(ss, substr, ',');
        result.push_back(substr);
    }
    if (result.size() != 3) {
        throw std::invalid_argument("Ciphertext is not valid");
    }
    C1 = T(result[0].c_str());
    C2.x = T(result[1].c_str());
    C2.y = T(result[2].c_str());
}

// T C1, Point C2 to binary code string
string ciphertextEncoder(T C1, Point C2) {
    string res = "";
    res += C1.get16String();
    res += ",";
    res += C2.x.get16String();
    res += ",";
    res += C2.y.get16String();
    return res;
}

vector<string> splitByLength(const string& input, int length) {
    vector<string> substrings;
    int inputLength = input.length();

    for (int i = 0; i < inputLength; i += length) {
        std::string substring = input.substr(i, length);
        substrings.push_back(substring);
    }
    return substrings;
}


// ------------------- ECC function -------------------

string encrypt(string message, string key) {
    // set key
    T d;
    Point Q;
    keyDecoder(key, d, Q);
    Q = powerECC(G, d);

    string ciphertext = "";
    for (string submessage : splitByLength(message, (int)BLOCK_SIZE)) {
        // decode
        T M = plaintextDecoder(submessage);
        // encrypt
        T C1;
        Point C2;
        encrypt(M, Q, C1, C2);
        // encode
        ciphertext += ciphertextEncoder(C1, C2);
        ciphertext += ";";
    }
    // remove the last ';'
    ciphertext.pop_back();
    return ciphertext;
}

string decrypt(string message, string key) {
    // set key
    T d;
    Point Q;
    keyDecoder(key, d, Q);

    string plaintext = "";
    for (string block : splitByChar(message, ';')) {
        if (block.size() == 0) {
            continue;
        }
        // decode
        T C1;
        Point C2;
        ciphertextDecoder(block, C1, C2);
        // decrypt
        T M = decrypt(C1, C2, d);
        // encode
        plaintext += plaintextEncoder(M);
    }
    return plaintext;
}

string generate(bool use_default) {
    T d;
    Point Q;
    if (use_default) {
        d = "dc4f177f659f561f638d88ed9f1f60a7932bdcbb59fed59e460a7949d43547dc";
        Q = powerECC(G, d);
    } else {
        generateKeySet(d, Q);
    }
    string key = "";
    key += d.get16String();
    key += ";";
    key += Q.x.get16String();
    key += ";";
    key += Q.y.get16String();
    return key;
}

void test() {
    cout << "Test ECC" << endl;

    T c = 1024;
    T d = 2973;
    Point Q = powerECC(G, d);
    string plain = "Hellooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo";
    // T M = plaintextDecoder(plain);
    // cout << "M: " << M.get16String() << endl;
    // T C1;
    // Point C2;
    // encrypt(M, Q, C1, C2);
    // cout << "C1: " << C1.get16String() << endl;
    // cout << "C2.x: " << C2.x.get16String() << endl;

    // string ciphertext = ciphertextEncoder(C1, C2);
    // cout << "ciphertext: " << ciphertext << endl;
    // T C1_2;
    // Point C2_2;
    // ciphertextDecoder(ciphertext, C1_2, C2_2);
    // cout << "C1_2: " << C1_2.get16String() << endl;
    // cout << "C2_2.x: " << C2_2.x.get16String() << endl;

    // T M2 = decrypt(C1_2, C2_2, d);
    // cout << "M2: " << M2.get16String() << endl;
    // string plain2 = plaintextEncoder(M2);
    // cout << "plain2: " << plain2 << endl;

    cout << "-----------------------------------------------" << endl;
    string key = generate();
    cout << "key: " << key << endl;
    string ciphertext = encrypt(plain, key);
    cout << "ciphertext: " << ciphertext << endl;
    plain = decrypt(ciphertext, key);
    cout << "plain: " << plain << endl;

    cout << "-----------------------------------------------" << endl;

    T r = getRandom();
    cout << "r: " << r.get16String() << endl;
    
    cout << "Test ECC done" << endl;
}
