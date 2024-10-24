template <class Info>
struct ST {
    int n, m;
    std::vector<std::vector<Info>> f;
    ST(std::vector<ll> &a) : n(a.size() - 1) {
        m = __lg(n) + 1;
        f.resize(m + 1, vector<Info>(n + 1));
        for (int i = 1; i <= n; i++) {
            f[0][i] = {a[i]};
        }
        for (int j = 1; j <= m; j++) {
            for (int i = 1; i + (1 << j) - 1 <= n; i++) {
                f[j][i] = f[j - 1][i] + f[j - 1][i + (1 << (j - 1))];
            }
        }
    }
    Info operator()(int l, int r) {
        int len = (r - l + 1);
        ll k = __lg(len);
        return f[k][l] + f[k][r - (1 << k) + 1];
    }
};
struct Info {
    ll x;
    friend Info operator+(const Info &a, const Info &b) {
        Info res;
        res.x = max(a.x, b.x);
        return res;
    }
};