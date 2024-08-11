template<class Info, class Tag>
struct LazySegmentTree {
    int n;
    std::vector<Info> info;
    std::vector<Tag> tag;
    LazySegmentTree() : n(0) {}
    LazySegmentTree(int n_, Info v_ = Info()) {
        init(n_, v_);
    }
    template<class T>
    LazySegmentTree(std::vector<T> init_) {
        init(init_);
    }
    void init(int n_, Info v_ = Info()) {
        init(std::vector(n_, v_));
    }
    template<class T>
    void init(std::vector<T> init_) {
        n = init_.size() - 1;
        info.assign(4 << std::__lg(n) + 2, Info());
        tag.assign(4 << std::__lg(n) + 2, Tag());
        auto build = [&](auto && build, int p, int l, int r)->void{
            if (l == r) {
                info[p] = init_[l];
                return;
            }
            int mid = (l + r) >> 1;
            build(build, p << 1, l, mid);
            build(build, p << 1 | 1, mid + 1, r);
            push_up(p);
        };
        build(build, 1, 1, n);
    }
    void push_up(int p) {
        info[p] = (info[p << 1] + info[p << 1 | 1]);
    }
    void push_down(int p, int l, int r) {
        info[p << 1] += tag[p];
        info[p << 1 | 1] += tag[p];
        tag[p << 1] += tag[p];
        tag[p << 1 | 1] += tag[p];
        tag[p].init();
    }
    void change(int p, int l, int r, int nl, int nr, Tag num) {
        if (nl <= l and r <= nr) {
            info[p] += num;
            tag[p] += num;
            return;
        }
        push_down(p, l, r);
        int mid = (l + r) >> 1;
        if (nl <= mid)change(p << 1, l, mid, nl, nr, num);
        if (nr > mid)change(p << 1 | 1, mid + 1, r, nl, nr, num);
        push_up(p);
    }
    Info query(int p, int l, int r, int nl, int nr) {
        if (nl <= l and r <= nr) {
            return info[p];
        }
        push_down(p, l, r);
        push_up(p);
        int mid = (l + r) >> 1;
        if (nr <= mid)return query(p << 1, l, mid, nl, nr);
        if (nl > mid)return query(p << 1 | 1, mid + 1, r, nl, nr);
        return query(p << 1, l, mid, nl, nr) + query(p << 1 | 1, mid + 1, r, nl, nr);
    }
};
struct tag {
    void init() {

    }
    tag&operator+=(const tag &t) & {

        return *this;
    }
};
struct node {
    friend node operator+(node lhs, node rhs) {
        node res;
        return res;
    }
    node&operator+=(tag&t) {
        return *this;
    }
};