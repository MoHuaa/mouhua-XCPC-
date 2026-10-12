struct TwoSat : SCC {
    int vars;
    vector<int> ans;
    TwoSat(int m = 0, int total = 0) { init(m, total); }
    void init(int m, int total = 0) {
        vars = m;
        SCC::init(max(2 * m, total));
        ans.assign(m + 1, 0);
    }
    int id(int u, bool f) const { return u + f * vars; }
    // (x_u == f) => (x_v == h)£¬Í¬Ê±Ìí¼ÓÄæ·ñ±ß¡£
    void imply(int u, bool f, int v, bool h) {
        addEdge(id(u, f), id(v, h));
        addEdge(id(v, !h), id(u, !f));
    }
    // (x_u == f) OR (x_v == h)¡£
    void addClause(int u, bool f, int v, bool h) {
        imply(u, !f, v, h);
    }
    void force(int u, bool f) { addEdge(id(u, !f), id(u, f)); }
    bool satisfiable() {
        SCC::work();
        for (int u = 1; u <= vars; ++u) {
            if (bel[u] == bel[u + vars]) return false;
            ans[u] = bel[u] < bel[u + vars];
        }
        return true;
    }
};