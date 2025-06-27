template<class T>
T power(T a, ll b, T res = 1) {
    for (; b != 0; b /= 2, a *= a) {
        if (b & 1) {
            res *= a;
        }
    }
    return res;
}
template<const ull P>
ull mulMod(ull a, ull b) {
    ull res = a * b - ull(1.L * a * b / P - 0.5L) * P;
    res %= P;
    return res;
}
template<const u32 P>
u32 mulMod(u32 a, u32 b) {
    return ull(a) * b % P;
}
template<class T, const T P>
struct MInt
{
    T x = 0;
    MInt(): x(0) {}
    MInt(T _x): x(_x % P) {}
    T norm(T x) {return (x >= P ? x - P : x);}
    MInt &operator=(T _x) {x = _x % P; return *this;}
    MInt &operator+=(MInt t) {x = norm(x + t.x); return *this;}
    MInt &operator-=(MInt t) {x = norm(x + P - t.x); return *this;}
    MInt &operator*=(MInt t) {x = mulMod<P>(x, t.x); return *this;}
    MInt &operator/=(MInt t) {*this *= power(t, P - 2); return *this;}
    friend MInt operator+(MInt a, MInt b) {return (a += b);}
    friend MInt operator-(MInt a, MInt b) {return (a -= b);}
    friend MInt operator*(MInt a, MInt b) {return (a *= b);}
    friend MInt operator/(MInt a, MInt b) {return (a /= b);}
    friend bool operator==(MInt a, MInt b) {return (a.x == b.x);}
    friend bool operator<(MInt a, MInt b) {return (a.x < b.x);}
};
constexpr int P = 998244353;
using Z = MInt<u32, P>;
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