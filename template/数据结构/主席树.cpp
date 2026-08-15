template<class Info>
struct PST {
    struct Node {
        int l;
        int r;
        Info h;
        Node() : l(0), r(0), h() {}
    };

    vector<Node> tr;
    int tot;

    PST(int maxNode = 0) : tr(maxNode + 1), tot(0) {}

    inline void pushup(int p) {
        tr[p].h = tr[p].l + tr[p].r;
    }

    int add(int p, int l, int r, int x, Info v) {
        int np = ++tot;
        tr[np] = tr[p];

        if (l == r) {
            tr[np].h = v;   // 单点加
            return np;
        }

        int m = (l + r) >> 1;
        if (x <= m) {
            tr[np].l = add(tr[np].l, l, m, x, v);
        } else {
            tr[np].r = add(tr[np].r, m + 1, r, x, v);
        }
        pushup(np);
        return np;
    }

    Info query1(int p, int l, int r, int nl, int nr) {
        if (!p) return Info();
        if (nl <= l && r <= nr) return tr[p].h;
        int mid = (l + r) >> 1;
        if (nr <= mid) return query1(tr[p].l, l, mid, nl, nr);
        if (nl > mid) return query1(tr[p].r, mid + 1, r, nl, nr);
        return query1(tr[p].l, l, mid, nl, nr) + query1(tr[p].r, mid + 1, r, nl, nr);
    }

    template<class Check>
    int query2(int p1, int p2, int l, int r, Check check) {
        Info diff = p2 - p1;
        if (!check(diff)) {
            return -1;
        }
        if (l == r) {
            return l;
        }
        int m = (l + r) >> 1;

        int lc1 = tr[p1].l, lc2 = tr[p2].l;
        Info leftDiff = lc2 - lc1;
        if (check(leftDiff)) {
            return query2(lc1, lc2, l, m, check);
        } else {
            return query2(tr[p1].r, tr[p2].r, m + 1, r, check);
        }
    }
};
struct info {
    ll val = 0;
    friend info operator+(info a, info b) {
        info res;
        res.val = (a.val ^ b.val);
        return res;
    }
    friend info operator-(info a, info b) {
        info res;
        res.val = (a.val ^ b.val);
        return res;
    }
};
