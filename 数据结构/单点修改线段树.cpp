template<class info>
struct Segment {
    std::vector<info> tre;
    int n;
    Segment() : n(0) {}
    Segment(int n) : n(n), tre(4 << std::__lg(n)+2) {}
    Segment(int n, vector<info>&v) : n(n), tre(4 << std::__lg(n)+2) {
        init(v);
    }
    void pushup(int p) {
        tre[p] = tre[p << 1] + tre[p << 1 | 1];
    }
    void init(vector<info>&v) {
        auto build = [&](auto && build, int p, int l, int r) {
            if (l == r) {
                tre[p] = v[l];
                return;
            }
            int mid = (l + r) >> 1;
            build(build, p << 1, l, mid);
            build(build, p << 1 | 1, mid + 1, r);
            pushup(p);
        };
        build(build, 1, 1, n);
    }
    void change(int pos,info x){
        change(1,1,n,pos,x);
    }
    void change(int p, int l, int r, int pos, info x) {
        if (l == r) {
            tre[p]=x;
            return;
        }
        int mid = (l + r) >> 1;
        if (pos <= mid)change(p << 1, l, mid, pos, x);
        else change(p << 1 | 1, mid + 1, r, pos, x);
        pushup(p);
    }
    info query(int l,int r){
        return query(1,1,n,l,r);
    }
    info query(int p, int l, int r, int nl, int nr) {
        if (nl <= l and r <= nr)return tre[p];
        int mid = (l + r) >> 1;
        if (nr <= mid)return query(p << 1, l, mid, nl, nr);
        if (nl > mid)return query(p << 1 | 1, mid + 1, r, nl, nr);
        return query(p << 1, l, mid, nl, nr) + query(p << 1 | 1, mid + 1, r, nl, nr);
    }
};
struct node {
    ll val;
    int id;
    node () {
    }
    friend  node operator+(node lhs, node rhs) {
        node now;
        if (lhs.val < rhs.val)now = lhs;
        else now = rhs;
        return now;
    }
};