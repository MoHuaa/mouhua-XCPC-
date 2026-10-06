// 固定无权树，点编号 1..n；先 addEdge，再 build(a)，a[1..n]，省略则全 0。
// add(u,v) 加 v，set(u,v) 赋值；query(u,k) 求距离 <= k 的点权和。
// T{} 为零，T 支持交换的 + 和减法；权值、差值及中间和须能用 T 表示。
// 建树 O(n log n)，修改/查询 O(log^2 n)，空间 O(n log n)。普通树遍历不递归。
template<class T = ll>
struct Centroid {
    int n;
    vector<vector<int>> e;
    vector<vector<pair<int, int>>> path;
    vector<vector<T>> tr, sub;
    vector<T> a;
    vector<int> sz, fa, q;
    vector<char> vis;

    Centroid(int n = 0) { init(n); }
    void init(int m) {
        n = m;
        e.assign(n + 1, {}); path.assign(n + 1, {});
        tr.assign(n + 1, {}); sub.assign(n + 1, {});
        a.assign(n + 1, T{}); vis.assign(n + 1, 0);
        sz.resize(n + 1); fa.resize(n + 1); q.clear();
    }
    void addEdge(int u, int v) { e[u].pb(v); e[v].pb(u); }
    void pull(vector<T>& b) {
        for (int i = 1; i < int(b.size()); ++i) {
            int j = i + (i & -i);
            if (j < int(b.size())) b[j] = b[j] + b[i];
        }
    }
    void work(int rt) {
        q = {rt}; fa[rt] = 0;
        int md = 0;
        for (int i = 0; i < int(q.size()); ++i) {
            int u = q[i]; sz[u] = 1;
            if (!path[u].empty()) md = max(md, path[u].back().second);
            for (int v : e[u]) if (!vis[v] && v != fa[u])
                fa[v] = u, q.pb(v);
        }
        int c = rt, m = q.size();
        // 倒序第一个子树大小 >= ceil(m/2) 的点，其每一侧都不超过 m/2。
        for (int i = m - 1; i >= 0; --i) {
            int u = q[i];
            if (sz[u] >= (m + 1) / 2) { c = u; break; }
            sz[fa[u]] += sz[u];
        }
        sub[c].assign(md + 2, T{});
        q = {c}; fa[c] = 0; path[c].pb({c, 0});
        for (int i = 0; i < int(q.size()); ++i) {
            int u = q[i], d = path[u].back().second;
            for (int v : e[u]) if (!vis[v] && v != fa[u]) {
                fa[v] = u; path[v].pb({c, d + 1}); q.pb(v);
            }
        }
        // q 是 BFS 序，最后一个点的距离最大；q 在递归前已用完，可复用。
        tr[c].assign(path[q.back()].back().second + 2, T{});
        vis[c] = 1;
        for (int v : e[c]) if (!vis[v]) work(v);
    }
    void build(const vector<T>& values = {}) {
        assert(values.empty() || int(values.size()) == n + 1);
        for (auto& p : path) p.clear();
        fill(vis.begin(), vis.end(), 0);
        a = values; a.resize(n + 1);
        if (n) work(1);
        if (values.empty()) return;
        for (int u = 1; u <= n; ++u) {
            int last = 0;
            for (int i = int(path[u].size()) - 1; i >= 0; --i) {
                auto [c, d] = path[u][i];
                tr[c][d + 1] = tr[c][d + 1] + a[u];
                if (last) sub[last][d + 1] = sub[last][d + 1] + a[u];
                last = c;
            }
        }
        for (int u = 1; u <= n; ++u) { pull(tr[u]); pull(sub[u]); }
    }
    void change(vector<T>& b, int d, T v) {
        for (++d; d < int(b.size()); d += d & -d) b[d] = b[d] + v;
    }
    T sum(const vector<T>& b, ll d) const {
        T ans{};
        if (d < 0) return ans;
        for (int i = int(min<ll>(d, int(b.size()) - 2)) + 1; i; i -= i & -i)
            ans = ans + b[i];
        return ans;
    }
    void add(int u, T v) {
        a[u] = a[u] + v;
        int last = 0;
        for (int i = int(path[u].size()) - 1; i >= 0; --i) {
            auto [c, d] = path[u][i];
            change(tr[c], d, v);
            if (last) change(sub[last], d, v);
            last = c;
        }
    }
    void set(int u, T v) { add(u, v - a[u]); }
    T query(int u, ll k) const {
        T ans{};
        if (k < 0) return ans;
        int last = 0;
        // tr[c] 按到 c 的距离维护；sub[last] 按到 last 的点分父亲 c 的距离维护。
        for (int i = int(path[u].size()) - 1; i >= 0; --i) {
            auto [c, d] = path[u][i];
            ans = ans + sum(tr[c], k - d);
            if (last) ans = ans - sum(sub[last], k - d);
            last = c;
        }
        return ans;
    }
};
