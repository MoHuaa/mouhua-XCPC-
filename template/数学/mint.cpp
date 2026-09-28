template<class T>
T power(T a, ull b, T res = 1) {
    for (; b; b >>= 1, a *= a)
        if (b & 1) res *= a;
    return res;
}
template<u32 P>
uint mulMod(u32 a, u32 b) {
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

template<int V, int P>
constexpr MInt<P> CInv = MInt<P>(V).inv();

constexpr int P = 998244353;
// constexpr int P = 1e9+7;
using Z = MInt<P>;

struct Comb {
    int n;
    std::vector<Z> _fac;
    std::vector<Z> _invfac;
    std::vector<Z> _inv;

    Comb() : n{0}, _fac{1}, _invfac{1}, _inv{0} {}
    Comb(int n) : Comb() {
        init(n);
    }

    void init(int m) {
        if (m <= n) return;
        _fac.resize(m + 1);
        _invfac.resize(m + 1);
        _inv.resize(m + 1);

        for (int i = n + 1; i <= m; i++) {
            _fac[i] = _fac[i - 1] * i;
        }
        _invfac[m] = _fac[m].inv();
        for (int i = m; i > n; i--) {
            _invfac[i - 1] = _invfac[i] * i;
            _inv[i] = _invfac[i] * _fac[i - 1];
        }
        n = m;
    }

    Z fac(int m) {
        if (m > n) init(2 * m);
        return _fac[m];
    }
    Z invfac(int m) {
        if (m > n) init(2 * m);
        return _invfac[m];
    }
    Z inv(int m) {
        if (m > n) init(2 * m);
        return _inv[m];
    }
    Z binom(int n, int m) {
        if (n < m || m < 0) return 0;
        return fac(n) * invfac(m) * invfac(n - m);
    }
} comb;
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
        return Z(1)* I[p[x >> 10] + (x & 1023) * f[x >> 10]] * f[x >> 10];
    }
};