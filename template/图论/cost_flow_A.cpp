//小流量A，大流量B
template<class F = ll, class C = ll>
struct MCF {
    struct Edge { int to; F cap; C cost; };
    static constexpr C INF = numeric_limits<C>::max() / 4;
    int n, r;
    vector<Edge> e;
    vector<vector<int>> g;
    vector<C> h;
    vector<int> lev, cur, que;
    MCF(int n) : n(n), g(n), h(n), lev(n), cur(n), que(n) {}
    int addEdge(int u, int v, F cap, C cost) {
        int id = e.size();
        g[u].pb(id); e.pb({v, cap, cost});
        g[v].pb(id ^ 1); e.pb({u, 0, -cost});
        return id;
    }
    void init(int t) {
        if (none_of(e.begin(), e.end(), [](auto &x) { return x.cap && x.cost < 0; })) {
            fill(h.begin(), h.end(), 0); return;
        }
        fill(h.begin(), h.end(), INF);
        vector<char> in(n);
        queue<int> q;
        h[t] = 0; q.push(t); in[t] = 1;
        while (!q.empty()) {
            int u = q.front(); q.pop(); in[u] = 0;
            for (int id : g[u]) if (e[id ^ 1].cap) {
                int v = e[id].to;
                C nd = h[u] + e[id ^ 1].cost;
                if (nd < h[v]) {
                    h[v] = nd;
                    if (!in[v]) q.push(v), in[v] = 1;
                }
            }
        }
    }
    bool bfs(int s, int t) {
        fill(lev.begin(), lev.end(), -1);
        int l = 0; r = 0;
        que[r++] = s; lev[s] = 0;
        while (l < r) {
            int u = que[l++];
            for (int id : g[u]) {
                auto [v, cap, cost] = e[id];
                if (cap && lev[v] < 0 && h[v] != INF && h[u] == cost + h[v]) {
                    lev[v] = lev[u] + 1;
                    if (v == t) return true;
                    que[r++] = v;
                }
            }
        }
        return false;
    }
    F dfs(int u, int t, F f) {
        if (u == t) return f;
        F res = 0;
        for (int &j = cur[u]; j < int(g[u].size()); j++) {
            int id = g[u][j], v = e[id].to;
            if (!e[id].cap || lev[v] != lev[u] + 1 || h[u] != e[id].cost + h[v]) continue;
            F x = dfs(v, t, min(f - res, e[id].cap));
            e[id].cap -= x; e[id ^ 1].cap += x; res += x;
            if (res == f) return res;
        }
        lev[u] = -1;
        return res;
    }
    bool dual(int s, int t) {
        vector<C> d(n, INF);
        using P = pair<C, int>;
        std::priority_queue<P, vector<P>, greater<P>> q;
        d[t] = 0; q.emplace(0, t);
        while (!q.empty()) {
            auto [du, u] = q.top(); q.pop();
            if (du != d[u]) continue;
            if (u == s) break;
            for (int id : g[u]) if (e[id ^ 1].cap) {
                int v = e[id].to;
                if (h[v] == INF) continue;
                C nd = du + e[id ^ 1].cost + h[u] - h[v];
                if (nd < d[v]) d[v] = nd, q.emplace(nd, v);
            }
        }
        if (d[s] == INF) return false;
        for (int u = 0; u < n; u++) if (h[u] != INF) h[u] += min(d[u], d[s]);
        return true;
    }
    template<class R = C>
    pair<F, R> flow(int s, int t, F limit = numeric_limits<F>::max()) {
        init(t);
        F f = 0; R cost = 0;
        if (h[s] == INF) return {0, 0};
        int stuck = 0;
        while (f < limit) {
            if (bfs(s, t)) {
                fill(cur.begin(), cur.end(), 0);
                F x = dfs(s, t, limit - f);
                f += x; cost += R(x) * R(h[s]); stuck = 0;
            } else if (++stuck == 4) {
                if (!dual(s, t)) break;
                stuck = 0;
            } else {
                C delta = INF;
                for (int k = 0; k < r; k++) for (int id : g[que[k]]) {
                    auto [v, cap, w] = e[id];
                    if (cap && lev[v] < 0 && h[v] != INF)
                        chmin(delta, w + h[v] - h[que[k]]);
                }
                if (delta == INF) break;
                for (int k = 0; k < r; k++) h[que[k]] += delta;
            }
        }
        return {f, cost};
    }
};
