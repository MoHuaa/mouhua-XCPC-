struct tag {
    ll val = 0;
    void init() {
        val = 0;
    }
    tag&operator+=(const tag &t) & {
        val += t.val;
        return *this;
    }
};
struct node {
    int minv, mincnt;
    friend node operator+(const node &l, const node &r) {
        node a;
        a.minv = min(l.minv, r.minv);
        if (l.minv == r.minv)a.mincnt = l.mincnt + r.mincnt;
        else if (l.minv < r.minv)a.mincnt = l.mincnt;
        else a.mincnt = r.mincnt;
        return a;
    }
    node&operator+=(tag&t) {
        minv += t.val;
        return *this;
    }
};