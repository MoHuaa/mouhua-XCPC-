#include <bits/stdc++.h>
using namespace std;
using ll = long long;

// 预先登记所有可能修改的 (x,y)，初始权值为 0。
// add 只能修改已登记的点；重复登记不影响结果。
// query 查询闭矩形 [x1,x2] × [y1,y2]，边界不必登记。
// N 为登记点数：建表 O(N log N)，空间 O(N log N)，操作 O(log² N)。
struct BIT2D {
    int n = 0;
    vector<int> xs;
    vector<vector<int>> ys;
    vector<vector<ll>> tr;

    BIT2D(vector<pair<int, int>> p = {}) { init(move(p)); }

    void init(vector<pair<int, int>> p) {
        xs.clear();
        for (auto [x, y] : p) xs.push_back(x);
        sort(xs.begin(), xs.end());
        xs.erase(unique(xs.begin(), xs.end()), xs.end());
        n = xs.size();
        ys.assign(n + 1, {});
        tr.assign(n + 1, {});

        sort(p.begin(), p.end(), [](auto a, auto b) {
            return a.second < b.second;
        });
        for (auto [x, y] : p) {
            int k = lower_bound(xs.begin(), xs.end(), x) - xs.begin() + 1;
            for (int i = k; i <= n; i += i & -i) {
                auto &v = ys[i];
                if (v.empty() || v.back() != y) v.push_back(y);
            }
        }
        for (int i = 1; i <= n; ++i)
            tr[i].assign(ys[i].size() + 1, 0);
    }

    void add(int x, int y, ll v) {
        int k = lower_bound(xs.begin(), xs.end(), x) - xs.begin() + 1;
        for (int i = k; i <= n; i += i & -i) {
            int j = lower_bound(ys[i].begin(), ys[i].end(), y) - ys[i].begin() + 1;
            for (; j < (int)tr[i].size(); j += j & -j) tr[i][j] += v;
        }
    }

    ll query(int x1, int y1, int x2, int y2) const {
        if (x1 > x2 || y1 > y2) return 0;
        auto get = [&](int i) {
            const auto &v = ys[i];
            int l = lower_bound(v.begin(), v.end(), y1) - v.begin();
            int r = upper_bound(v.begin(), v.end(), y2) - v.begin();
            ll ans = 0;
            for (; r > l; r -= r & -r) ans += tr[i][r];
            for (; l > r; l -= l & -l) ans -= tr[i][l];
            return ans;
        };
        int l = lower_bound(xs.begin(), xs.end(), x1) - xs.begin();
        int r = upper_bound(xs.begin(), xs.end(), x2) - xs.begin();
        ll ans = 0;
        for (; r > l; r -= r & -r) ans += get(r);
        for (; l > r; l -= l & -l) ans -= get(l);
        return ans;
    }
};
