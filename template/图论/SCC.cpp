struct SCC {
    int n = 0, cur = 0, cnt = 0;
    vector<vector<int>> adj;
    vector<int> dfn, low, bel, stk;

    SCC(int n = 0) { init(n); }

    void init(int m) {
        n = m;
        adj.assign(n + 1, {});
        dfn.assign(n + 1, 0);
        low = bel = dfn;
        stk.clear();
        cur = cnt = 0;
    }

    void addEdge(int u, int v) {
        adj[u].push_back(v);
    }

    int work() {
        fill(dfn.begin(), dfn.end(), 0);
        fill(bel.begin(), bel.end(), 0);
        stk.clear();
        cur = cnt = 0;
        vector<array<int, 2>> call; // {vertex, next adjacency index}

        auto enter = [&](int u) {
            dfn[u] = low[u] = ++cur;
            stk.push_back(u);
            call.push_back({u, 0});
        };

        for (int rt = 1; rt <= n; ++rt) {
            if (dfn[rt]) continue;
            enter(rt);

            while (!call.empty()) {
                auto& f = call.back();
                int u = f[0];

                if (f[1] < (int)adj[u].size()) {
                    int v = adj[u][f[1]++];
                    if (!dfn[v]) enter(v);
                    else if (!bel[v]) low[u] = min(low[u], dfn[v]);
                    continue; // do not reuse f after enter(), which may reallocate
                }

                if (low[u] == dfn[u]) {
                    ++cnt;
                    int v;
                    do {
                        v = stk.back();
                        stk.pop_back();
                        bel[v] = cnt;
                    } while (v != u);
                }

                call.pop_back();
                if (!call.empty()) {
                    int p = call.back()[0];
                    low[p] = min(low[p], low[u]);
                }
            }
        }

        for (int u = 1; u <= n; ++u)
            bel[u] = cnt + 1 - bel[u];
        return cnt;
    }

    // ===== OPTIONAL: flat, owning component-member lists =====
    struct Groups {
        vector<int> vert, off;
        span<const int> operator[](int c) const {
            return span<const int>(vert).subspan(
                off[c - 1], off[c] - off[c - 1]
            );
        }
    };

    Groups groups() const {
        Groups g{vector<int>(n), vector<int>(cnt + 1)};
        for (int u = 1; u <= n; ++u)
            ++g.off[bel[u]];
        partial_sum(g.off.begin(), g.off.end(), g.off.begin());
        auto p = g.off;
        for (int u = n; u; --u)
            g.vert[--p[bel[u]]] = u;
        return g;
    }

    // ===== OPTIONAL: condensation. Depends on groups(). =====
    // unique=true removes parallel edges; false keeps their multiplicity.
    // Internal edges (including self-loops) are discarded in either mode.
    vector<vector<int>> compress(bool unique = true) const {
        vector<vector<int>> dag(cnt + 1);
        vector<int> seen(cnt + 1);
        auto vs = groups();
        for (int c = 1; c <= cnt; ++c)
            for (int u : vs[c])
                for (int v : adj[u]) {
                    int d = bel[v];
                    if (c != d && (!unique || seen[d] != c)) {
                        seen[d] = c;
                        dag[c].push_back(d);
                    }
                }
        return dag;
    }
};