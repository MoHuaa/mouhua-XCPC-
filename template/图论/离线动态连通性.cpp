// 依赖 RollbackDSU。点 1..n，时刻 0..q-1，add(L,R,u,v) 生效于 [L,R)。
// work(answer)：叶子回调 answer(time, dsu)，图结构不被消耗，可重新运行。
// 边的增删配对、重边语义由题目预处理；总时间 O(q+K log q log n)。
struct TimeGraph {
    int n, q, base = 1;
    vector<vector<pair<int, int>>> tr;
    TimeGraph(int n, int q) : n(n), q(q) {
        while (base < q) base <<= 1;
        tr.resize(2 * base);
    }
    void add(int l, int r, int u, int v) {
        assert(0 <= l && l <= r && r <= q);
        for (l += base, r += base; l < r; l >>= 1, r >>= 1) {
            if (l & 1) tr[l++].pb({u, v});
            if (r & 1) tr[--r].pb({u, v});
        }
    }
    void work(auto answer) const {
        RollbackDSU d(n);
        auto dfs = [&](auto&& self, int p) -> void {
            int s = d.snapshot();
            for (auto [u, v] : tr[p]) d.merge(u, v);
            if (p >= base) { if (p - base < q) answer(p - base, d); }
            else { self(self, p * 2); self(self, p * 2 + 1); }
            d.rollback(s);
        };
        dfs(dfs, 1);
    }
};
