#include "Cipolla.h"
#include "BigInt.h"
#include <random>
#include <stdexcept>
#include <limits>
using namespace std;

vector<BigInt> Cipolla::Solve(BigInt a, BigInt p) {
    vector<BigInt> solutions;

    if (p <= 2) return solutions;
    if (a % p == 0) {
        solutions.push_back(0);
        return solutions;
    }

    int legendre = LegendreSymbol(a, p);
    if (legendre != 1) return solutions;

    for (BigInt t_val = 1; t_val <= 10; t_val = t_val + 1) {
        BigInt omega = (t_val * t_val - a) % p;
        if (omega < 0) omega = omega + p;

        if (omega != 0 && LegendreSymbol(omega, p) == -1) {
            auto [x, y] = ComplexPow(t_val, omega, (p + 1) / 2, p);
            BigInt x1 = (x + p) % p;
            BigInt x2 = (p - x1) % p;

            if ((x1 * x1) % p == a % p) solutions.push_back(x1);
            if ((x2 * x2) % p == a % p && x1 != x2) solutions.push_back(x2);

            if (!solutions.empty()) return solutions;
        }
    }

    int attempts = 0;
    const int maxAttempts = 100;
    while (attempts++ < maxAttempts) {
        BigInt t = RandomInt(p);
        BigInt omega = (t * t - a) % p;
        if (omega < 0) omega = omega + p;

        if (omega != 0 && LegendreSymbol(omega, p) == -1) {
            auto [x, y] = ComplexPow(t, omega, (p + 1) / 2, p);
            BigInt x1 = (x + p) % p;
            BigInt x2 = (p - x1) % p;

            if ((x1 * x1) % p == a % p) solutions.push_back(x1);
            if ((x2 * x2) % p == a % p && x1 != x2) solutions.push_back(x2);

            if (!solutions.empty()) return solutions;
        }
    }

    return solutions;
}

pair<BigInt, BigInt> Cipolla::ComplexPow(BigInt t, BigInt omega, BigInt exp, BigInt mod) {
    pair<BigInt, BigInt> res = { 1, 0 };
    pair<BigInt, BigInt> base = { t % mod, 1 };

    while (exp > 0) {
        if (exp % 2 == 1) {
            BigInt new_real = (res.first * base.first + res.second * base.second * omega) % mod;
            BigInt new_imag = (res.first * base.second + res.second * base.first) % mod;
            res = { new_real, new_imag };
        }

        BigInt new_real = (base.first * base.first + base.second * base.second * omega) % mod;
        BigInt new_imag = (BigInt(2) * base.first * base.second) % mod;
        base = { new_real, new_imag };

        exp = exp / 2;
    }

    return res;
}

BigInt Cipolla::RandomInt(BigInt max) {
    if (max <= 0) throw invalid_argument("Max must be positive");

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<unsigned long long> dis(0, numeric_limits<unsigned long long>::max());

    BigInt random_value = 0;
    while (random_value < max) {
        unsigned long long part = dis(gen);
        random_value = random_value * (numeric_limits<unsigned long long>::max() + 1) + part;
    }

    return random_value % max;
}

int Cipolla::LegendreSymbol(BigInt a, BigInt p) {
    BigInt exponent = (p - 1) / 2;
    BigInt result = ModPow(a, exponent, p);

    if (result == 0) return 0;
    if (result == 1) return 1;
    if (result == p - 1) return -1;

    throw runtime_error("Invalid Legendre symbol value");
}

BigInt Cipolla::ModPow(BigInt base, BigInt exponent, BigInt mod) {
    BigInt result = 1;
    base = base % mod;

    while (exponent > 0) {
        if (exponent % 2 == 1) {
            result = (result * base) % mod;
        }

        base = (base * base) % mod;
        exponent = exponent / 2;
    }

    return result;
}
