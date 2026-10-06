#include <cassert>
#include <vector>

// 稠密矩阵 a[1..n][1..m]；T{} 为零，+ 需结合、交换，矩形和还需减法。
// 仅支持单点增量；空间/矩阵构建 O(nm)，修改/查询 O(log n log m)。
template<class T>
struct BIT2D {
    int n = 0, m = 0;
    std::vector<T> tr;

    BIT2D(int n = 0, int m = 0) { init(n, m); }
    template<class U>
    BIT2D(const std::vector<std::vector<U>>& a) { init(a); }

    T& at(int x, int y) { return tr[std::size_t(x) * (m + 1) + y]; }
    const T& at(int x, int y) const {
        return tr[std::size_t(x) * (m + 1) + y];
    }

    void init(int rows, int cols) {
        assert(0 <= rows && rows < (1 << 30) && 0 <= cols && cols < (1 << 30));
        n = rows; m = cols;
        tr.assign(std::size_t(n + 1) * (m + 1), T{});
    }

    // 第 0 行/列不参与；真实行须等长。两维依次传播，O(nm) 建树。
    template<class U>
    void init(const std::vector<std::vector<U>>& a) {
        assert(a.size() <= std::size_t(1 << 30));
        int rows = a.empty() ? 0 : int(a.size()) - 1;
        std::size_t cols = rows ? a[1].size() : 0;
        assert(cols <= std::size_t(1 << 30));
        init(rows, cols ? int(cols) - 1 : 0);
        for (int i = 1; i <= n; ++i) {
            assert(a[i].size() == cols);
            for (int j = 1; j <= m; ++j) at(i, j) = T(a[i][j]);
            for (int j = 1; j <= m; ++j) {
                int k = j + (j & -j);
                if (k <= m) at(i, k) = at(i, k) + at(i, j);
            }
        }
        for (int i = 1; i <= n; ++i) {
            int k = i + (i & -i);
            if (k <= n)
                for (int j = 1; j <= m; ++j) at(k, j) = at(k, j) + at(i, j);
        }
    }

    void add(int x, int y, T delta) {
        assert(1 <= x && x <= n && 1 <= y && y <= m);
        for (int i = x; i <= n; i += i & -i)
            for (int j = y; j <= m; j += j & -j)
                at(i, j) = at(i, j) + delta;
    }

    // 前缀 [1,x] × [1,y]；x 或 y 为 0 时返回零。
    T sum(int x, int y) const {
        assert(0 <= x && x <= n && 0 <= y && y <= m);
        T ans{};
        for (int i = x; i; i -= i & -i)
            for (int j = y; j; j -= j & -j) ans = ans + at(i, j);
        return ans;
    }

    // 1 基闭矩形；任一维为空时返回零，否则必须在矩阵内。
    T query(int x1, int y1, int x2, int y2) const {
        if (x1 > x2 || y1 > y2) return T{};
        assert(1 <= x1 && x2 <= n && 1 <= y1 && y2 <= m);
        return sum(x2, y2) - sum(x1 - 1, y2)
             - sum(x2, y1 - 1) + sum(x1 - 1, y1 - 1);
    }
};
