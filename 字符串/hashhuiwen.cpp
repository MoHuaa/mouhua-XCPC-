typedef unsigned long long ULL;
const int maxn = 2e5 + 7;
const int sigma = maxn;
const int HASH_CNT = 2;
char s[maxn];
ULL Prime_Pool[] = {1998585857ul, 23333333333ul};
ULL Seed_Pool[] = {911, 146527, 19260817, 91815541};
ULL Mod_Pool[] = {29123, 998244353, 1000000009, 4294967291ull};

struct Hash {
    ULL Seed, Mod;
    ULL bas[maxn];
    ULL sum[maxn];
    int perm[sigma];

    Hash(ULL Seed, ULL Mod) : Seed(Seed), Mod(Mod) {
        bas[0] = 1;
        for (int i = 1; i < maxn - 2; i++) {
            bas[i] = bas[i - 1] * Seed % Mod;
        }
    }

    void init(int n) {
        for (int i = 1; i <= n; i++) {
            sum[i] = (sum[i - 1] * Seed % Mod + (s[i] - 'a')) % Mod;
        }
    }

    void indexInit(int n) {
        iota(perm + 1, perm + 1 + sigma, 1);
        random_shuffle(perm + 1, perm + 1 + sigma);
        for (int i = 1; i <= n; i++) {
            sum[i] = (sum[i - 1] * Seed % Mod + perm[s[i]]) % Mod;
        }
    }

    ULL getHash(int l, int r) {
        return (sum[r] - sum[l - 1] * bas[r - l + 1] % Mod + Mod) % Mod;
    }
};
struct hashhw {
    Hash ha1[2] = {Hash(Seed_Pool[0], Mod_Pool[0]), Hash(Seed_Pool[1], Mod_Pool[1])};
    Hash ha2[2] = {Hash(Seed_Pool[0], Mod_Pool[0]), Hash(Seed_Pool[1], Mod_Pool[1])};
    int n;
    void init(int _n) {
        n=_n;
        ha1[0].init(n);
        ha1[1].init(n);
        reverse(s + 1, s + 1 + n);
        ha2[0].init(n);
        ha2[1].init(n);
        reverse(s + 1, s + 1 + n);
    }
    bool check(int i, int j) {
        if (ha1[0].getHash(i, j) == ha2[0].getHash(n - j + 1, n - i + 1) and
                ha1[1].getHash(i, j) == ha2[1].getHash(n - j + 1, n - i + 1))
            return true;
        return false;
    }
}h1;