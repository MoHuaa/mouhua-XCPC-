pair<ll,ll> exgcd(ll a, ll b, ll c)
{
    function<ll(ll, ll, ll &, ll &)> ex_gcd = [&](ll a, ll b, ll & x, ll & y)
    {
        if (b == 0)
        {
            x = 1;
            y = 0;
            return a;
        }
        ll x1, y1;
        ll g = ex_gcd(b, a % b, x1, y1);
        x = y1;
        y = x1 - a / b * y1;
        return g;
    };
    ll x, y;
    ll g = ex_gcd(a, b, x, y);
    if (c % g != 0)
        return {0, 0};
    ll z = abs(b / g);
    x = (__int128_t)x * (c / g) % z;
    x = (x + z) % z;
    ll w = abs(a / g);
    y = (__int128_t)y * (c / g) % w;
    y = (y + w) % w;
    ll xmin = x, ymin = y;
    ll xmax = (c - (__int128_t)b * ymin) / a;
    ll ymax = (c - (__int128_t)a * xmin) / b;
    return {x, z};
}