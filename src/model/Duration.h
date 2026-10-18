// Rational durations. A whole note is 1/1, a quarter is 1/4.
// Add and compare by cross-multiplying. No floating point.

#pragma once

#include <cstdlib>

namespace nota {

struct Duration {
    int num = 0;
    int den = 1;
};

[[nodiscard]] inline int durationGcd(int a, int b) {
    a = std::abs(a);
    b = std::abs(b);
    while (b != 0) {
        const int rest = a % b;
        a = b;
        b = rest;
    }
    return a == 0 ? 1 : a;
}

// Reduce by gcd. A negative denominator flips the sign onto the numerator.
[[nodiscard]] inline Duration duration(int num, int den) {
    if (den < 0) {
        num = -num;
        den = -den;
    }
    const int divisor = durationGcd(num, den);
    return Duration{num / divisor, den / divisor};
}

[[nodiscard]] inline Duration operator+(Duration left, Duration right) {
    return duration(left.num * right.den + right.num * left.den, left.den * right.den);
}

// Negative when left is shorter, zero when equal, positive when left is longer.
[[nodiscard]] inline int compare(Duration left, Duration right) {
    const long long lhs = static_cast<long long>(left.num) * right.den;
    const long long rhs = static_cast<long long>(right.num) * left.den;
    if (lhs < rhs) {
        return -1;
    }
    if (lhs > rhs) {
        return 1;
    }
    return 0;
}

[[nodiscard]] inline Duration quarterDuration() {
    return Duration{1, 4};
}

[[nodiscard]] inline Duration wholeDuration() {
    return Duration{1, 1};
}

} // namespace nota
