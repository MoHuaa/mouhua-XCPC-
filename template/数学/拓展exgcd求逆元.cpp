ll exgcd(ll a, ll b, ll &x, ll &y) {
    if (!b) {
        x = 1, y = 0;
        return a;
    }
    ll d = exgcd(b, a % b, y, x);
    y -= a / b * x;
    return d;
}
ll inv(ll n, ll M) {
    ll x, y;
    exgcd(n, M, x, y);
    x = (x % M + M) % M;
    return x;
}