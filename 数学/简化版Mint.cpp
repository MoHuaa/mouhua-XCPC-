template<int P>
struct MInt {
    int x;
    constexpr MInt() : x(0) {}
    constexpr MInt(i64 x) : x(x % P) {}

    constexpr MInt operator-() const {
        return MInt(x ? P - x : 0);
    }

    constexpr MInt inv() const {
        return power(*this, P - 2);
    }

    constexpr MInt& operator+=(const MInt& rhs) {
        x = (x + rhs.x) % P;
        return *this;
    }

    constexpr MInt& operator-=(const MInt& rhs) {
        x = (x - rhs.x + P) % P;
        return *this;
    }

    constexpr MInt& operator*=(const MInt& rhs) {
        x = 1LL * x * rhs.x % P;
        return *this;
    }

    constexpr MInt& operator/=(const MInt& rhs) {
        return *this *= rhs.inv();
    }

    friend constexpr MInt operator+(const MInt& lhs, const MInt& rhs) {
        return MInt(lhs) += rhs;
    }

    friend constexpr MInt operator-(const MInt& lhs, const MInt& rhs) {
        return MInt(lhs) -= rhs;
    }

    friend constexpr MInt operator*(const MInt& lhs, const MInt& rhs) {
        return MInt(lhs) *= rhs;
    }

    friend constexpr MInt operator/(const MInt& lhs, const MInt& rhs) {
        return MInt(lhs) /= rhs;
    }
};