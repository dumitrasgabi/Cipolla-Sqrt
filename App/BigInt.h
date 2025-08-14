#pragma once
#include <vector>
#include <string>
#include <iostream>

class BigInt {
private:
    std::vector<uint32_t> digits;
    bool isNegative = false;

public:
    BigInt() = default;
    BigInt(int64_t value);
    BigInt(const std::string& str);
    uint64_t toUInt64() const;
    static BigInt fromUInt64(uint64_t value);

    BigInt operator+(const BigInt& other) const;
    BigInt operator-(const BigInt& other) const;
    BigInt operator*(const BigInt& other) const;
    BigInt operator/(const BigInt& other) const;
    BigInt operator%(const BigInt& other) const;

    bool operator==(const BigInt& other) const;
    bool operator!=(const BigInt& other) const;
    bool operator<(const BigInt& other) const;
    bool operator<=(const BigInt& other) const;
    bool operator>(const BigInt& other) const;
    bool operator>=(const BigInt& other) const;

    BigInt operator-() const;

    BigInt abs() const;
    std::string toString() const;

    static BigInt modpow(BigInt base, BigInt exponent, const BigInt& mod);
    static BigInt random(const BigInt& max);
    void removeLeadingZeros();
};

