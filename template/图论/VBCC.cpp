struct VBCC {
    int n = 0, cur = 0, cc = 0;
    vector<vector<int>> adj;
    vector<int> dfn, low, deg, stk, vert, off;

    VBCC(int n = 0) { init(n); }

    void init(int m) {
        n = m;
        adj.assign(n + 1, {});
        dfn.assign(n + 1, 0);
        low = deg = dfn;
        stk.clear();
        vert.clear();
        off.assign(1, 0);
        cur = cc = 0;
    }

    void addEdge(int u, int v) {
        if (u == v) return;  // Connectivity/vertex-block convention, not edge blocks.
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    // Emit {u} plus the vertex-stack suffix ending at v. v=0: singleton.
    void emit(int u, int v = 0) {
        vert.push_back(u);
        ++deg[u];
        if (v) {
            int x;
            do {
                x = stk.back();
                stk.pop_back();
                vert.push_back(x);
                ++deg[x];
            } while (x != v);
        }
        off.push_back((int)vert.size());
    }

    int work() {
        fill(dfn.begin(), dfn.end(), 0);
        fill(deg.begin(), deg.end(), 0);
        stk.clear();
        vert.clear();
        off.assign(1, 0);
        cur = cc = 0;

        struct Frame { int u, skip, it; };
        vector<Frame> call;
        auto enter = [&](int u, int p) {
            dfn[u] = low[u] = ++cur;
            stk.push_back(u);
            call.push_back({u, p, 0});
        };

        for (int rt = 1; rt <= n; ++rt) {
            if (dfn[rt]) continue;
            ++cc;
            enter(rt, 0);
            while (!call.empty()) {
                auto& f = call.back();
                int u = f.u;
                if (f.it < (int)adj[u].size()) {
                    int v = adj[u][f.it++];
                    if (v == f.skip) {
                        f.skip = 0;  // Skip exactly one parent adjacency.
                        continue;
                    }
                    if (dfn[v])
                        low[u] = min(low[u], dfn[v]);
                    else
                        enter(v, u);
                    continue;  // f may have been invalidated by enter().
                }

                call.pop_back();
                if (call.empty()) {
                    if (adj[u].empty()) emit(u);
                    stk.pop_back();  // Only this DFS root remains.
                } else {
                    int p = call.back().u;
                    low[p] = min(low[p], low[u]);
                    if (low[u] >= dfn[p]) emit(p, u);
                }
            }
        }
        return count();
    }

    int count() const { return (int)off.size() - 1; }

    // Non-owning, read-only view. Valid until work()/init()/destruction.
    span<const int> operator[](int c) const {
        return span<const int>(vert).subspan(off[c - 1], off[c] - off[c - 1]);
    }

    bool is_cut(int u) const { return deg[u] > 1; }

    // ===== OPTIONAL: components after deleting an original vertex =====
    int after_remove(int u) const {
        return cc - 1 + (adj[u].empty() ? 0 : deg[u]);
    }

    // ===== OPTIONAL: full block-cut forest, circle u=u, square c=n+c =====
    vector<vector<int>> block_cut() const {
        vector<vector<int>> t(n + count() + 1);
        for (int u = 1; u <= n; ++u) t[u].reserve(deg[u]);
        for (int c = 1; c <= count(); ++c) {
            auto vs = (*this)[c];
            t[n + c].reserve(vs.size());
            for (int u : vs) {
                t[u].push_back(n + c);
                t[n + c].push_back(u);
            }
        }
        return t;
    }
};