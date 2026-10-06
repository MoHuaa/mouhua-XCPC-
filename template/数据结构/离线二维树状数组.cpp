#include <algorithm>
#include <cassert>
#include <utility>
#include <vector>

// 预先登记所有可能修改的点，之后可按原操作顺序增量修改、查询。
// C 是真实坐标；T{} 为零，支持加减，累加满足结合、交换律。
// 空点集查询返回零；未登记的位置不能 add。
// K 为登记点数：空间 O(K log K)，构建 O(K log² K)，操作 O(log² K)。
template<class T, class C = long long>
struct OfflineBIT2D {
    int n = 0;
    std::vector<std::pair<C, C>> points;
    std::vector<C> xs;
    std::vector<std::vector<C>> ys;
    std::vector<std::vector<T>> bit;

    OfflineBIT2D() = default;
    OfflineBIT2D(const std::vector<std::pair<C, C>> &p) { init(p); }

    // 重复登记自动去重；重新 init 会清空所有已加的值。
    void init(const std::vector<std::pair<C, C>> &p) {
        points = p;
        std::sort(points.begin(), points.end());
        points.erase(std::unique(points.begin(), points.end()), points.end());
        assert(points.size() < std::size_t(1 << 30));
        xs.clear();
        for (auto [x, y] : points) xs.push_back(x);
        xs.erase(std::unique(xs.begin(), xs.end()), xs.end());
        n = int(xs.size());
        ys.assign(n + 1, {});
        bit.assign(n + 1, {});

        for (auto [x, y] : points) {
            int i = int(std::lower_bound(xs.begin(), xs.end(), x) - xs.begin()) + 1;
            for (; i <= n; i += i & -i) ys[i].push_back(y);
        }
        for (int i = 1; i <= n; ++i) {
            auto &v = ys[i];
            std::sort(v.begin(), v.end());
            v.erase(std::unique(v.begin(), v.end()), v.end());
            bit[i].assign(v.size() + 1, T{});
        }
    }

    void add(C x, C y, T delta) {
        // 必须登记精确点对；某个外层桶包含 y 不代表 (x,y) 已登记。
        assert(std::binary_search(points.begin(), points.end(), std::pair<C, C>{x, y}));
        int i = int(std::lower_bound(xs.begin(), xs.end(), x) - xs.begin()) + 1;
        for (; i <= n; i += i & -i) {
            int j = int(std::lower_bound(ys[i].begin(), ys[i].end(), y) - ys[i].begin()) + 1;
            for (; j < int(bit[i].size()); j += j & -j)
                bit[i][j] = bit[i][j] + delta;
        }
    }

    // 坐标 <= x 且 <= y；查询边界不必登记。
    T sum(C x, C y) const {
        int r = int(std::upper_bound(xs.begin(), xs.end(), x) - xs.begin());
        return prefix(r, y, true);
    }

    // 闭矩形 [x1,x2] × [y1,y2]；反向边界返回零。
    T query(C x1, C y1, C x2, C y2) const {
        if (x2 < x1 || y2 < y1) return T{};
        int l = int(std::lower_bound(xs.begin(), xs.end(), x1) - xs.begin());
        int r = int(std::upper_bound(xs.begin(), xs.end(), x2) - xs.begin());
        // 直接按秩区分 < 与 <=，不计算 x1-1 或 y1-1。
        return prefix(r, y2, true) - prefix(l, y2, true)
             - prefix(r, y1, false) + prefix(l, y1, false);
    }

private:
    // 前 x_count 个离散 x；include_y 为 true 时取 <=y，否则取 <y。
    T prefix(int x_count, C y, bool include_y) const {
        T ans{};
        for (int i = x_count; i > 0; i -= i & -i) {
            const auto &v = ys[i];
            auto it = include_y ? std::upper_bound(v.begin(), v.end(), y)
                                : std::lower_bound(v.begin(), v.end(), y);
            for (int j = int(it - v.begin()); j > 0; j -= j & -j)
                ans = ans + bit[i][j];
        }
        return ans;
    }
};
