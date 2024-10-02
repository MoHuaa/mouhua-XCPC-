using ull = unsigned long long;
using u128 = unsigned __int128;
constexpr ull Mod = (1ULL << 61) - 1;
constexpr int maxn = 1e6 + 7;
mt19937_64 rnd(chrono::steady_clock::now().time_since_epoch().count());
uniform_int_distribution<ull> dist(Mod / 2, Mod - 1);
const ull Seed = dist(rnd);
ull bas[maxn];
ull norm(ull x) {
    if (x >= Mod) {
        x -= Mod;
    }
    return x;
}
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
            sum[i] = norm(mul(sum[i - 1] , Seed) + s[i - 1]);
        }
    }
    ull getHash(int l, int r) {
        ull res = norm(sum[r] - mul(sum[l - 1], bas[r - l + 1])+Mod);
        return res;
    }
};
struct HHash{
    Hash h1,h2;
    int n;
    HHash(string s): h1(s), h2(string(s.rbegin(), s.rend())), n(s.length()){}
    bool check(int i, int j) {
        if (h1.getHash(i, j) == h2.getHash(n - j + 1, n - i + 1)) {
            return true;
        }
        return false;
    }
};