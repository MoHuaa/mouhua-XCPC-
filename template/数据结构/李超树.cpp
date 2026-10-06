// 预登记查询横坐标，内部排序去重；只添加直线，不支持删除/改旧线。
// LiChao<> 求最小，LiChao<true> 求最大；ll 系数，i128 乘加，无需 INF。
// add 返回直线 ID；query(x) 返回最优 ID，0 表示无解，否则 line[id](x) 为值。
// x 必须已登记；M 为不同 x 个数，直线添加/查询 O(log M)，空间 O(M + 添加数)。
template<bool isMax = false>
struct LiChao {
    struct Line {
        ll a = 0, b = 0;
        i128 operator()(ll x) const { return (i128)a * x + b; }
    };
    int n = 1;
    vector<ll> xs;
    vector<Line> line;
    vector<int> tr;

    LiChao(vector<ll> x = {}) { init(move(x)); }
    void init(vector<ll> x) {
        sort(x.begin(), x.end());
        x.erase(unique(x.begin(), x.end()), x.end());
        n = 1;
        while (n < int(x.size())) n <<= 1;
        xs = move(x);
        if (!xs.empty()) xs.resize(n, xs.back());
        tr.assign(size_t(2) * n, 0);
        line = {{}};
    }
    bool better(int a, int b, ll x) const {
        return !b || (isMax ? line[a](x) > line[b](x) : line[a](x) < line[b](x));
    }
    void add(int p, int l, int r, int id) {
        if (!tr[p]) { tr[p] = id; return; }
        int m = (l + r) >> 1;
        if (better(id, tr[p], xs[m])) swap(id, tr[p]);
        if (l == r) return;
        if (better(id, tr[p], xs[l])) add(p * 2, l, m, id);
        else if (better(id, tr[p], xs[r])) add(p * 2 + 1, m + 1, r, id);
    }
    int add(ll a, ll b) {
        int id = line.size();
        line.pb({a, b});
        if (!xs.empty()) add(1, 0, n - 1, id);
        return id;
    }
    int query(ll x) const {
        if (xs.empty()) return 0;
        int k = lower_bound(xs.begin(), xs.end(), x) - xs.begin(), ans = 0;
        assert(k < int(xs.size()) && xs[k] == x);
        for (k += n; k; k >>= 1)
            if (tr[k] && better(tr[k], ans, x)) ans = tr[k];
        return ans;
    }

    // === [1] 可选：只在闭定义域 [L,R] 生效的线段，添加 O(log² M) ===
    void add_segment(int p, int l, int r, int L, int R, int id) {
        if (L <= l && r <= R) return add(p, l, r, id);
        int m = (l + r) >> 1;
        if (L <= m) add_segment(p * 2, l, m, L, R, id);
        if (m < R) add_segment(p * 2 + 1, m + 1, r, L, R, id);
    }
    int add_segment(ll a, ll b, ll L, ll R) {
        int id = line.size();
        line.pb({a, b});
        int l = lower_bound(xs.begin(), xs.end(), L) - xs.begin();
        int r = upper_bound(xs.begin(), xs.end(), R) - xs.begin() - 1;
        if (l <= r) add_segment(1, 0, n - 1, l, r, id);
        return id;
    }
};
