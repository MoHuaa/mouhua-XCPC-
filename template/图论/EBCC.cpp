// 无向图，点 1..n，边 ID 从 0 开始；支持重边、自环、不连通图。
// work 后 bridge[id] 判桥，bel[u] 为 1..cnt 的边双编号；O(n+m)。
struct EBCC {
    int n, cnt = 0;
    vector<pair<int, int>> edges;
    vector<vector<pair<int, int>>> e;
    vector<int> dfn, low, bel;
    vector<char> bridge;
    EBCC(int n = 0) : n(n), e(n + 1) {}
    int addEdge(int u, int v) {
        int id = edges.size(); edges.pb({u, v});
        e[u].pb({v, id}); e[v].pb({u, id});
        return id;
    }
    int work() {
        dfn.assign(n + 1, 0); low = bel = dfn;
        bridge.assign(edges.size(), 0); cnt = 0;
        vector<int> par(n + 1), pe(n + 1, -1), it(n + 1), st;
        int tim = 0;
        for (int s = 1; s <= n; ++s) if (!dfn[s]) {
            st = {s}; dfn[s] = low[s] = ++tim;
            while (!st.empty()) {
                int u = st.back();
                if (it[u] < int(e[u].size())) {
                    auto [v, id] = e[u][it[u]++];
                    if (id == pe[u]) continue;
                    if (dfn[v]) low[u] = min(low[u], dfn[v]);
                    else {
                        par[v] = u; pe[v] = id;
                        dfn[v] = low[v] = ++tim; st.pb(v);
                    }
                } else {
                    st.pop_back();
                    if (par[u]) {
                        bridge[pe[u]] = low[u] > dfn[par[u]];
                        low[par[u]] = min(low[par[u]], low[u]);
                    }
                }
            }
        }
        for (int s = 1; s <= n; ++s) if (!bel[s]) {
            bel[s] = ++cnt; st = {s};
            while (!st.empty()) {
                int u = st.back(); st.pop_back();
                for (auto [v, id] : e[u]) if (!bridge[id] && !bel[v])
                    bel[v] = cnt, st.pb(v);
            }
        }
        return cnt;
    }
    // 可选：桥森林，边保存 {相邻边双编号, 原边 ID}。
    auto compress() const {
        vector<vector<pair<int, int>>> g(cnt + 1);
        for (int i = 0; i < int(edges.size()); ++i) if (bridge[i]) {
            auto [u, v] = edges[i]; u = bel[u]; v = bel[v];
            g[u].pb({v, i}); g[v].pb({u, i});
        }
        return g;
    }
};
