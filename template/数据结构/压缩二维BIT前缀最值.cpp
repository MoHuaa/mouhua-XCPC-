#include <bits/stdc++.h>
using namespace std;
using ll = long long;

// isMax=true：单点 chmax，查询前缀最大值；false：单点 chmin，查询前缀最小值。
// 预先登记所有可能修改的完整 (x,y)；登记时尚未放入权值。
// query(x,y) 查询 px<=x 且 py<=y 的点；点坐标均>=1时即 [1,x]×[1,y]。
// 未更新的位置不参与查询；空前缀返回 E。update 不是覆盖赋值。
// N 为登记点数：建表/空间 O(N log N)，更新/查询 O(log² N)。
template<bool isMax>
struct BIT2DExtreme {
    static constexpr ll E = isMax ? LLONG_MIN : LLONG_MAX;
    static ll op(ll a, ll b) { return isMax ? max(a, b) : min(a, b); }
    int n = 0;
    vector<int> xs;
    vector<vector<int>> ys;
    vector<vector<ll>> tr;

    BIT2DExtreme(vector<pair<int, int>> p = {}) { init(move(p)); }

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
            tr[i].assign(ys[i].size() + 1, E);
    }

    void update(int x, int y, ll v) {
        int k = lower_bound(xs.begin(), xs.end(), x) - xs.begin() + 1;
        for (int i = k; i <= n; i += i & -i) {
            int j = lower_bound(ys[i].begin(), ys[i].end(), y) - ys[i].begin() + 1;
            for (; j < (int)tr[i].size(); j += j & -j)
                tr[i][j] = op(tr[i][j], v);
        }
    }

    ll query(int x, int y) const {
        ll ans = E;
        int k = upper_bound(xs.begin(), xs.end(), x) - xs.begin();
        for (int i = k; i; i -= i & -i) {
            int j = upper_bound(ys[i].begin(), ys[i].end(), y) - ys[i].begin();
            for (; j; j -= j & -j) ans = op(ans, tr[i][j]);
        }
        return ans;
    }
};
