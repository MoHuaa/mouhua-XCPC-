// 点 1..n，边 ID 从 0 开始；允许重边、自环；O(n+m)，显式栈。
// Euler<true> 有向，Euler<false> 无向。work(s=0) 自动选起点；无解 nullopt。
// 返回 vert 和 edge 序列；不修改原图。空图 n>0 返回一个起点，n=0 返回空序列。
template<bool directed = false>
struct Euler {
    struct Result { vector<int> vert, edge; };
    int n, m = 0;
    vector<vector<pair<int, int>>> e;
    vector<int> in, out;
    Euler(int n = 0) : n(n), e(n + 1), in(n + 1), out(n + 1) {}
    int addEdge(int u, int v) {
        e[u].pb({v, m}); ++out[u]; ++in[v];
        if (!directed) e[v].pb({u, m}), ++out[v], ++in[u];
        return m++;
    }
    optional<Result> work(int s = 0) const {
        assert(0 <= s && s <= n);
        int start = 0, finish = 0;
        vector<int> odd;
        for (int u = 1; u <= n; ++u) {
            if constexpr (directed) {
                int d = out[u] - in[u];
                if (d == 1) { if (start) return nullopt; start = u; }
                else if (d == -1) { if (finish) return nullopt; finish = u; }
                else if (d) return nullopt;
            } else if (out[u] & 1) odd.pb(u);
        }
        if constexpr (directed) {
            if (bool(start) != bool(finish)) return nullopt;
            if (start && s && s != start) return nullopt;
            if (!s) s = start;
        } else {
            if (!odd.empty() && odd.size() != 2) return nullopt;
            if (!odd.empty()) {
                if (s && s != odd[0] && s != odd[1]) return nullopt;
                if (!s) s = odd[0];
            }
        }
        if (!s) for (int u = 1; u <= n; ++u) if (out[u]) { s = u; break; }
        if (!m) return Result{s ? vector<int>{s} : (n ? vector<int>{1} : vector<int>{}), {}};
        if (!s || !out[s]) return nullopt;
        vector<int> cur(n + 1);
        vector<char> used(m);
        vector<pair<int, int>> st{{s, -1}};
        Result ans;
        while (!st.empty()) {
            auto [u, id] = st.back();
            while (cur[u] < int(e[u].size()) && used[e[u][cur[u]].second]) ++cur[u];
            if (cur[u] == int(e[u].size())) {
                ans.vert.pb(u); if (id != -1) ans.edge.pb(id); st.pop_back();
            } else {
                auto [v, k] = e[u][cur[u]++]; used[k] = 1; st.pb({v, k});
            }
        }
        if (int(ans.edge.size()) != m) return nullopt;
        reverse(ans.vert.begin(), ans.vert.end());
        reverse(ans.edge.begin(), ans.edge.end());
        return ans;
    }
};
