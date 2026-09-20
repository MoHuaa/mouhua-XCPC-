template<class T>
T power(T a, ull b, T res = 1) {
    for (; b; b >>= 1, a *= a)
        if (b & 1) res *= a;
    return res;
}
template<uint P>
uint mulMod(uint a, uint b) {
    return ull(a) * b % P;
}

template<ull P>
ull mulMod(ull a, ull b) {
    constexpr u128 im = -u128(1) / P;
    constexpr ull l = ull(im), h = ull(im >> 64);
    u128 z = u128(a) * b;
    ull x = ull(z), y = ull(z >> 64);
    u128 t = u128(y) * l + (u128(x) * l >> 64);
    u128 s = u128(x) * h + ull(t);
    ull q = y * h + ull(t >> 64) + ull(s >> 64);
    u128 r = z - u128(q) * P;
    return ull(r >= P ? r - P : r);
}


template<class T, T P>
struct MInt {
    T x = 0;
    MInt() = default;
    template<class U> MInt(U v) {
        ull a = v;
        bool neg = v < 0;
        x = (neg ? -a : a) % P;
        if (neg && x) x = P - x;
    }
    T val() const { return x; }
    MInt &operator+=(MInt b) {
        x = x >= P - b.x ? x - (P - b.x) : x + b.x;
        return *this;
    }
    MInt &operator-=(MInt b) {
        x = x >= b.x ? x - b.x : P - (b.x - x);
        return *this;
    }
    MInt &operator*=(MInt b) {
        x = mulMod<P>(x, b.x);
        return *this;
    }
    MInt inv() const { return power(*this, ull(P) - 2); }
    MInt &operator/=(MInt b) { return *this *= b.inv(); }
    MInt operator-() const { return MInt() - *this; }
    friend MInt operator+(MInt a, MInt b) { return a += b; }
    friend MInt operator-(MInt a, MInt b) { return a -= b; }
    friend MInt operator*(MInt a, MInt b) { return a *= b; }
    friend MInt operator/(MInt a, MInt b) { return a /= b; }
    friend bool operator==(MInt a, MInt b) { return a.x == b.x; }
    friend bool operator!=(MInt a, MInt b) { return a.x != b.x; }
    friend bool operator<(MInt a, MInt b) { return a.x < b.x; }
    friend std::ostream &operator<<(std::ostream &os, MInt a) {
        return os << a.x;
    }
    friend std::istream &operator>>(std::istream &is, MInt &a) {
        ll v;
        if (is >> v) a = MInt(v);
        return is;
    }
};
const int N = 1E5 + 7;
Z fac[N], ifac[N], inv[N];
void init(int x) {
    fac[0] = ifac[0] = inv[1] = 1;
    for (int i = 2; i <= x; i++) inv[i] = (P - P / i) * inv[P % i];
    for (int i = 1; i <= x; i++) fac[i] = fac[i - 1] * i, ifac[i] = ifac[i - 1] * inv[i];
}
Z C(int x, int y) {
    return x < y || y < 0 ? 0 : fac[x] * ifac[y] * ifac[x - y];
}
struct Inversion {
    static constexpr int B = (1 << 10), T = (1 << 20);
    std::array < int, T + 1 > f, p;
    std::array < int, T * 3 + 3 > buf;
    int *I = buf.begin() + T;
    Inversion() {
        for (int i = 1; i <= B; i++) {
            int s = 0, d = (i << 10);
            for (int j = 1; j <= T; j++) {
                if ((s += d) >= P) s -= P;
                if (s <= T) {
                    if (!f[j]) f[j] = i, p[j] = s;
                }
                else if (s >= P - T) {
                    if (!f[j]) f[j] = i, p[j] = s - P;
                }
                else {
                    int t = (P - T - s - 1) / d;
                    s += t * d, j += t;
                }
            }
        }
        I[1] = f[0] = 1;
        for (int i = 2; i <= (T << 1); i++)
            I[i] = 1ll * (P - P / i) * I[P % i] % P;

        for (int i = -1; i >= -T; i--)
            I[i] = P - I[-i];

    }
    Z inv(int x) {
        return Z(1) * I[p[x >> 10] + (x & 1023) * f[x >> 10]] * f[x >> 10];
    }
};