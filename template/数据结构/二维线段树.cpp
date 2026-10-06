#include <bit>
#include <cassert>
#include <climits>
#include <stdexcept>
#include <vector>

// GNU++20；1 基闭矩形，参数顺序 (x1,y1,x2,y2)。
// Info{} 为单位元，+ 需结合且可交换；仅支持单点赋值。
// 两维补齐二次幂：空间/批量构建 O(nm)，修改/查询 O(log n log m)。
template<class Info>
struct Segment2D {
    int n = 0, m = 0;
    std::size_t sx = 1, sy = 1, stride = 2;
    std::vector<Info> info;

    Segment2D(int n = 0, int m = 0) { init(n, m); }
    template<class T>
    Segment2D(const std::vector<std::vector<T>>& a) { init(a); }

    Info& at(std::size_t x, std::size_t y) { return info[x * stride + y]; }
    const Info& at(std::size_t x, std::size_t y) const {
        return info[x * stride + y];
    }

    // 初始化为单位元；带 len 等字段的真实零叶子应由调用者赋完整 Info。
    void init(int rows, int cols) {
        assert(rows >= 0 && cols >= 0);
        n = rows; m = cols;
        sx = sy = 1; stride = 2;
        info.clear();
        if (!n || !m) return;
        sx = std::bit_ceil(std::size_t(n));
        sy = std::bit_ceil(std::size_t(m));
        std::size_t lim = info.max_size();
        if (sx > lim / 2 || sy > lim / 2 || 2 * sx > lim / (2 * sy))
            throw std::length_error("Segment2D too large");
        stride = 2 * sy;
        info.assign(2 * sx * stride, Info{});
    }

    template<class T>
    void init(const std::vector<std::vector<T>>& a) {
        assert(a.size() <= std::size_t(INT_MAX) + 1);
        int rows = a.empty() ? 0 : int(a.size() - 1);
        std::size_t cols = a.empty() ? 0 : a[rows ? 1 : 0].size();
        assert(cols <= std::size_t(INT_MAX) + 1);
        init(rows, cols ? int(cols - 1) : 0);
        for (int i = 0; i < n; ++i) assert(a[i + 1].size() == cols);
        if (!n || !m) return;
        for (int i = 0; i < n; ++i) {
            std::size_t x = sx + i;
            for (int j = 0; j < m; ++j) at(x, sy + j) = Info(a[i + 1][j + 1]);
            for (std::size_t y = sy - 1; y; --y)
                at(x, y) = at(x, y * 2) + at(x, y * 2 + 1);
        }
        for (std::size_t x = sx - 1; x; --x)
            for (std::size_t y = 1; y < stride; ++y)
                at(x, y) = at(x * 2, y) + at(x * 2 + 1, y);
    }

    void set(int x, int y, Info v) {
        assert(1 <= x && x <= n && 1 <= y && y <= m);
        std::size_t p = sx + x - 1, q = sy + y - 1;
        at(p, q) = v;
        for (std::size_t j = q >> 1; j; j >>= 1)
            at(p, j) = at(p, j * 2) + at(p, j * 2 + 1);
        for (p >>= 1; p; p >>= 1)
            for (std::size_t j = q; j; j >>= 1)
                at(p, j) = at(p * 2, j) + at(p * 2 + 1, j);
    }

    Info get(int x, int y) const {
        assert(1 <= x && x <= n && 1 <= y && y <= m);
        return at(sx + x - 1, sy + y - 1);
    }

    Info query_y(std::size_t x, int y1, int y2) const {
        Info ans{};
        for (std::size_t l = sy + y1 - 1, r = sy + y2; l < r; l >>= 1, r >>= 1) {
            if (l & 1) ans = ans + at(x, l++);
            if (r & 1) ans = ans + at(x, --r);
        }
        return ans;
    }

    Info query(int x1, int y1, int x2, int y2) const {
        if (x1 > x2 || y1 > y2) return Info{};
        assert(1 <= x1 && x2 <= n && 1 <= y1 && y2 <= m);
        Info ans{};
        for (std::size_t l = sx + x1 - 1, r = sx + x2; l < r; l >>= 1, r >>= 1) {
            if (l & 1) ans = ans + query_y(l++, y1, y2);
            if (r & 1) ans = ans + query_y(--r, y1, y2);
        }
        return ans;
    }
};
