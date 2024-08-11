template <typename T>
struct fenwick {
    int n;
    std::vector<std::vector<T>> tr;
    fenwick(int _n) : n(_n) {
        tr.resize(2);
        for (int i = 0; i < 2; i++) {
            tr[i].resize(n + 2);
        }
    }
    void add(int i, int x, const T &v) {
        for ( ; x <= n; x += x & -x) {
            tr[i][x] += v;
        }
    }
    void modify(int l, int r, const T &v) {
        add(0, l, v);
        add(0, r + 1, -v);
        add(1, l, l * v);
        add(1, r + 1, (r + 1) * (-v));
    }
    T sum(int i, int x) {
        T ans = 0;
        for ( ; x > 0; x -= x & -x) {
            ans += tr[i][x];
        }
        return ans;
    }
    T Sum(int x) {
        return sum(0, x) * (x + 1) - sum(1, x);
    }
    T Sum(int l, int r) {
        return Sum(r) - Sum(l - 1);
    }
};