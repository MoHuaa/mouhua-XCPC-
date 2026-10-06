// 左 1..n，右 1..m；HK 最大匹配，O((n+m+E)sqrt(n+m))；DFS 用显式栈。
// work 从空匹配求解；matchL/matchR 为 0 表示未匹配；允许重复边。
struct HK {
    int n, m, lim;
    vector<vector<int>> e;
    vector<int> matchL, matchR, dis, cur, q, st;
    HK(int n, int m) : n(n), m(m), e(n + 1), matchL(n + 1), matchR(m + 1),
        dis(n + 1), cur(n + 1) {}
    void addEdge(int u, int v) { e[u].pb(v); }
    bool bfs() {
        fill(dis.begin(), dis.end(), -1); q.clear(); lim = n + 1;
        for (int u = 1; u <= n; ++u) if (!matchL[u]) dis[u] = 0, q.pb(u);
        for (int i = 0; i < int(q.size()); ++i) {
            int u = q[i]; if (dis[u] >= lim) continue;
            for (int v : e[u]) {
                int w = matchR[v];
                if (!w) lim = min(lim, dis[u] + 1);
                else if (dis[w] < 0) dis[w] = dis[u] + 1, q.pb(w);
            }
        }
        return lim <= n;
    }
    bool dfs(int rt) {
        st = {rt};
        while (!st.empty()) {
            int u = st.back(); bool down = false;
            while (cur[u] < int(e[u].size())) {
                int v = e[u][cur[u]++], w = matchR[v];
                if (!w && dis[u] + 1 == lim) {
                    for (int i = int(st.size()) - 1; i >= 0; --i) {
                        int x = st[i], old = matchL[x];
                        matchL[x] = v; matchR[v] = x; v = old;
                    }
                    return true;
                }
                if (w && dis[w] == dis[u] + 1 && dis[w] < lim) {
                    st.pb(w); down = true; break;
                }
            }
            if (!down) dis[u] = -1, st.pop_back();
        }
        return false;
    }
    int work() {
        fill(matchL.begin(), matchL.end(), 0); fill(matchR.begin(), matchR.end(), 0);
        int ans = 0;
        while (bfs()) {
            fill(cur.begin(), cur.end(), 0);
            for (int u = 1; u <= n; ++u) if (!matchL[u] && dis[u] == 0) ans += dfs(u);
        }
        return ans;
    }
    // 可选：work 后最小点覆盖，返回 {左点列表, 右点列表}。
    auto min_cover() const {
        vector<char> a(n + 1), b(m + 1);
        vector<int> q, L, R;
        for (int u = 1; u <= n; ++u) if (!matchL[u]) a[u] = 1, q.pb(u);
        for (int i = 0; i < int(q.size()); ++i) {
            int u = q[i];
            for (int v : e[u]) if (matchL[u] != v && !b[v]) {
                b[v] = 1; int w = matchR[v];
                if (w && !a[w]) a[w] = 1, q.pb(w);
            }
        }
        for (int u = 1; u <= n; ++u) if (!a[u]) L.pb(u);
        for (int v = 1; v <= m; ++v) if (b[v]) R.pb(v);
        return pair{L, R};
    }
};
