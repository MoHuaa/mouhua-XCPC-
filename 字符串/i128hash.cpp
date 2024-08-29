#include<bits/stdc++.h>
using namespace std;
using u64 = unsigned long long;
using i128 = __int128;
using ll=long long;
constexpr int maxn = 2e5 + 7;
constexpr u64 Mod = u64(1E18) + 9;
int Seed;
i128 bas[maxn];
bool isprime(int n) {
    if (n <= 1) {
        return false;
    }
    for (int i = 2; i * i <= n; i++) {
        if (n % i == 0) {
            return false;
        }
    }
    return true;
}

int findPrime(int n) {
    while (!isprime(n)) {
        n++;
    }
    return n;
}
void HInit() {
    mt19937 mt{random_device{}()};
    uniform_int_distribution<> dis(2000, 5000);
    Seed = dis(mt);
    Seed = findPrime(Seed);
    bas[0] = 1;
    for (int i = 1; i < maxn; i++) {
        bas[i] = bas[i - 1] * Seed % Mod;
    }
}
struct Hash {
    int n;
    std::vector<i128> sum;
    Hash(const string& s): n(s.length()), sum(n + 1) {
        for (int i = 1; i <= n; i++) {
            sum[i] = (sum[i - 1] * Seed % Mod + s[i - 1]) % Mod;
        }
    }
    i128 getHash(int l, int r) {
        return (sum[r] - sum[l - 1] * bas[r - l + 1] % Mod + Mod) % Mod;
    }
};
int main() {
    ios::sync_with_stdio(false); cin.tie(nullptr);
    ll n;
    cin >> n;
    HInit();
    set<i128>s;
    for (int i = 1; i <= n; i++) {
        string ss;
        cin >> ss;
        Hash a(ss);
        ll len = ss.length();
        s.insert(a.getHash(1,len));
    }
    cout<<s.size();
    return 0;
}