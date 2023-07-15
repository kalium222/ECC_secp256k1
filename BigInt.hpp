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
};

#endif