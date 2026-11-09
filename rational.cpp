#include <iostream>
#include <stdexcept>

using i64 = int64_t;
using i32 = int32_t;
using i16 = int16_t;
using i8 = int8_t;
using u64 = uint64_t;
using u32 = uint32_t;
using u16 = uint16_t;
using u8 = uint8_t;

i64 gcd(i64 a, i64 b) {
    if (b == 0) return a;
    return gcd(b, a % b);
}

i64 lcm(i64 a, i64 b) {
    if (a == 0 || b == 0) return 0;
    return std::abs(a / gcd(a, b)) * b;
}

class Rational {
private:
    i64 numerator;
    i64 denominator;
public:
    i64 num() const {
        return this->numerator;
    }
    i64 den() const {
        return this->denominator;
    }

    Rational(i64 num = 0, i64 den = 1) {
        if (den == 0) {
            throw std::invalid_argument("Denominator can't be zero");
        }
        i64 common_divisor = gcd(num, den);
        numerator = num / common_divisor;
        denominator = den / common_divisor;
        if (denominator < 0) {
            numerator = -numerator;
            denominator = -denominator;
        }
        if (numerator == 0) {
            denominator = 1;
        }
    }

    Rational operator+(const Rational& other) const {
        // factor out the gcd of the denominators first before finding cross products
        i64 g = gcd(this->denominator, other.denominator);
        i64 b1 = this->denominator / g;
        i64 b2 = other.denominator / g;

        // numerator is (a * d') + (c * b')
        // denominator is b1 * b2 * g (the lcm)
        // using __int128 because of overflow
        __int128 cross1 = static_cast<__int128>(this->numerator) * b2;
        __int128 cross2 = static_cast<__int128>(other.numerator) * b1;
        __int128 num = cross1 + cross2;
        __int128 den = static_cast<__int128>(b1) * other.denominator;

        return Rational(static_cast<i64>(num), static_cast<i64>(den));
    }
    Rational operator-(const Rational& other) const {
        return Rational(this->numerator * other.denominator - other.numerator * this->denominator, this->denominator * other.denominator);
    }
    Rational operator*(const Rational& other) const {
        // instead of multiplying a * c and b * d, find partial GCDs between opposite numerators and denominators
        i64 g1 = gcd(this->numerator, other.denominator);
        i64 g2 = gcd(other.numerator, this->denominator);

        return Rational((this->numerator / g1) * (other.numerator / g2), (this->denominator / g2) * (other.denominator / g1));
    }
    Rational operator/(const Rational& other) const {
        return Rational(this->numerator * other.denominator, this->denominator * other.numerator);
    }

    bool operator==(const Rational& other) const = default;

    std::strong_ordering operator<=>(const Rational& other) const {
        __int128 cross1 = static_cast<__int128>(numerator) * other.denominator;
        __int128 cross2 = static_cast<__int128>(other.numerator) * denominator;
        return cross1 <=> cross2;
    }
};

std::ostream& operator<<(std::ostream& os, const Rational& r) {
    os << r.num();
    if (r.den() != 1) {
        os << "/" << r.den();
    }
    return os;
}

int main(void) {
    Rational a(10, 15), b(3, 10);
    std::cout << a << " + " << b << " = " << (a + b) << "\n";
    std::cout << a << " - " << b << " = " << (a - b) << "\n";
    std::cout << a << " * " << b << " = " << (a * b) << "\n";
    std::cout << a << " / " << b << " = " << (a / b) << "\n";
    std::cout << a << " + " << 3 << " = " << (a + 3) << "\n";
    std::cout << "is " << a << " > " << b << "? " << std::boolalpha << (a == b) << ".\n";
    return 0;
}