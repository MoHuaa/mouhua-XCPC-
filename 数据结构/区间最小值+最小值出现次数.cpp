constexpr int N = 2e5 + 7;
struct info {
    int minv, mincnt;
    friend info operator+(const info &l, const info &r) {
        info a;
        a.minv = min(l.minv, r.minv);
        if (l.minv == r.minv)a.mincnt = l.mincnt + r.mincnt;
        else if (l.minv < r.minv)a.mincnt = l.mincnt;
        else a.mincnt = r.mincnt;
        return a;
    }
};
struct node {
    int t;
    info val;
} seg[N * 4];
void update(int id) {
    seg[id].val = seg[id << 1].val + seg[id << 1 | 1].val;
}
void settag(int id, int t) {
    seg[id].val.minv = seg[id].val.minv + t;
    seg[id].t = seg[id].t + t;
}
void pushdown(int id) {
    if (seg[id].t != 0) {
        settag(id << 1, seg[id].t);
        settag(id << 1 | 1, seg[id].t);
        seg[id].t = 0;
    }
}
void build(int id, int l, int r) {
    seg[id].t=0;
    if (l == r) {
        seg[id].val.minv = 0;
        seg[id].val.mincnt = 1;
        return;
    } else {
        int mid = (l + r) >> 1;
        build(id << 1, l, mid);
        build(id << 1 | 1, mid + 1, r);
        update(id);
    }
}
void change(int id, int l, int r, int nl, int nr, int t) {
    if (l == nl and r == nr) {
        settag(id, t);
        return;
    }
    int mid = (l + r) >> 1;
    pushdown(id);
    if (nr <= mid)change(id << 1, l, mid, nl, nr, t);
    else if (nl > mid)change(id << 1 | 1, mid + 1, r, nl, nr, t);
    else {
        change(id << 1, l, mid, nl, mid, t);
        change(id << 1 | 1, mid + 1, r, mid + 1, nr, t);
    }
    update(id);
}
info query(int id, int l, int r, int ql, int qr) {
    if (l == ql and r == qr) {
        return seg[id].val;
    }
    int mid = (l + r) / 2;
    pushdown(id);
    update(id);
    if (qr <= mid)return query(id * 2, l, mid, ql, qr);
    else if (ql > mid)return query(id * 2 + 1, mid + 1, r, ql, qr);
    else {
        return query(id * 2, l, mid, ql, mid) + query(id * 2 + 1, mid + 1, r, mid + 1, qr);
    }
}