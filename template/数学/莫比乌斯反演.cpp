constexpr int maxn = 1e5 + 7;
int p[maxn];
int pr[maxn];
int tot = 0;
int phi[maxn];
int mu[maxn];
void sieve(ll n) {
    p[1] = phi[1] = mu[1] = 1;
    for (ll i = 2; i < n; ++i) {
        if (!p[i]) mu[i] = -1, pr[++tot] = i, phi[i] = i - 1;
        for (int j = 1; j <= tot && i * pr[j] < n; ++j) {
            p[i * pr[j]] = pr[j];
            if (i % pr[j]) {
                mu[i * pr[j]] = -mu[i];
                phi[i * pr[j]] = phi[i] * phi[pr[j]];
            } else {
                phi[pr[j]*i] = phi[i] * pr[j];
                break;
            }
        }
    }
}
