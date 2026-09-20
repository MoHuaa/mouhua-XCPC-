constexpr u32 P = 998244353, G = 3;
using Z = MInt<u32, P>;

inline std::vector<Z> roots{0, 1};

template<bool inverse = false>
void ntt(std::vector<Z> &a) {
    int n = int(a.size());
    if (n <= 1) return;

    for (int k = int(roots.size()); k < n; k *= 2) {
        roots.resize(2 * k);
        Z e = power(Z(G), (ull(P) - 1) / (2 * k));
        for (int i = k / 2; i < k; ++i) {
            roots[2 * i] = roots[i];
            roots[2 * i + 1] = roots[i] * e;
        }
    }

    for (int k = inverse ? 1 : n / 2; k && k < n;
         k = inverse ? k * 2 : k / 2) {
        for (int i = 0; i < n; i += 2 * k) {
            for (int j = 0; j < k; ++j) {
                Z u = a[i + j], v = a[i + j + k];
                if constexpr (inverse) v *= roots[k + j];
                a[i + j] = u + v;
                a[i + j + k] = u - v;
                if constexpr (!inverse)
                    a[i + j + k] *= roots[k + j];
            }
        }
    }

    if constexpr (inverse) {
        std::reverse(a.begin() + 1, a.end());
        Z iv = P - (P - 1) / n;
        for (Z &x : a) x *= iv;
    }
}

struct Poly : std::vector<Z> {
    using std::vector<Z>::vector;

    friend Poly operator*(Poly a, Poly b) {
        int n = int(a.size()), m = int(b.size());
        if (!n || !m) return {};
        int need = n + m - 1, sz = 1;

        if (std::min(n, m) <= 32) {
            Poly c(need);
            for (int i = 0; i < n; ++i)
                for (int j = 0; j < m; ++j)
                    c[i + j] += a[i] * b[j];
            return c;
        }

        while (sz < need) sz *= 2;
        a.resize(sz), b.resize(sz);
        ntt(a), ntt(b);
        for (int i = 0; i < sz; ++i) a[i] *= b[i];
        ntt<true>(a);
        a.resize(need);
        return a;
    }

    Poly &operator*=(Poly b) {
        return *this = std::move(*this) * std::move(b);
    }
    Poly cdq(Poly f) const {
    int n = int(f.size()), len = 1, lg = 0;
    if (!n) return f;
    while (len < n) len *= 2, ++lg;

    Poly g(begin(), begin() + std::min(size(), f.size()));
    g.resize(len);
    g[0] = 0;
    std::vector<Poly> w(lg + 1);

    auto solve = [&](auto &&self, int l, int k) -> void {
        int s = 1 << k, r = std::min(l + s, n), m = l + s / 2;

        if (s <= 32) {
            for (int i = l; i < r; ++i)
                for (int j = i + 1; j < r; ++j)
                    f[j] += f[i] * g[j - i];
            return;
        }

        self(self, l, k - 1);
        if (m >= n) return;

        if (w[k].empty()) {
            w[k] = Poly(g.begin(), g.begin() + s);
            ntt(w[k]);
        }

        Poly a(s);
        std::copy(f.begin() + l, f.begin() + m, a.begin());
        ntt(a);
        for (int i = 0; i < s; ++i) a[i] *= w[k][i];
        ntt<true>(a);

        for (int i = m; i < r; ++i) f[i] += a[i - l];
        self(self, m, k - 1);
    };

    solve(solve, 0, lg);
    return f;
}
};