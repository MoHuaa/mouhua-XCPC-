template<class F = ll, class C = ll,
         class K = conditional_t<is_floating_point_v<C>, ll, C>>
struct MCF {
    using P = pair<K, int>;
    static constexpr K INF = numeric_limits<K>::max() / 4;
    struct Edge { int to; F cap; K cost; };
    int n;
    C unit;
    vector<Edge> e;
    vector<vector<int>> g;
    vector<int> cur;
    vector<K> h;
    vector<C> raw; // 原始费用；e 中保存工作费用
    vector<F> ex;
    MCF(int n, C unit = 0) : n(n), unit(unit), g(n), cur(n), h(n), ex(n) {}
    int addEdge(int u, int v, F cap, C cost) {
        int id = e.size();
        g[u].pb(id); e.pb({v, cap, 0});
        g[v].pb(id ^ 1); e.pb({u, 0, 0});
        raw.pb(cost); raw.pb(-cost);
        return id;
    }
    void push(int id, F f) {
        e[id].cap -= f; e[id ^ 1].cap += f;
        ex[e[id ^ 1].to] -= f; ex[e[id].to] += f;
    }
    void global(K eps) {
        vector<K> d(n, INF);
        std::priority_queue<P, vector<P>, greater<P>> q;
        int active = 0;
        for (int i = 0; i < n; i++) {
            if (ex[i] > 0) active++;
            if (ex[i] < 0) d[i] = 0, q.emplace(0, i);
        }
        if (!active) return;
        K last = 0;
        while (!q.empty()) {
            auto [du, u] = q.top(); q.pop();
            if (du != d[u]) continue;
            last = du;
            if (ex[u] > 0 && --active == 0) break;
            for (int id : g[u]) if (e[id ^ 1].cap) {
                int v = e[id].to;
                K nd = du + e[id ^ 1].cost + h[v] - h[u] + eps;
                if (nd < d[v]) d[v] = nd, q.emplace(nd, v);
            }
        }
        for (int i = 0; i < n; i++) h[i] -= min(d[i], last);
        fill(cur.begin(), cur.end(), 0);
    }
    bool look(int u, K eps) {
        for (int &j = cur[u]; j < int(g[u].size()); j++) {
            int id = g[u][j];
            if (e[id].cap && e[id].cost + h[u] < h[e[id].to]) return true;
        }
        K best = -INF;
        for (int id : g[u]) if (e[id].cap) chmax(best, h[e[id].to] - e[id].cost);
        h[u] = (best == -INF ? h[u] : best) - eps;
        cur[u] = 0;
        return false;
    }
    void optimize(K eps) {
        while (eps > 1) {
            eps = max(K(1), K(eps / 32));
            fill(cur.begin(), cur.end(), 0);
            for (int id = 0; id < int(e.size()); id++)
                if (e[id].cap && e[id].cost + h[e[id ^ 1].to] < h[e[id].to]) push(id, e[id].cap);
            int relabels = 0;
            queue<int> q;
            for (int i = 0; i < n; i++) if (ex[i] > 0) q.push(i);
            while (!q.empty()) {
                int u = q.front(); q.pop();
                while (ex[u] > 0) {
                    if (relabels >= n) global(eps), relabels = 0;
                    if (!look(u, eps)) { relabels++; continue; }
                    int id = g[u][cur[u]], v = e[id].to;
                    if (ex[v] >= 0 && !look(v, eps)) relabels++;
                    if (e[id].cost + h[u] < h[v]) {
                        F x = min(ex[u], e[id].cap);
                        if (ex[v] <= 0 && ex[v] + x > 0) q.push(v);
                        push(id, x);
                    } else cur[u]++;
                }
            }
        }
    }
    template<class R = C>
    pair<F, R> flow(int s, int t, F limit = numeric_limits<F>::max()) {
        for (auto &a : g) stable_partition(a.begin(), a.end(), [&](int id) {
            return e[id].cap && raw[id] <= 0;
        });
        int id = addEdge(t, s, limit, 0);
        e[id].cost = -(K(n) + 1); e[id ^ 1].cost = K(n) + 1;
        optimize(K(2));
        F f = e[id ^ 1].cap;
        g[t].pop_back(); g[s].pop_back(); e.resize(id); raw.resize(id);
        fill(h.begin(), h.end(), 0);
        K eps = 0;
        for (int i = 0; i < id; i++) {
            K c;
            if constexpr (is_floating_point_v<C>) c = K(round((long double)raw[i] / unit));
            else c = raw[i];
            e[i].cost = c * (K(n) + 1);
            if (e[i].cap) chmax(eps, -e[i].cost);
        }
        optimize(eps);
        R ans = 0;
        for (int i = 0; i < id; i += 2) ans += R(e[i ^ 1].cap) * R(raw[i]);
        return {f, ans};
    }
};