#ifndef CIPOLLA_H
#define CIPOLLA_H

#include "BigInt.h"
#include <vector>
#include <utility>
using namespace std;

class Cipolla {
public:
    static vector<BigInt> Solve(BigInt a, BigInt p);
    static pair<BigInt, BigInt> ComplexPow(BigInt t, BigInt omega, BigInt exp, BigInt mod);
    static int LegendreSymbol(BigInt a, BigInt p);
    static BigInt RandomInt(BigInt max);
    static BigInt ModPow(BigInt base, BigInt exponent, BigInt mod);

private:
    Cipolla() {}
};

#endif
