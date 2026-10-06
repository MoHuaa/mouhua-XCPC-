// 点 1..n，按大小合并，无路径压缩；find/merge O(log n)。
// merge 返回是否真的合并；snapshot 为历史栈长度，rollback 恢复到快照。
struct RollbackDSU {
    int cnt;
    vector<int> p;
    vector<array<int, 4>> hist;
    RollbackDSU(int n = 0) : cnt(n), p(n + 1, -1) {}
    int find(int x) const { while (p[x] >= 0) x = p[x]; return x; }
    bool same(int u, int v) const { return find(u) == find(v); }
    int size(int x) const { return -p[find(x)]; }
    int snapshot() const { return hist.size(); }
    bool merge(int u, int v) {
        u = find(u); v = find(v);
        if (u == v) return false;
        if (p[u] > p[v]) swap(u, v);
        hist.pb({u, p[u], v, p[v]});
        p[u] += p[v]; p[v] = u; --cnt;
        return true;
    }
    void rollback(int s) {
        assert(0 <= s && s <= int(hist.size()));
        while (int(hist.size()) > s) {
            auto [u, a, v, b] = hist.back(); hist.pop_back();
            p[u] = a; p[v] = b; ++cnt;
        }
    }
};
