template<class F = ll, class T = F>
struct Dinic {
    struct Edge { int to, rev; F cap; };
    int n, s, t;
    vector<vector<Edge>> g;
    vector<int> h, cur, que;
    Dinic(int n) : n(n), g(n), h(n), cur(n), que(n) {}
    int addEdge(int u, int v, F cap) {
        int i = g[u].size(), j = g[v].size();
        g[u].pb({v, j + (u == v), cap});
        g[v].pb({u, i, 0});
        return i;
    }
    bool bfs() {
        fill(h.begin(), h.end(), n);
        int l = 0, r = 0;
        h[s] = 0; que[r++] = s;
        while (l < r) {
            int u = que[l++];
            for (auto &x : g[u]) if (x.cap && h[x.to] == n) {
                h[x.to] = h[u] + 1;
                if (x.to == t) return true;
                que[r++] = x.to;
            }
        }
        return false;
    }
    T dfs(int u, T up) {
        if (u == s) return up;
        T res = 0;
        for (int &i = cur[u]; i < int(g[u].size()); i++) {
            auto &x = g[u][i];
            auto &y = g[x.to][x.rev];
            if (!y.cap || h[u] != h[x.to] + 1) continue;
            T f = dfs(x.to, min(up - res, T(y.cap)));
            x.cap += f; y.cap -= f; res += f;
            if (res == up) return res;
        }
        h[u] = n;
        return res;
    }
    T flow(int source, int sink, T limit = numeric_limits<T>::max()) {
        s = source; t = sink;
        T f = 0;
        while (f < limit && bfs()) {
            fill(cur.begin(), cur.end(), 0);
            f += dfs(t, limit - f);
        }
        return f;
    }
    vector<int> minCut() {
        vector<int> cut(n);
        int l = 0, r = 0;
        cut[s] = 1; que[r++] = s;
        while (l < r) {
            int u = que[l++];
            for (auto &x : g[u]) if (x.cap && !cut[x.to])
                cut[x.to] = 1, que[r++] = x.to;
        }
        return cut;
    }
};