template <class Info>
struct ST {
    int n, m;
    std::vector<int> lg;
    std::vector<std::vector<Info>> f;
    ST(std::vector<int> a) : n(a.size() - 1), lg(a.size()) {
        m = __lg(n) + 1;
        f.resize(n + 1);
        for (int i = 0; i <= n; i++) {
            f[i].resize(m + 1);
            f[i][0] = {a[i]};
            if (i) lg[i] = __lg(i);
        }
        for (int j = 1; j <= m; j++) {
            for (int i = 1; i + (1 << j) - 1 <= n; i++) {
                f[i][j] = f[i][j - 1] + f[i + (1 << j - 1)][j - 1];
            }
        }
    }
    Info Query(int l, int r) {
        Info ans = f[l][0];
        int p = l + 1;
        for (int i = m; i >= 0; i--) {
            if (p + (1 << i) - 1 <= r) {
                ans = ans + f[p][i];
                p += 1 << i;
            }
        }
        return ans;
    }
};
struct Info {
    int x;
    friend Info operator+(const Info &a, const Info &b) {

    }
};