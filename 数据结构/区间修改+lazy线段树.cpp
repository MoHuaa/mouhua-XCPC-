template<class Info, class Tag>
struct LazySegmentTree {
    int n;
    std::vector<Info> info;
    std::vector<Tag> tag;
    LazySegmentTree() : n(0) {}
    template<class T>
    LazySegmentTree(std::vector<T> init_) {
        init(init_);
    }
    template<class T>
    void init(std::vector<T> init_) {
        n = init_.size() - 1;
        info.assign((4*n+1, Info());
        tag.assign(4*n+1, Tag());
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
    void push_down(int p) {
        info[p << 1] += tag[p];
        info[p << 1 | 1] += tag[p];
        tag[p << 1] += tag[p];
        tag[p << 1 | 1] += tag[p];
        tag[p].init();
    }
    void change(int l, int r, Tag num) {
        change(1, 1, n, l, r, num);
    }
    void change(int p, int l, int r, int nl, int nr, Tag num) {
        if (nl <= l and r <= nr) {
            info[p] += num;
            tag[p] += num;
            return;
        }
        push_down(p);
        int mid = (l + r) >> 1;
        if (nl <= mid)change(p << 1, l, mid, nl, nr, num);
        if (nr > mid)change(p << 1 | 1, mid + 1, r, nl, nr, num);
        push_up(p);
    }
    Info query(int l, int r) {
        return query(1, 1, n, l, r);
    }
    Info query(int p, int l, int r, int nl, int nr) {
        if (nl <= l and r <= nr) {
            return info[p];
        }
        push_down(p);
        push_up(p);
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
        push_down(p);
        push_up(p);
        int mid = (l + r) >> 1;
        if (f(info[p << 1]))return find_first(p << 1, l, mid, f);
        else return find_first(p << 1 | 1, mid + 1, r, f);
    }
    int find_first(const std::function<bool(const Info &)> &f) {
        return find_first(1, 1, n, f);
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