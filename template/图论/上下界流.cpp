// 依赖当前 Dinic；点 1..n，0<=lo<=hi，容量和总流量须能放入 F。
// addEdge 返回原边 ID；每对象只 solve 一次，成功后 get(id) 还原流量。
// solve() 循环可行流；solve(s,t,0) 非负可行流，mode=1 最大，-1 最小非负流。
// 失败 nullopt；不修改 Dinic，不要求 addNode，不使用手填 INF。
template<class F = ll>
struct BoundFlow {
    struct Edge { int u, id; F lo; };
    int n;
    bool done = false;
    Dinic<F> f;
    vector<F> d;
    vector<Edge> edges;
    BoundFlow(int n) : n(n), f(n + 3), d(n + 1) {}
    int addEdge(int u, int v, F lo, F hi) {
        assert(!done && F{} <= lo && lo <= hi);
        d[u] -= lo; d[v] += lo;
        edges.pb({u, f.addEdge(u, v, hi - lo), lo});
        return int(edges.size()) - 1;
    }
    F get(int id) const {
        auto [u, k, lo] = edges[id]; const auto& e = f.g[u][k];
        return lo + f.g[e.to][e.rev].cap;
    }
    void erase(int u, int k) {
        auto& e = f.g[u][k]; f.g[e.to][e.rev].cap = 0; e.cap = 0;
    }
    optional<F> solve(int s = 0, int t = 0, int mode = 0) {
        assert(!done && -1 <= mode && mode <= 1);
        assert((!s && !t && !mode) || (1 <= s && s <= n && 1 <= t && t <= n && s != t));
        done = true;
        int ss = n + 1, tt = n + 2, back = -1;
        F need{};
        for (int u = 1; u <= n; ++u) if (d[u] > F{}) need += d[u];
        if (s) back = f.addEdge(t, s, need);
        for (int u = 1; u <= n; ++u) {
            if (d[u] > F{}) f.addEdge(ss, u, d[u]);
            if (d[u] < F{}) f.addEdge(u, tt, -d[u]);
        }
        if (f.flow(ss, tt, need) != need) return nullopt;
        F ans{};
        if (s) {
            auto e = f.g[t][back]; ans = f.g[e.to][e.rev].cap;
            erase(t, back);
        }
        for (int k = 0; k < int(f.g[ss].size()); ++k) erase(ss, k);
        for (int k = 0; k < int(f.g[tt].size()); ++k) erase(tt, k);
        if (s && mode > 0) ans += f.flow(s, t);
        if (s && mode < 0) ans -= f.flow(t, s, ans);
        return ans;
    }
};
