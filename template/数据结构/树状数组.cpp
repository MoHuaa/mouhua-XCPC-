template<class T>
struct BIT {
    int n = 0;
    std::vector<T> tr;

    BIT(int n = 0) { init(n); }

    void init(int m) {
        assert(0 <= m && m < (1 << 30));
        n = m;
        tr.assign(n + 1, T{});
    }

    void add(int x, T v) {
        assert(1 <= x && x <= n);
        for (; x <= n; x += x & -x)
            tr[x] = tr[x] + v;
    }

    T sum(int x) const {
        // assert(0 <= x && x <= n);
        T ans{};
        for (; x; x -= x & -x)
            ans = ans + tr[x];
        return ans;
    }

    T getSum(int l, int r) const {
        if (l > r) return T{};
        assert(1 <= l && r <= n);
        T ans{};
        // Stop when the two prefix decompositions reach their common node.
        for (--l; r > l; r -= r & -r)
            ans = ans + tr[r];
        for (; l > r; l -= l & -l)
            ans = ans - tr[l];
        return ans;
    }
     int kth(T k) const {
        if (!n || !(T{} < k)) return -1;
        int x = 0;
        for (int d = 1 << std::__lg(n); d; d >>= 1) {
            int y = x + d;
            if (y <= n && tr[y] < k) {
                x = y;
                k = k - tr[y];
            }
        }
        return x < n ? x + 1 : -1;
    }
};