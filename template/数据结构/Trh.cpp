template<class SegmentTree, class Info>
struct Trh : HLD {
    SegmentTree seg;
    using HLD::HLD;
    using HLD::build;

    // a[u] is a COMPLETE leaf Info (not Info{} for a zero-valued leaf).
    // For edge weights, a[u] describes (parent[u],u) under this build's root;
    // the root gets Info{}. Rebuilding requires rebuilding/reindexing seg too.
    void init_values(const std::vector<Info>& a) {
        assert((int)a.size() == n + 1);
        assert(!n || root != 0);
        std::vector<Info> b(n + 1);
        for (int u = 1; u <= n; ++u) b[in[u]] = a[u];
        seg.init(b);
    }
    void build(const std::vector<Info>& a, int rt = 1) {
        HLD::build(rt);
        init_values(a);
    }
    template<class Tag>
    void change(int u, int v, Tag t, bool edge = false) {
        path(u, v, [&](int l, int r) { seg.change(l, r, t); }, edge);
    }
    Info query(int u, int v, bool edge = false) {
        Info ans{};
        path(u, v, [&](int l, int r) { ans = ans + seg.query(l, r); }, edge);
        return ans;
    }
    // Optional ordered query; the explicit reverse function prevents a
    // silent assumption that a noncommutative Info can be reversed for free.
    template<class R>
    Info query(int u, int v, R rev, bool edge = false) {
        return fold(u, v, [&](int l, int r) { return seg.query(l, r); }, rev, edge);
    }
};