using ull = unsigned long long;
constexpr int maxn = 1e6 + 7;
ull bas1[maxn], bas2[maxn];
constexpr ll mod1 = 1000000007;
constexpr ll mod2 = 980009747;
constexpr ll Seed1 = 146527;
constexpr ll Seed2 = 19260817;
void HInit() {
    bas1[0] = bas2[0] = 1;
    for (int i = 1; i < maxn; i++) {
        bas1[i] = bas1[i - 1] * Seed1 % mod1;
        bas2[i] = bas2[i - 1] * Seed2 % mod2;
    }
}
template<ull base, ull Mod>
struct Hash
{
    int n;
    std::vector<ull> sum;
    ull *bas;
    Hash(const string &s, ull *Bas): n(s.size()), sum(n + 1), bas(Bas) {
        for (int i = 1; i <= n; i++) {
            sum[i] = (sum[i - 1] * base % Mod + s[i - 1]) % Mod;
        }
    }
    ull getHash(int l, int r) {
        return (sum[r] - sum[l - 1] * bas[r - l + 1] % Mod + Mod) % Mod;
    }
};
struct HHash {
    Hash<Seed1, mod1>h1;
    Hash<Seed2, mod2>h2;
    HHash(const string&s): h1(s, bas1), h2(s, bas2) {
    }
    array<ull, 2>getHash(int l, int r) {
        return array<ull, 2>{h1.getHash(l,r),h2.getHash(l,r)};
    }
};