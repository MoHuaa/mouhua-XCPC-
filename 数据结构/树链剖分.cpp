template<class SegmentTree, class Info>
struct Trh {
    std::vector<int> sz, top, dep, parent, in, out;
    int cur, n;
    SegmentTree seg;
    std::vector<std::vector<int>> e;
    Trh(int _n) : n(_n), sz(_n + 1), top(_n + 1), dep(_n + 1), parent(_n + 1, -1), e(_n + 1), in(_n + 1), cur(0), out(_n + 1) {
        seg.init(_n);
    }
    void addEdge(int u, int v) {
        e[u].push_back(v);
        e[v].push_back(u);
    }
    void init(int s) {
        dfsSz(s);
        dfsHLD(s);
    }
    void dfsSz(int u) {
        if (parent[u] != -1)
            e[u].erase(std::find(e[u].begin(), e[u].end(), parent[u]));
        sz[u] = 1;
        for (int &v : e[u]) {
            parent[v] = u;
            dep[v] = dep[u] + 1;
            dfsSz(v);
            sz[u] += sz[v];
            if (sz[v] > sz[e[u][0]])
                std::swap(v, e[u][0]);
        }
    }
    void dfsHLD(int u) {
        in[u] = ++cur;
        for (int v : e[u]) {
            if (v == e[u][0]) {
                top[v] = top[u];
            } else {
                top[v] = v;
            }
            dfsHLD(v);
        }
        out[u] = cur;
    }
    int lca(int u, int v) {
        while (top[u] != top[v]) {
            if (dep[top[u]] > dep[top[v]]) {
                u = parent[top[u]];
            } else {
                v = parent[top[v]];
            }
        }
        if (dep[u] < dep[v]) {
            return u;
        } else {
            return v;
        }
    }
    void change(int u, int v, ll add) {
        tag p{add};
        while (top[u] != top[v]) {
            if (dep[top[u]] > dep[top[v]]) {
                seg.change(1, 1, n, in[top[u]], in[u], p);
                u = parent[top[u]];
            } else {
                seg.change(1, 1, n, in[top[v]], in[v], p);
                v = parent[top[v]];
            }
        }
        if (dep[u] < dep[v]) {
            seg.change(1, 1, n, in[u], in[v], p);
        } else {
            seg.change(1, 1, n, in[v], in[u], p);
        }
    }
    node query(int u, int v) {
        node ans;
        while (top[u] != top[v]) {
            if (dep[top[u]] > dep[top[v]]) {
                ans = ans + seg.query(1, 1, n, in[top[u]], in[u]);
                u = parent[top[u]];
            } else {
                ans = ans + seg.query(1, 1, n, in[top[v]], in[v]);
                v = parent[top[v]];
            }
        }
        if (dep[u] < dep[v]) {
            ans = ans + seg.query(1, 1, n, in[u], in[v]);
        } else {
            ans = ans + seg.query(1, 1, n, in[v], in[u]);
        }
        return ans;
    }
};