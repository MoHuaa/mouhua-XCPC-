// 已知全部修改坐标；单点赋值，闭矩形查询；Info{} 为单位元，+ 交换结合。
// 接在公共头后使用：using namespace std; #define pb push_back（GNU++20）。
// Stop=true 要求默认值、赋入值、合并结果的值位全部初始化，+ 为纯值运算。
template<class Info, bool Stop = false>
struct Seg2D {
    using P = pair<int, int>;
    int n = 1, h = 0;
    vector<int> xs, ys;
    vector<vector<Info>> tr;
    vector<vector<array<int, 4>>> to;

    Seg2D(vector<P> a = {}) { init(move(a)); }
    int rank(const vector<int>& a, int x, bool r = false) const {
        return (r ? upper_bound(a.begin(), a.end(), x)
                  : lower_bound(a.begin(), a.end(), x)) - a.begin();
    }
    bool same(const Info& a, const Info& b) {
        if constexpr (Stop && has_unique_object_representations_v<Info>)
            return !memcmp(&a, &b, sizeof a);
        return false;
    }
    bool change(int p, int h, int x, int k, Info& v) {
        if (h) {
            int c = 4 * p + 1, s = (x >> (h - 2)) & 3;
            if (!change(c + s, h - 2, x, to[p][k][s], v)) return false;
            for (int i = 0; i < 4; ++i)
                if (i != s && to[p][k][i] != to[p][k + 1][i])
                    v = v + tr[c + i][tr[c + i].size() / 2 + to[p][k][i]];
        }
        auto& t = tr[p]; k += t.size() / 2;
        if (same(t[k], v)) return false;
        t[k] = v;
        while (k >>= 1) {
            Info z = t[k << 1] + t[k << 1 | 1];
            if (same(t[k], z)) break;
            t[k] = z;
        }
        return true; // 此 y 的值变了；内层根不变也不能直接停止外层。
    }
    Info query(int p, int h, int l, int r, int a, int b) const {
        if (a == b) return Info{};
        Info ans{};
        if (l <= 0 && (1 << h) <= r) {
            const auto& t = tr[p]; int n = t.size() / 2;
            if (a == 0 && b == n) return t[1];
            for (a += n, b += n; a < b; a >>= 1, b >>= 1) {
                if (a & 1) ans = ans + t[a++];
                if (b & 1) ans = ans + t[--b];
            }
            return ans;
        }
        h -= 2;
        int L = max(0, l >> h), R = min(3, (r - 1) >> h);
        for (int i = L; i <= R; ++i) {
            int d = i << h;
            ans = ans + query(4 * p + 1 + i, h, l - d, r - d,
                              to[p][a][i], to[p][b][i]);
        }
        return ans;
    }
    void init(vector<P> a) {
        sort(a.begin(), a.end());
        a.erase(unique(a.begin(), a.end()), a.end());
        xs.clear();
        for (auto [x, y] : a) if (xs.empty() || xs.back() != x) xs.pb(x);
        n = 1; h = 0;
        while (n < int(xs.size())) n <<= 2, h += 2;
        int s = (n - 1) / 3;
        vector<vector<int>> y(s + n);
        tr.assign(s + n, {}); to.assign(s, {});
        for (int i = 0, j = 0; i < int(xs.size()); ++i)
            while (j < int(a.size()) && a[j].first == xs[i])
                y[s + i].pb(a[j++].second);
        for (int p = s + n - 1; p >= 0; --p) {
            if (p < s) {
                int c = 4 * p + 1;
                for (int i = 0; i < 4; ++i) {
                    auto &l = y[p], &r = y[c + i];
                    vector<int> z;
                    z.reserve(l.size() + r.size());
                    set_union(l.begin(), l.end(), r.begin(), r.end(), back_inserter(z));
                    l.swap(z);
                }
                array<int, 4> pos{};
                to[p].reserve(y[p].size() + 1);
                for (int v : y[p]) {
                    to[p].pb(pos);
                    for (int i = 0; i < 4; ++i)
                        if (pos[i] < int(y[c + i].size()) && y[c + i][pos[i]] == v) ++pos[i];
                }
                to[p].pb(pos); // to[p][k][i]：前 k 个 y 中属于孩子 i 的数量。
                for (int i = 0; i < 4; ++i) vector<int>().swap(y[c + i]);
            }
            tr[p].assign(2 * y[p].size(), Info{});
        }
        ys = move(y[0]);
    }
    void set(int x, int y, Info v) {
        change(0, h, rank(xs, x), rank(ys, y), v);
    }
    Info query(int x1, int y1, int x2, int y2) const {
        if (x1 > x2 || y1 > y2 || xs.empty()) return Info{};
        int l = rank(xs, x1), r = rank(xs, x2, true);
        int a = rank(ys, y1), b = rank(ys, y2, true);
        if (l == r || a == b) return Info{};
        if (r == int(xs.size())) r = n;
        return query(0, h, l, r, a, b);
    }
};
