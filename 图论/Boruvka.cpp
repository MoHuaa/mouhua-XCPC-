template <typename T>
struct Boruvka {
    int n;
    DSU c;
    const T INF;
    explicit Boruvka(size_t n, T INF = numeric_limits<T>::max())
        : n(n), c(n + 1), INF(INF) {}

    inline int find(int k) { return c.find(k); }

    template <typename F>
    T build(const F &update) {
        T cost = T{};
        while (c.size(1) < n) {
            vector<pair<T, int> > v(n + 1, make_pair(INF, -1));
            update(n);
            bool con = false;
            for (int i = 1; i <= n; i++) {
                if (v[i].second >= 1 && c.merge(i, v[i].second)) {
                    cost += v[i].first;
                    con = true;
                }
            }
            if (!con) return INF;
        }
        return cost;
    }
};