#pragma once
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <numeric>
#include <utility>
#include <vector>

// Fixed coordinates; flat inner trees and parent-to-child prefix ranks.
// Info{} is the identity, + must commute. Closed rectangles, input-order IDs.
// Raw coordinates may be negative; only ranks are used for tree indexing.
// Reference: https://maspypy.github.io/library/ds/segtree/segtree_2d.hpp
template <class Info, class Coord = int> class RangeFC {
    using Point = std::pair<Coord, Coord>;
    int base_ = 1;
    std::vector<Point> points_;
    std::vector<int> rank_, pos_, off_, left_;
    std::vector<Coord> ys_;
    std::vector<Info> tree_;

    int x_bound(Coord x, bool upper) const {
        auto cmp = [](const Point& p, Coord v) { return p.first < v; };
        if (!upper) return int(std::lower_bound(points_.begin(), points_.end(), x, cmp) - points_.begin());
        return int(std::upper_bound(points_.begin(), points_.end(), x,
                   [](Coord v, const Point& p) { return v < p.first; }) - points_.begin());
    }
    void inner_set(int u, int k, const Info& v) {
        int n = off_[u + 1] - off_[u], o = 2 * off_[u];
        tree_[o + (k += n)] = v;
        while (k >>= 1) tree_[o + k] = tree_[o + 2 * k] + tree_[o + 2 * k + 1];
    }
    Info inner_query(int u, int l, int r) const {
        int n = off_[u + 1] - off_[u], o = 2 * off_[u];
        Info ans{};
        for (l += n, r += n; l < r; l >>= 1, r >>= 1) {
            if (l & 1) ans = ans + tree_[o + l++];
            if (r & 1) ans = ans + tree_[o + --r];
        }
        return ans;
    }
    void set_rank(int i, const Info& v) {
        int u = 1, k = pos_[i];
        for (;;) {
            inner_set(u, k, v);
            if (u >= base_) break;
            int o = off_[u], a = left_[o + k] - left_[o];
            bool go_left = left_[o + k + 1] != left_[o + k];
            k = go_left ? a : k - a;
            u = 2 * u + !go_left;
        }
    }
    Info query_impl(int u, int l, int r, int ql, int qr, int a, int b) const {
        if (a == b || qr <= l || r <= ql) return Info{};
        if (ql <= l && r <= qr) return inner_query(u, a, b);
        int o = off_[u], la = left_[o + a] - left_[o], lb = left_[o + b] - left_[o];
        int mid = (l + r) >> 1;
        return query_impl(2 * u, l, mid, ql, qr, la, lb)
             + query_impl(2 * u + 1, mid, r, ql, qr, a - la, b - lb);
    }

public:
    void init(const std::vector<Point>& known) { build(known, {}); }
    void build(const std::vector<Point>& known, const std::vector<Info>& values) {
        int n = int(known.size());
        assert(values.empty() || int(values.size()) == n);
        std::vector<int> order(n);
        std::iota(order.begin(), order.end(), 0);
        std::sort(order.begin(), order.end(), [&](int i, int j) { return known[i] < known[j]; });
        points_.resize(n); rank_.resize(n); pos_.resize(n); ys_.resize(n);
        for (int i = 0; i < n; ++i) {
            points_[i] = known[order[i]]; rank_[order[i]] = i;
            assert(i == 0 || points_[i - 1] != points_[i]);
        }
        base_ = 1;
        while (base_ < n) base_ <<= 1;
        off_.assign(2 * base_ + 1, 0);
        for (int i = 0; i < n; ++i)
            for (int u = base_ + i; u; u >>= 1) ++off_[u + 1];
        std::partial_sum(off_.begin(), off_.end(), off_.begin());
        tree_.assign(std::size_t(2) * off_.back(), Info{});
        left_.assign(off_[base_] + 1, 0);
        std::vector<int> ptr = off_, by_y(n);
        std::iota(by_y.begin(), by_y.end(), 0);
        std::sort(by_y.begin(), by_y.end(), [&](int i, int j) {
            return Point{points_[i].second, points_[i].first} < Point{points_[j].second, points_[j].first};
        });
        for (int p = 0; p < n; ++p) {
            int i = by_y[p], child = 0;
            pos_[i] = p; ys_[p] = points_[i].second;
            Info v = values.empty() ? Info{} : values[order[i]];
            for (int u = base_ + i; u; child = u, u >>= 1) {
                int k = ptr[u]++;
                tree_[off_[u + 1] + k] = v;
                if (u < base_) left_[k + 1] = !(child & 1);
            }
        }
        std::partial_sum(left_.begin(), left_.end(), left_.begin());
        for (int u = 1; u < 2 * base_; ++u) {
            int o = 2 * off_[u], m = off_[u + 1] - off_[u];
            for (int k = m - 1; k > 0; --k)
                tree_[o + k] = tree_[o + 2 * k] + tree_[o + 2 * k + 1];
        }
    }
    void set(Coord x, Coord y, Info v) {
        Point p{x, y};
        int i = int(std::lower_bound(points_.begin(), points_.end(), p) - points_.begin());
        assert(i < int(points_.size()) && points_[i] == p);
        set_rank(i, v);
    }
    void set_id(int id, Info v) {
        assert(0 <= id && id < int(rank_.size()));
        set_rank(rank_[id], v);
    }
    Info get_id(int id) const {
        assert(0 <= id && id < int(rank_.size()));
        return tree_[2 * off_[base_ + rank_[id]] + 1];
    }
    Info query(Coord x1, Coord y1, Coord x2, Coord y2) const {
        if (x1 > x2 || y1 > y2 || points_.empty()) return Info{};
        int l = x_bound(x1, false), r = x_bound(x2, true);
        int a = int(std::lower_bound(ys_.begin(), ys_.end(), y1) - ys_.begin());
        int b = int(std::upper_bound(ys_.begin(), ys_.end(), y2) - ys_.begin());
        return query_impl(1, 0, base_, l, r, a, b);
    }
    std::size_t bytes() const {
        return sizeof(*this) + points_.capacity() * sizeof(Point) + tree_.capacity() * sizeof(Info)
             + ys_.capacity() * sizeof(Coord)
             + (rank_.capacity() + pos_.capacity() + off_.capacity() + left_.capacity()) * sizeof(int);
    }
};
