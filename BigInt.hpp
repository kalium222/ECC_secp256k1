#ifndef _BIGINT_HPP_
#define _BIGINT_HPP_

#include <gmp.h>
#include <string>

// ------------------- BigInt -------------------
// This template class is a wrapper of 256 bits mpz_t for cpp
template <int NumBits>
class BigInt {
private:
    mpz_t value;

    // Tool functions to compute quadratic residue
    
    // treat w like imaginary unit
    static void extend_mul(
        BigInt& resultx, BigInt& resulty,
        const BigInt& x1, const BigInt& y1,
        const BigInt& x2, const BigInt& y2,
        const BigInt& w_square, const BigInt& p
    ) {
        resultx = (x1*x2 + y1*y2*w_square) % p;
        if (resultx < 0) resultx += p;
        resulty = (x1*y2 + x2*y1) % p;
        if (resulty < 0) resulty += p;
    }

    // use binary method to compute a^n mod p
    static void extend_pow(
        BigInt& resultx, BigInt& resulty,
        const BigInt& x, const BigInt& y,
        const BigInt& n, const BigInt& w_square, const BigInt& p
    ) {
        resultx = 1;
        resulty = 0;
        BigInt exp = n;
        BigInt tmpx = x, tmpy = y;
        while (exp > 0) {
            if (exp % 2 == 1) {
                extend_mul(resultx, resulty, resultx, resulty, tmpx, tmpy, w_square, p);
            }
            extend_mul(tmpx, tmpy, tmpx, tmpy, tmpx, tmpy, w_square, p);
            exp /= 2;
        }
    }

public:
    BigInt() {
        mpz_init2(value, NumBits);
    }

    BigInt(const char* str, int base = 16) {
        mpz_init2(value, NumBits);
        mpz_set_str(value, str, base);
    }

    BigInt(const int i) {
        mpz_init2(value, NumBits);
        mpz_set_si(value, i);
    }

    BigInt(const mpz_t& other) {
        mpz_init2(value, NumBits);
        mpz_set(value, other);
    }

    BigInt(const BigInt& other) {
        mpz_init2(value, NumBits);
        mpz_set(value, other.value);
    }

    ~BigInt() {
        mpz_clear(value);
    }

    // ------------------- Operator Overloading -------------------
    BigInt& operator=(const BigInt& other) {
        if (this != &other) {
            mpz_set(value, other.value);
        }
        return *this;
    }

    BigInt& operator=(const int i) {
        mpz_set_si(value, i);
        return *this;
    }

    BigInt operator+(const BigInt& other) const {
        BigInt result;
        mpz_add(result.value, value, other.value);
        return result;
    }

    BigInt operator-(const BigInt& other) const {
        BigInt result;
        mpz_sub(result.value, value, other.value);
        return result;
    }

    BigInt operator-() const {
        BigInt result;
        mpz_neg(result.value, value);
        return result;
    }

    BigInt operator*(const BigInt& other) const {
        BigInt result;
        mpz_mul(result.value, value, other.value);
        return result;
    }

    BigInt operator*(const int i) const {
        BigInt result;
        mpz_mul_si(result.value, value, i);
        return result;
    }

    BigInt operator/(const BigInt& other) const {
        BigInt result;
        mpz_div(result.value, value, other.value);
        return result;
    }

    BigInt operator%(const BigInt& other) const {
        BigInt result;
        mpz_mod(result.value, value, other.value);
        return result;
    }

    BigInt operator^(const BigInt& other) const {
        BigInt result;
        mpz_xor(result.value, value, other.value);
        return result;
    }

    BigInt operator+=(const BigInt& other) {
        mpz_add(value, value, other.value);
        return *this;
    }

    BigInt operator/=(const BigInt& other) {
        mpz_div(value, value, other.value);
        return *this;
    }

    BigInt operator%=(const BigInt& other) {
        mpz_mod(value, value, other.value);
        return *this;
    }

    BigInt operator++() {
        mpz_add_ui(value, value, 1);
        return *this;
    }

    BigInt operator++(int) {
        BigInt result(*this);
        mpz_add_ui(value, value, 1);
        return result;
    }

    bool operator==(const BigInt& other) const {
        return mpz_cmp(value, other.value) == 0;
    }

    bool operator!=(const BigInt& other) const {
        return mpz_cmp(value, other.value) != 0;
    }

    bool operator<(const BigInt& other) const {
        return mpz_cmp(value, other.value) < 0;
    }

    bool operator<=(const BigInt& other) const {
        return mpz_cmp(value, other.value) <= 0;
    }

    bool operator>(const BigInt& other) const {
        return mpz_cmp(value, other.value) > 0;
    }

    bool operator>=(const BigInt& other) const {
        return mpz_cmp(value, other.value) >= 0;
    }



    // ------------------- Member Function -------------------
    // mpz_t& getValue() const {
    //     return value;
    // }

    void fromMessage(const string& message) {
        mpz_import(this->value, message.size(), 1, sizeof(char), 0, 0, message.c_str());
    }

    string toMessage() const {
        size_t count;
        char* str = (char*)mpz_export(NULL, &count, 1, sizeof(char), 0, 0, value);
        std::string result(str, count);
        free(str);
        return result;
    }

    string get16String() const {
        char* str256bit = new char[NumBits / 4 + 1];
        mpz_get_str(str256bit, 16, value);
        std::string result(str256bit);
        delete[] str256bit;
        return result;
    }

    string get2String() const {
        char* str256bit = new char[NumBits + 1];
        mpz_get_str(str256bit, 2, value);
        std::string result(str256bit);
        delete[] str256bit;
        return result;
    }

    static BigInt Random() {
        mpz_t number;
        mpz_init2(number, NumBits);

        // Set the seed for random number generation
        gmp_randstate_t state;
        gmp_randinit_default(state);
        gmp_randseed_ui(state, time(NULL));  // Set the seed value as desired

        // Generate a random NumBits-bit number
        mpz_urandomb(number, state, NumBits);

        return BigInt(number);
    }

    int getLegendraSymbol(const BigInt& p) const {
        return mpz_legendre(this->value, p.value);
    }

    // For x^2 = n(*this) (mod p), return x
    // There are 2 solutions, return the smaller one
    BigInt getDiscreteSquareRootMod(const BigInt& p) const {
        // if n = 0, then x = 0
        if (this->value == 0) return BigInt(0);
        // Check if n is a quadratic residue mod p
        // by legendra symbol
        if (this->getLegendraSymbol(p) != 1) {
            // Very unlikely to happen
            throw std::runtime_error("n is not a quadratic residue mod p");
            return BigInt(-1);
        }
        // Randomly find a that a^2 - n is not a quadratic residue mod p
        // This is easy because there are only (p-1)/2 possible values in this Group
        // extended element w has w^2 = a^2 - n (mod p)
        BigInt a, w_squqre;
        while (true) {
            a = BigInt::Random() % p;
            w_squqre = (a * a - *this) % p;
            if (w_squqre.getLegendraSymbol(p) == -1) break;
        }
        // Calculate x = (a + w)^((p+1)/2) (mod p)
        // Like complex
        BigInt x = a;
        BigInt y = BigInt(1);
        extend_pow(x, y, x, y, (p + 1) / 2, w_squqre, p);
        x = (x%p + p) % p;
        return (x < p - x) ? x : p - x;
    }
};

#endif