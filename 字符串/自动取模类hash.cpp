using ull = unsigned long long;
using u128 = unsigned __int128;
constexpr ull Mod = (1ULL << 61) - 1;
template<typename T>
constexpr T power(T a, ull b) {
    T res {1};
    for (; b != 0; b /= 2, a *= a) {
        if (b % 2 == 1) {
            res *= a;
        }
    }
    return res;
}

ull mulMod(ull a, ull b) {
    u128 t = static_cast<u128>(a) * b;
    t = (t >> 61) + (t & Mod);
    return (t >= Mod) ? t - Mod : t;
}

template<typename U, U P>
struct ModIntBase {
public:
    constexpr ModIntBase() : x {0} {}

    template<typename T, typename = std::enable_if_t<std::is_integral<T>::value>>
    constexpr ModIntBase(T x_) : x {norm(x_ % P)} {}

    constexpr static U norm(U x) {
        if ((x >> (8 * sizeof(U) - 1) & 1) == 1) {
            x += P;
        }
        if (x >= P) {
            x -= P;
        }
        return x;
    }

    constexpr U val() const {
        return x;
    }

    constexpr ModIntBase operator-() const {
        ModIntBase res;
        res.x = norm(P - x);
        return res;
    }

    constexpr ModIntBase inv() const {
        return power(*this, P - 2);
    }

    constexpr ModIntBase &operator*=(const ModIntBase &rhs) & {
        x = mulMod(x, rhs.val());
        return *this;
    }

    constexpr ModIntBase &operator+=(const ModIntBase &rhs) & {
        x = norm(x + rhs.x);
        return *this;
    }

    constexpr ModIntBase &operator-=(const ModIntBase &rhs) & {
        x = norm(x- rhs.x+P);
        return *this;
    }

    constexpr ModIntBase &operator/=(const ModIntBase &rhs) & {
        return *this *= rhs.inv();
    }

    friend constexpr ModIntBase operator*(ModIntBase lhs, const ModIntBase &rhs) {
        lhs *= rhs;
        return lhs;
    }

    friend constexpr ModIntBase operator+(ModIntBase lhs, const ModIntBase &rhs) {
        lhs += rhs;
        return lhs;
    }

    friend constexpr ModIntBase operator-(ModIntBase lhs, const ModIntBase &rhs) {
        lhs -= rhs;
        return lhs;
    }

    friend constexpr ModIntBase operator/(ModIntBase lhs, const ModIntBase &rhs) {
        lhs /= rhs;
        return lhs;
    }

    friend constexpr std::ostream &operator<<(std::ostream &os, const ModIntBase &a) {
        return os << a.val();
    }

    friend constexpr bool operator==(ModIntBase lhs, ModIntBase rhs) {
        return lhs.val() == rhs.val();
    }

    friend constexpr bool operator!=(ModIntBase lhs, ModIntBase rhs) {
        return lhs.val() != rhs.val();
    }

    friend constexpr bool operator<(ModIntBase lhs, ModIntBase rhs) {
        return lhs.val() < rhs.val();
    }

private:
    U x;
};

template<ull P>
using ModInt64 = ModIntBase<ull, P>;
using Z = ModInt64<Mod>;
constexpr int maxn = 1e6 + 7;
mt19937 mt{random_device{}()};
uniform_int_distribution<> dis(2000, 5000);
const int Seed = dis(mt);
Z bas[maxn];
void HInit() {
    bas[0] = Z(1);
    for (int i = 1; i < maxn; i++) {
        bas[i] = bas[i - 1] * Seed;
    }
}
struct Hash {
    int n;
    std::vector<Z> sum;
    Hash(const string& s): n(s.length()), sum(n + 1) {
        for (int i = 1; i <= n; i++) {
            sum[i] = (sum[i - 1] * Seed) + s[i - 1];
        }
    }
    Z getHash(int l, int r) {
        return (sum[r] - sum[l - 1] * bas[r - l + 1]);
    }
};