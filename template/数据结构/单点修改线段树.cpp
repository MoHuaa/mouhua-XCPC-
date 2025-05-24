template<class Info>
struct Segment {
    std::vector<Info> info;
    int n;
    Segment() : n(0) {}
    Segment(int n) : n(n), info(4*n+1) {}
    Segment(int n, vector<Info>&v) : n(n), info(4*n+1) {
        init(v);
    }
    void pushup(int p) {
        info[p] = info[p << 1] + info[p << 1 | 1];
    }
    void init(vector<Info>&v) {
        auto build = [&](auto && build, int p, int l, int r) {
            if (l == r) {
                info[p] = v[l];
                return;
            }
            int mid = (l + r) >> 1;
            build(build, p << 1, l, mid);
            build(build, p << 1 | 1, mid + 1, r);
            pushup(p);
        };
        build(build, 1, 1, n);
    }
    void change(int pos, Info x) {
        change(1, 1, n, pos, x);
    }
    void change(int p, int l, int r, int pos, Info x) {
        if (l == r) {
            info[p] = x;
            return;
        }
        int mid = (l + r) >> 1;
        if (pos <= mid)change(p << 1, l, mid, pos, x);
        else change(p << 1 | 1, mid + 1, r, pos, x);
        pushup(p);
    }
    Info query(int l, int r) {
        return query(1, 1, n, l, r);
    }
    Info query(int p, int l, int r, int nl, int nr) {
        if (nl <= l and r <= nr)return info[p];
        int mid = (l + r) >> 1;
        if (nr <= mid)return query(p << 1, l, mid, nl, nr);
        if (nl > mid)return query(p << 1 | 1, mid + 1, r, nl, nr);
        return query(p << 1, l, mid, nl, nr) + query(p << 1 | 1, mid + 1, r, nl, nr);
    }
    int find_first(int p, int l, int r, const std::function<bool(const Info &)> &f)
    {
        if (l == r) {
            return l;
        }
        int mid = (l + r) >> 1;
        if (f(info[p << 1]))return find_first(p << 1, l, mid, f);
        else return find_first(p << 1 | 1, mid + 1, r, f);
    }
};
struct node {
    friend  node operator+(node lhs, node rhs) {
        node now;
        if (lhs.val < rhs.val)now = lhs;
        else now = rhs;
        return now;
    }
};