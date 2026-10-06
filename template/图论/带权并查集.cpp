// 点 1..n；d[x] 表示 value[x]-value[parent[x]]，find 后相对根。
// merge(u,v,w) 加入 value[v]-value[u]=w，返回约束是否一致（不是是否合并）。
// diff(u,v) 同根时返回 value[v]-value[u]，否则 nullopt；中间量需不溢出。
template<class T = ll>
struct WeightedDSU {
    vector<int> fa, sz;
    vector<T> d;
    WeightedDSU(int n = 0) : fa(n + 1), sz(n + 1, 1), d(n + 1) {
        iota(fa.begin(), fa.end(), 0);
    }
    int find(int x) {
        if (fa[x] == x) return x;
        int p = fa[x]; fa[x] = find(p); d[x] = d[x] + d[p];
        return fa[x];
    }
    bool merge(int u, int v, T w) {
        int x = find(u), y = find(v);
        w = w + d[u] - d[v];
        if (x == y) return w == T{};
        if (sz[x] < sz[y]) swap(x, y), w = -w;
        fa[y] = x; d[y] = w; sz[x] += sz[y];
        return true;
    }
    optional<T> diff(int u, int v) {
        if (find(u) != find(v)) return nullopt;
        return d[v] - d[u];
    }
};
