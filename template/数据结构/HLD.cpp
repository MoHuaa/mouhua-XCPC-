struct HLD {
    int n = 0, root = 0;
    std::vector<std::vector<int>> e;
    std::vector<int> sz, son, top, dep, parent, in, seq;

    HLD(int n = 0) { init(n); }
    void init(int m) {
        n = m;
        root = 0;
        e.assign(n + 1, {});
        sz.assign(n + 1, 0); son = top = dep = parent = in = seq = sz;
    }
    void addEdge(int u, int v) {
        e[u].push_back(v);
        e[v].push_back(u);
    }

    // Iterative build. Preserves both the edges and their adjacency-list order.
    // seq first holds parent-before-child order; afterwards it is inverse DFN.
    void build(int rt = 1) {
        if (!n) { root = 0; return; }
        assert(1 <= rt && rt <= n);
        root = rt;
        parent[rt] = dep[rt] = 0;
        seq[1] = rt;
        int cnt = 1;
        for (int i = 1; i <= cnt; ++i) {
            int u = seq[i];
            sz[u] = 1; son[u] = 0;
            for (int v : e[u]) if (v != parent[u]) {
                parent[v] = u;
                dep[v] = dep[u] + 1;
                seq[++cnt] = v;
            }
        }
        for (int i = n; i > 1; --i) {
            int u = seq[i], p = parent[u];
            sz[p] += sz[u];
            if (sz[u] >= sz[son[p]]) son[p] = u;
        }
        std::vector<int> st{rt};
        cnt = 0;
        while (!st.empty()) {
            int t = st.back(); st.pop_back();
            for (int u = t; u; u = son[u]) {
                top[u] = t;
                in[u] = ++cnt; seq[cnt] = u;
                for (int v : e[u])
                    if (v != parent[u] && v != son[u]) st.push_back(v);
            }
        }
    }

    int lca(int u, int v) const {
        while (top[u] != top[v]) {
            if (dep[top[u]] < dep[top[v]]) std::swap(u, v);
            u = parent[top[u]];
        }
        return dep[u] < dep[v] ? u : v;
    }
    std::pair<int, int> subtree(int u, bool edge = false) const {
        return {in[u] + int(edge), in[u] + sz[u] - 1};
    }

    // Unordered disjoint path intervals. Use for commutative aggregation,
    // or the same position-independent pointwise update on the entire path.
    // edge=true: edge (parent[x],x) lives at in[x]; omit the LCA slot only.
    template<class F>
    void path(int u, int v, F op, bool edge = false) const {
        while (top[u] != top[v]) {
            if (dep[top[u]] < dep[top[v]]) std::swap(u, v);
            op(in[top[u]], in[u]);
            u = parent[top[u]];
        }
        if (dep[u] > dep[v]) std::swap(u, v);
        if (in[u] + int(edge) <= in[v])
            op(in[u] + int(edge), in[v]);
    }

    // OPTIONAL: ordered aggregation from original u to original v.
    // query(l,r) returns an Info value in increasing DFN order.
    // Info{} is identity; + is associative; rev reverses the sequence:
    // rev(a+b)=rev(b)+rev(a), rev(rev(a))=a, rev(Info{})=Info{}.
    template<class Q, class R>
    auto fold(int u, int v, Q query, R rev, bool edge = false) const {
        using Info = decltype(query(1, 1));
        Info left{}, right{};
        while (top[u] != top[v]) {
            if (dep[top[u]] > dep[top[v]]) {
                left = left + rev(query(in[top[u]], in[u]));
                u = parent[top[u]];
            } else {
                right = query(in[top[v]], in[v]) + right;
                v = parent[top[v]];
            }
        }
        if (dep[u] > dep[v]) {
            int l = in[v] + int(edge);
            if (l <= in[u]) left = left + rev(query(l, in[u]));
        } else {
            int l = in[u] + int(edge);
            if (l <= in[v]) right = query(l, in[v]) + right;
        }
        return left + right;
    }
};
