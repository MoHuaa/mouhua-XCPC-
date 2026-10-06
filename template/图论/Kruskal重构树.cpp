// 依赖 DSU；原点 1..n，合并点 n+1..tot，按边权升序。
// roots 保留森林；same(u,v) 判原图连通；ch[u] 为两个孩子，val[u] 为合并权。
// 相同点/不同连通块的瓶颈另判；仅内部点的 val 有瓶颈语义。O(m log m)。
template<class T = ll>
struct KruskalTree {
    struct Edge { int u, v; T w; };
    int n = 0, tot = 0;
    vector<array<int, 2>> ch;
    vector<T> val;
    vector<int> roots, bel;
    void build(int m, vector<Edge> e) {
        n = tot = m;
        ch.assign(2 * n + 1, {}); val.assign(2 * n + 1, T{});
        roots.clear(); bel.resize(n + 1);
        DSU d(n);
        vector<int> rt(n + 1); iota(rt.begin(), rt.end(), 0);
        sort(e.begin(), e.end(), [](const Edge& a, const Edge& b) { return a.w < b.w; });
        for (auto [u, v, w] : e) {
            int x = d.find(u), y = d.find(v);
            if (x == y) continue;
            ch[++tot] = {rt[x], rt[y]}; val[tot] = w;
            d.merge(x, y); rt[d.find(x)] = tot;
        }
        for (int u = 1; u <= n; ++u) {
            bel[u] = d.find(u);
            if (bel[u] == u) roots.pb(rt[u]);
        }
        ch.resize(tot + 1); val.resize(tot + 1);
    }
    bool same(int u, int v) const { return bel[u] == bel[v]; }
};
