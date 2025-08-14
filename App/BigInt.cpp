#include "BigInt.h"
#include <algorithm>
#include <random>
#include <stdexcept>
#include <cctype>

void BigInt::removeLeadingZeros() {
    while (digits.size() > 1 && digits.back() == 0) {
        digits.pop_back();
    }
    if (digits.empty()) {
        digits.push_back(0);
    }
}

BigInt::BigInt(int64_t value) {
    if (value < 0) {
        isNegative = true;
        value = -value;
    }
    do {
        digits.push_back(value % 10);
        value /= 10;
    } while (value > 0);
}

BigInt::BigInt(const std::string& str) {
    if (str.empty()) {
        digits.push_back(0);
        return;
    }

    size_t start = 0;
    if (str[0] == '-') {
        isNegative = true;
        start = 1;
    }

    for (size_t i = str.length(); i > start; ) {
        if (!isdigit(str[i - 1])) {
            throw std::invalid_argument("Invalid character in BigInt string");
        }
        digits.push_back(str[--i] - '0');
    }
    removeLeadingZeros();
}

//=======================================

BigInt BigInt::operator-() const {
    BigInt result = *this;
    if (result != BigInt(0)) {
        result.isNegative = !result.isNegative;
    }
    return result;
}

bool BigInt::operator!=(const BigInt& other) const {
    return !(*this == other);
}

bool BigInt::operator==(const BigInt& other) const {
    return digits == other.digits && isNegative == other.isNegative;
}

bool BigInt::operator<(const BigInt& other) const {
    if (isNegative != other.isNegative) {
        return isNegative;
    }

    if (digits.size() != other.digits.size()) {
        return digits.size() < other.digits.size();
    }

    for (size_t i = digits.size(); i-- > 0;) {
        if (digits[i] != other.digits[i]) {
            return digits[i] < other.digits[i];
        }
    }

    return false;
}

bool BigInt::operator>(const BigInt& other) const {
    return other < *this;
}

bool BigInt::operator<=(const BigInt& other) const {
    return !(other < *this);
}

bool BigInt::operator>=(const BigInt& other) const {
    return !(*this < other);
}

BigInt BigInt::operator+(const BigInt& other) const {
    if (isNegative != other.isNegative) {
        if (isNegative) {
            return other - (-*this);
        }
        else {
            return *this - (-other);
        }
    }

    BigInt result;
    result.isNegative = isNegative;
    result.digits.clear();

    size_t max_len = std::max(digits.size(), other.digits.size());
    uint32_t carry = 0;

    for (size_t i = 0; i < max_len || carry; ++i) {
        uint32_t sum = carry;
        if (i < digits.size()) sum += digits[i];
        if (i < other.digits.size()) sum += other.digits[i];
        result.digits.push_back(sum % 10);
        carry = sum / 10;
    }

    return result;
}

BigInt BigInt::operator-(const BigInt& other) const {
    if (isNegative != other.isNegative) {
        return *this + (-other);
    }

    if (abs() < other.abs()) {
        return -(other - *this);
    }

    BigInt result;
    result.isNegative = isNegative;
    result.digits.clear();

    int32_t borrow = 0;
    for (size_t i = 0; i < digits.size(); ++i) {
        int32_t diff = digits[i] - borrow;
        if (i < other.digits.size()) diff -= other.digits[i];
        if (diff < 0) {
            diff += 10;
            borrow = 1;
        }
        else {
            borrow = 0;
        }
        result.digits.push_back(diff);
    }

    result.removeLeadingZeros();

    if (result == BigInt(0)) {
        result.isNegative = false;
    }

    return result;
}

BigInt BigInt::operator*(const BigInt& other) const {
    BigInt result;
    result.digits.resize(digits.size() + other.digits.size(), 0);

    for (size_t i = 0; i < digits.size(); ++i) {
        uint32_t carry = 0;
        for (size_t j = 0; j < other.digits.size() || carry; ++j) {
            uint64_t product = result.digits[i + j] +
                static_cast<uint64_t>(digits[i]) *
                (j < other.digits.size() ? other.digits[j] : 0) +
                carry;
            result.digits[i + j] = product % 10;
            carry = product / 10;
        }
    }

    result.isNegative = isNegative != other.isNegative;
    result.removeLeadingZeros();
    return result;
}

BigInt BigInt::operator/(const BigInt& other) const {
    if (other == BigInt(0)) {
        throw std::runtime_error("Division by zero");
    }

    BigInt dividend = abs();
    BigInt divisor = other.abs();
    BigInt quotient, remainder;

    for (int i = static_cast<int>(dividend.digits.size()) - 1; i >= 0; --i) {
        remainder = remainder * BigInt(10) + BigInt(dividend.digits[i]);
        uint32_t digit = 0;
        while (remainder >= divisor) {
            remainder = remainder - divisor;
            digit++;
        }
        quotient.digits.insert(quotient.digits.begin(), digit);
    }

    quotient.removeLeadingZeros();
    quotient.isNegative = isNegative != other.isNegative;
    return quotient;
}

BigInt BigInt::operator%(const BigInt& other) const {
    BigInt quotient = *this / other;
    return *this - quotient * other;
}

//=======================================

BigInt BigInt::abs() const {
    BigInt result = *this;
    result.isNegative = false;
    return result;
}

std::string BigInt::toString() const {
    std::string str;
    if (isNegative) {
        str += '-';
    }
    for (auto it = digits.rbegin(); it != digits.rend(); ++it) {
        str += std::to_string(*it);
    }
    return str;
}

BigInt BigInt::modpow(BigInt base, BigInt exponent, const BigInt& mod) {
    BigInt result(1);
    base = base % mod;

    while (exponent > BigInt(0)) {
        if (exponent % BigInt(2) == BigInt(1)) {
            result = (result * base) % mod;
        }
        exponent = exponent / BigInt(2);
        base = (base * base) % mod;
    }
    return result;
}

BigInt BigInt::random(const BigInt& max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());

    if (max <= BigInt(0)) {
        throw std::invalid_argument("Maximum must be positive");
    }

    std::string numStr;
    std::string maxStr = max.abs().toString();

    bool less = false;
    for (size_t i = 0; i < maxStr.length(); ++i) {
        char maxDigit = maxStr[i];
        char minDigit = less ? '0' : '1';
        char randomDigit;

        if (!less) {
            std::uniform_int_distribution<> dist(minDigit - '0', maxDigit - '0');
            randomDigit = dist(gen) + '0';
            if (randomDigit < maxDigit) less = true;
        }
        else {
            std::uniform_int_distribution<> dist(0, 9);
            randomDigit = dist(gen) + '0';
        }

        numStr += randomDigit;
    }

    return BigInt(numStr);
}

uint64_t BigInt::toUInt64() const {
    if (*this > BigInt(std::numeric_limits<uint64_t>::max())) {
        throw std::overflow_error("BigInt too large for uint64_t");
    }

    uint64_t result = 0;
    uint64_t multiplier = 1;

    for (size_t i = 0; i < digits.size(); i++) {
        result += digits[i] * multiplier;
        multiplier *= 10;
    }

    return result;
}

BigInt BigInt::fromUInt64(uint64_t value) {
    BigInt result;
    result.isNegative = false;
    result.digits.clear();

    if (value == 0) {
        result.digits.push_back(0);
        return result;
    }

    while (value > 0) {
        result.digits.push_back(value % 10);
        value /= 10;
    }

    return result;
}