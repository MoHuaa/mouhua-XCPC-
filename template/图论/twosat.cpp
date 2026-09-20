struct TwoSat {
    int n;
    SCC g;
    vector<int> ans;

    TwoSat(int n = 0) { init(n); }
    void init(int m) {
        n = m;
        g.init(2 * n);
        ans.assign(n + 1, 0);
    }
    int id(int u, bool f) const { return 2 * u - 1 + f; }

    // (x_u == f) OR (x_v == h)
    void addClause(int u, bool f, int v, bool h) {
        g.addEdge(id(u, !f), id(v, h));
        g.addEdge(id(v, !h), id(u, f));
    }

    // ans is valid only after this call returns true, and until graph mutation.
    bool satisfiable() {
        g.work();
        for (int u = 1; u <= n; ++u) {
            int a = g.bel[id(u, false)], b = g.bel[id(u, true)];
            if (a == b) return false;
            ans[u] = a < b;
        }
        return true;
    }

    // OPTIONAL: force x_u == f.
    void force(int u, bool f) { addClause(u, f, u, f); }
};