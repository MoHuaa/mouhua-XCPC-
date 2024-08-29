using ull = unsigned long long;
constexpr int maxn = 1e5 + 7;
ull bas1[maxn], bas2[maxn];
constexpr ull _mod[2] = {1000000007, 980009747};
constexpr ll _Seed[2] = {146527, 19260817};
void HInit() {
    bas1[0] = bas2[0] = 1;
    for (int i = 1; i < maxn; i++) {
        bas1[i] = bas1[i - 1] * _Seed[0] % _mod[0];
        bas2[i] = bas2[i - 1] * _Seed[1] % _mod[1];
    }
}
template<ull base, ull Mod, ull *bas>
struct Hash
{
    int n;
    std::vector<ull> sum;
    Hash(const string &s): n(s.size()), sum(n + 1) {
        for (int i = 1; i <= n; i++) {
            sum[i] = (sum[i - 1] * base % Mod + s[i - 1]) % Mod;
        }
    }
    ull getHash(int l, int r) {
        return (sum[r] - sum[l - 1] * bas[r - l + 1] % Mod + Mod) % Mod;
    }
};
struct HHash {
    Hash<_Seed[0], _mod[0], bas1>h1;
    Hash<_Seed[1], _mod[1], bas2>h2;
    HHash(const string&s): h1(s), h2(s) {
    }
    array<ull, 2>getHash(int l, int r) {
        return array<ull, 2> {h1.getHash(l, r), h2.getHash(l, r)};
    }
};
struct hashhw {
    HHash h1, h2;
    int n;
    hashhw(string s): h1(s), n(s.length()), h2(string(s.rbegin(), s.rend())) {}
    bool check(int i, int j) {
        if (h1.getHash(i, j) == h2.getHash(n - j + 1, n - i + 1)) {
            return true;
        }
        return false;
    }
};