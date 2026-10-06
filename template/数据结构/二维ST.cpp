#include <bit>
#include <cassert>
#include <limits>
#include <vector>

// 静态矩阵 a[1..n][1..m]；第 0 行/列不参与，真实行须等长。
// Info{} 为单位元，operator+ 须结合、交换、幂等，适合 min/max/gcd，不适合和。
// 支持显式 Info(a[i][j])；默认构造、空矩阵、反复 init 均可。
// 构建/空间 O(nm log n log m)，查询 O(1)；修改后需重新构建。
template<class Info>
struct ST2D {
    int n = 0, m = 0;
    // 每层只存有效左上角，按行展平；大小 (n-2^kx+1)*(m-2^ky+1)。
    std::vector<std::vector<std::vector<Info>>> f;

    ST2D() = default;
    template<class T>
    explicit ST2D(const std::vector<std::vector<T>> &a) { init(a); }

    template<class T>
    void init(const std::vector<std::vector<T>> &a) {
        assert(a.size() <= std::size_t(std::numeric_limits<int>::max()));
        n = a.empty() ? 0 : int(a.size()) - 1;
        m = 0;
        f.clear();
        if (!n) return;
        assert(a[1].size() <= std::size_t(std::numeric_limits<int>::max()));
        m = a[1].empty() ? 0 : int(a[1].size()) - 1;
        for (int i = 2; i <= n; ++i) assert(a[i].size() == a[1].size());
        if (!m) return;

        int nx = std::bit_width(unsigned(n)), ny = std::bit_width(unsigned(m));
        f.assign(nx, std::vector<std::vector<Info>>(ny));
        for (int kx = 0; kx < nx; ++kx) {
            for (int ky = 0; ky < ny; ++ky) {
                int rows = n - (1 << kx) + 1, cols = m - (1 << ky) + 1;
                auto &cur = f[kx][ky];
                cur.resize(std::size_t(rows) * cols);
                if (!kx && !ky) {
                    for (int x = 1; x <= n; ++x)
                        for (int y = 1; y <= m; ++y)
                            cur[std::size_t(x - 1) * cols + y - 1] = Info(a[x][y]);
                    continue;
                }
                // 先构建 kx=0 的横向层，再由上一横向尺寸相同的层纵向倍增。
                int px = kx ? kx - 1 : 0, py = kx ? ky : ky - 1;
                int dx = kx ? 1 << (kx - 1) : 0, dy = kx ? 0 : 1 << (ky - 1);
                for (int x = 1; x <= rows; ++x)
                    for (int y = 1; y <= cols; ++y)
                        cur[std::size_t(x - 1) * cols + y - 1] =
                            at(px, py, x, y) + at(px, py, x + dx, y + dy);
            }
        }
    }

    // 1 基闭矩形；任一维为空时返回单位元，否则必须在矩阵内。
    Info query(int x1, int y1, int x2, int y2) const {
        if (x1 > x2 || y1 > y2) return Info{};
        assert(1 <= x1 && x2 <= n && 1 <= y1 && y2 <= m);
        int kx = std::bit_width(unsigned(x2 - x1 + 1)) - 1;
        int ky = std::bit_width(unsigned(y2 - y1 + 1)) - 1;
        int x = x2 - (1 << kx) + 1, y = y2 - (1 << ky) + 1;
        // 四块可能重叠；幂等性保证重复计入不改变答案。
        return at(kx, ky, x1, y1) + at(kx, ky, x1, y)
             + at(kx, ky, x, y1) + at(kx, ky, x, y);
    }

private:
    const Info &at(int kx, int ky, int x, int y) const {
        int cols = m - (1 << ky) + 1;
        return f[kx][ky][std::size_t(x - 1) * cols + y - 1];
    }
};
