struct LCA {
    int n;
    vector<pair<int, int>> e;
    vector<int> d, g, in;
    RMQ<ull> rmq{vector<ull>{}};

    LCA(int n) : n(n), d(n + 2), g(2 * n), in(n + 1) {
        e.reserve(n - 1);
    }
    void addEdge(int u, int v) {
        e.emplace_back(u, v);
        ++d[u]; ++d[v];
    }
    void work(int root = 1) {
        if (!e.empty()) {
            for (int i = 1; i <= n + 1; ++i) d[i] += d[i - 1];
            for (auto [u, v] : e) {
                g[--d[u]] = v;
                g[--d[v]] = u;
            }
            decltype(e)().swap(e);
        }
        vector<ull> a(n);
        vector<int> stk(n);
        int top = 0, timer = 0;
        stk[top++] = root;
        in[root] = root;
        while (top) {
            // 未访问时 in[u] 暂存父亲，访问后改为 DFS 序。
            int u = stk[--top], p = in[u];
            in[u] = ++timer;
            a[timer - 1] = (ull(in[p]) << 32) | unsigned(p);
            for (int i = d[u]; i < d[u + 1]; ++i) {
                int v = g[i];
                if (v == p) continue;
                in[v] = u;
                stk[top++] = v;
            }
        }
        rmq = RMQ<ull>(a);
    }
    int lca(int u, int v) const {
        if (u == v) return u;
        int l = in[u], r = in[v];
        if (l > r) swap(l, r);
        return uint32_t(rmq.query(l, r - 1));
    }
};