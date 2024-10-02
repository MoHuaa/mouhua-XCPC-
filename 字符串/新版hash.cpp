using ull = unsigned long long;
using u128 = unsigned __int128;
constexpr ull Mod = (1ULL << 61) - 1;
constexpr int maxn = 1e6 + 7;
mt19937_64 rnd(chrono::steady_clock::now().time_since_epoch().count());
uniform_int_distribution<ull> dist(Mod / 2, Mod - 1);
const ull Seed = dist(rnd);
ull bas[maxn];
ull mul(ull a, ull b) {
    u128 t = (u128)(a) * b;
    t = (t >> 61) + (t & Mod);
    return (t >= Mod) ? t - Mod : t;
}
void HInit() {
    bas[0] = 1;
    for (int i = 1; i < maxn; i++) {
        bas[i] = mul(bas[i - 1] , Seed);
    }
}
struct Hash {
    int n;
    std::vector<ull> sum;
    Hash(const string& s): n(s.length()), sum(n + 1) {
        for (int i = 1; i <= n; i++) {
            sum[i] = (mul(sum[i - 1] , Seed) + s[i - 1]) % Mod;
        }
    }
    ull getHash(int l, int r) {
        return (sum[r] - mul(sum[l - 1], bas[r - l + 1]) + Mod) % Mod;
    }
};