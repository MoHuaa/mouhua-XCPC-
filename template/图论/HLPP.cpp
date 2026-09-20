template<class F = ll, class T = F>
struct HLPP {
    struct Edge { int to, rev; F cap; };
    int n, s, t, hi, top, work;
    vector<vector<Edge>> g;
    vector<int> h, cur, ah, an, gh, gp, gn, q;
    vector<T> ex;
    HLPP(int n) : n(n), g(n), h(n), cur(n), ah(n), an(n),
                  gh(n), gp(n), gn(n), q(n), ex(n) {}
    int addEdge(int u, int v, F cap) {
        int i = g[u].size(), j = g[v].size();
        g[u].pb({v, j + (u == v), cap});
        g[v].pb({u, i, 0});
        return i;
    }
    void active(int u) {
        if (u != s && u != t && ex[u] > 0 && h[u] < n) {
            an[u] = ah[h[u]]; ah[h[u]] = u;
            chmax(hi, h[u]);
        }
    }
    void gadd(int u) {
        int k = h[u];
        gp[u] = -1; gn[u] = gh[k];
        if (gn[u] >= 0) gp[gn[u]] = u;
        gh[k] = u; chmax(top, k);
    }
    void gdel(int u) {
        int p = gp[u], v = gn[u];
        if (p < 0) gh[h[u]] = v; else gn[p] = v;
        if (v >= 0) gp[v] = p;
    }
    void global() {
        work = 0; hi = top = -1;
        fill(h.begin(), h.end(), n);
        fill(ah.begin(), ah.end(), -1);
        fill(gh.begin(), gh.end(), -1);
        int l = 0, r = 0;
        h[t] = 0; q[r++] = t;
        while (l < r) {
            int u = q[l++];
            for (auto &x : g[u]) {
                int v = x.to;
                if (v != s && h[v] == n && g[v][x.rev].cap) {
                    h[v] = h[u] + 1; cur[v] = x.rev;
                    q[r++] = v;
                }
            }
        }
        for (int u = 0; u < n; u++)
            if (u != s && u != t && h[u] < n) gadd(u), active(u);
    }
    void discharge(int u) {
        int p = cur[u], r = g[u].size(), hu = h[u], nh = n, best = 0;
        for (int z = 0; z < 2; z++)
            for (int i = z ? 0 : p, end = z ? p : r; i < end; i++) {
                auto &x = g[u][i];
                if (!x.cap) continue;
                int v = x.to;
                if (hu == h[v] + 1) {
                    T f = min(ex[u], T(x.cap));
                    bool zero = ex[v] == 0;
                    x.cap -= f; g[v][x.rev].cap += f;
                    ex[u] -= f; ex[v] += f;
                    if (zero) active(v);
                    if (!ex[u]) { cur[u] = i; return; }
                } else if (h[v] + 1 < nh) nh = h[v] + 1, best = i;
            }
        ++work;
        if (gh[hu] == u && gn[u] < 0) {
            for (int k = hu; k <= top; k++) {
                for (int v = gh[k]; v >= 0; v = gn[v]) h[v] = n;
                gh[k] = ah[k] = -1;
            }
            hi = top = hu - 1;
        } else {
            gdel(u); h[u] = nh; cur[u] = best;
            if (nh < n) gadd(u), active(u);
        }
    }
    T flow(int source, int sink) {
        s = source; t = sink;
        T before = ex[t];
        for (auto &x : g[s]) if (x.to != s && x.cap) {
            T f = x.cap;
            x.cap = 0; g[x.to][x.rev].cap += f;
            ex[s] -= f; ex[x.to] += f;
        }
        global();
        while (true) {
            while (hi >= 0 && ah[hi] < 0) hi--;
            if (hi < 0) break;
            int u = ah[hi]; ah[hi] = an[u];
            if (ex[u] <= 0 || h[u] != hi) continue;
            discharge(u);
            if (work > 4 * n) global();
        }
        return ex[t] - before;
    }
    vector<int> minCut() {
        global();
        vector<int> cut(n);
        for (int u = 0; u < n; u++) cut[u] = h[u] == n;
        return cut;
    }
};