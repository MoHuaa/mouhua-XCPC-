namespace factor {
    using f64 = long double;
    ll p;
    f64 invp;
    void setmod(ll x) {
        p = x, invp = (f64) 1 / x;
    }
    ll mul(ll a, ll b) {
        ll z = a * invp * b + 0.5;
        ll res = a * b - z * p;
        return res + (res >> 63 & p);
    }
    ll power(ll a, ll x, ll res = 1) {
        for(;x;x >>= 1, a = mul(a, a))
            if(x & 1) res = mul(res, a);
        return res;
    }
    inline ll rho(ll n) {
        if(!(n & 1)) return 2;
        static std::mt19937_64 gen((size_t)"hehezhou");
        ll x = 0, y = 0, prod = 1;
        auto f = [&](ll o) { return mul(o, o) + 1; };
        setmod(n);
        for(int t = 30, z = 0;t % 64 || std::gcd(prod, n) == 1;++t) {
            if (x == y) x = ++ z, y = f(x);
            if(ll q = mul(prod, x + n - y)) prod = q;
            x = f(x), y = f(f(y));
        }
        return std::gcd(prod, n);
    }
    bool checkprime(ll p) {
        if(p == 1) return 0;
        setmod(p);
        ll d = __builtin_ctzll(p - 1), s = (p - 1) >> d;
        for(ll a : {2, 3, 5, 7, 11, 13, 82, 373}) {
            if(a % p == 0)
                continue;
            ll x = power(a, s), y;
            for(int i = 0;i < d;++i, x = y) {
                y = mul(x, x);
                if(y == 1 && x != 1 && x != p - 1)
                    return 0;
            }
            if(x != 1) return 0;
        }
        return 1;
    }
    std::vector<ll> get_factor(ll x) {
        std::queue<ll> q; q.push(x);
        std::vector<ll> res;
        for(;q.size();) {
            ll x = q.front(); q.pop();
            if(x == 1) continue;
            if(checkprime(x)) {
                res.push_back(x);
                continue;
            }
            ll y = rho(x);
            q.push(y), q.push(x / y);
        }
        sort(res.begin(), res.end());
        return res;
    }
}