constexpr ull P=(1ULL<<61)-1;
using Z = MInt<ull,P>;
template<const ull P>
ull mulMod(ull a, ull b) {
    u128 t = (u128)(a) * b;
    t = (t >> 61) + (t & P);
    return (t >= P) ? t - P : t;
}
constexpr int maxn = 1e6 + 7;
mt19937_64 rnd(chrono::steady_clock::now().time_since_epoch().count());
uniform_int_distribution<ull> dist(Mod / 2, Mod - 1);
const ull Seed = dist(rnd);
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