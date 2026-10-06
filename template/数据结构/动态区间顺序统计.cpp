// a[1..n]；p 登记未来的 (位置, 值)，初值自动加入；set 只能使用登记点对。
// count 默认数 <x，upper=true 数 <=x；kth 从 1 开始，无解返回 nullopt。
// tr[u][i] = {位置, 频数}；内层 BIT 为 0 基，查询传前缀长度。
template<class T = int>
struct ValBIT {
    int n = 0, size = 1;
    vector<T> xs;
    vector<int> a;
    vector<vector<array<int, 2>>> tr;

    ValBIT() = default;
    ValBIT(const vector<T>& a, vector<pair<int, T>> p = {}) { init(a, move(p)); }
    void init(const vector<T>& v, vector<pair<int, T>> p = {}) {
        assert(!v.empty());
        n = int(v.size()) - 1;
        for (int i = 1; i <= n; ++i) p.pb({i, v[i]});
        sort(p.begin(), p.end());
        p.erase(unique(p.begin(), p.end()), p.end());
        xs.clear();
        for (auto [i, x] : p) xs.pb(x);
        sort(xs.begin(), xs.end());
        xs.erase(unique(xs.begin(), xs.end()), xs.end());
        size = 1;
        while (size < int(xs.size())) size <<= 1;
        a.assign(n + 1, 0); tr.assign(2 * size, {});
        for (auto [i, x] : p) {
            assert(1 <= i && i <= n);
            int k = lower_bound(xs.begin(), xs.end(), x) - xs.begin();
            if (x == v[i]) a[i] = k;
            for (int u = size + k; u > 1; u >>= 1) {
                if (tr[u].empty() || tr[u].back()[0] != i) tr[u].pb({i, 0});
                tr[u].back()[1] += x == v[i];
            }
        }
        for (int p = 2; p < 2 * size; ++p)
            for (int i = 0; i < int(tr[p].size()); ++i) {
                int j = i | (i + 1);
                if (j < int(tr[p].size())) tr[p][j][1] += tr[p][i][1];
            }
    }
    int pos(int p, int x, bool upper = false) const {
        const auto& t = tr[p];
        return (upper ? upper_bound(t.begin(), t.end(), x, [](int x, auto v) { return x < v[0]; })
                      : lower_bound(t.begin(), t.end(), x, [](auto v, int x) { return v[0] < x; })) - t.begin();
    }
    void add(int p, int x, int d) {
        for (int k = pos(p, x); k < int(tr[p].size()); k |= k + 1) tr[p][k][1] += d;
    }
    void set(int p, T v) {
        assert(1 <= p && p <= n);
        int k = lower_bound(xs.begin(), xs.end(), v) - xs.begin();
        assert(k < int(xs.size()) && xs[k] == v);
        if (a[p] == k) return;
        int x = a[p] + size, y = k + size, j = pos(y, p);
        assert(j < int(tr[y].size()) && tr[y][j][0] == p);
        a[p] = k;
        // 公共祖先中，位置 p 的频数始终为 1，不需要先删再加。
        for (; x != y; x >>= 1, y >>= 1) add(x, p, -1), add(y, p, 1);
    }
    int query(int p, int L, int R) const {
        int l = pos(p, L), r = pos(p, R, true), ans = 0;
        for (; r > l; r &= r - 1) ans += tr[p][r - 1][1];
        for (; l > r; l &= l - 1) ans -= tr[p][l - 1][1];
        return ans;
    }
    int count(int l, int r, T x, bool upper = false) const {
        if (l > r) return 0;
        assert(1 <= l && r <= n);
        int k = (upper ? upper_bound(xs.begin(), xs.end(), x)
                       : lower_bound(xs.begin(), xs.end(), x)) - xs.begin();
        if (k == int(xs.size())) return r - l + 1;
        int ans = 0;
        for (k += size; k > 1; k >>= 1)
            if (k & 1) ans += query(k - 1, l, r);
        return ans;
    }
    optional<T> kth(int l, int r, int k) const {
        if (k <= 0 || k > r - l + 1) return nullopt;
        assert(1 <= l && r <= n);
        int p = 1;
        while (p < size) {
            int cnt = query(p * 2, l, r);
            p *= 2;
            if (k > cnt) k -= cnt, ++p;
        }
        return xs[p - size];
    }
    int rank(int l, int r, T x) const { return count(l, r, x) + 1; }
    optional<T> pre(int l, int r, T x) const { return kth(l, r, count(l, r, x)); }
    optional<T> next(int l, int r, T x) const { return kth(l, r, count(l, r, x, true) + 1); }
};
