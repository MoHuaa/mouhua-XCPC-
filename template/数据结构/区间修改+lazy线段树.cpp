template<class Info, class Tag>
struct LazySegmentTree {
    int n = 0;
    std::vector<Info> info;
    std::vector<Tag> tag;

    LazySegmentTree() = default;
    template<class T>
    LazySegmentTree(const std::vector<T>& a) { init(a); }

    template<class T>
    void init(const std::vector<T>& a) {
        n = std::max(0, (int)a.size() - 1);
        info.assign(4 * n + 4, Info{});
        tag.assign(4 * n + 4, Tag{});
        auto build = [&](auto&& self, int p, int l, int r) -> void {
            if (l == r) {
                info[p] = Info(a[l]);
                return;
            }
            int m = (l + r) >> 1;
            self(self, p * 2, l, m);
            self(self, p * 2 + 1, m + 1, r);
            push_up(p);
        };
        if (n) build(build, 1, 1, n);
    }

    void push_up(int p) { info[p] = info[p * 2] + info[p * 2 + 1]; }

    void apply(int p, const Tag& t) {
        info[p] += t;
        tag[p] += t;
    }

    void push_down(int p) {
        apply(p * 2, tag[p]);
        apply(p * 2 + 1, tag[p]);
        tag[p] = Tag{};
    }

    void change(int l, int r, const Tag& t) {
        if (n && l <= r) change(1, 1, n, l, r, t);
    }
    void change(int p, int l, int r, int L, int R, const Tag& t) {
        if (L <= l && r <= R) {
            apply(p, t);
            return;
        }
        push_down(p);
        int m = (l + r) >> 1;
        if (L <= m) change(p * 2, l, m, L, R, t);
        if (R > m) change(p * 2 + 1, m + 1, r, L, R, t);
        push_up(p);
    }

    Info query(int l, int r) {
        return n && l <= r ? query(1, 1, n, l, r) : Info{};
    }
    Info query(int p, int l, int r, int L, int R) {
        if (L <= l && r <= R) return info[p];
        push_down(p);
        int m = (l + r) >> 1;
        if (R <= m) return query(p * 2, l, m, L, R);
        if (L > m) return query(p * 2 + 1, m + 1, r, L, R);
        return query(p * 2, l, m, L, R)
             + query(p * 2 + 1, m + 1, r, L, R);
    }

    Info all() const { return n ? info[1] : Info{}; }

    // OPTIONAL: overwrite one leaf with a complete Info value.
    void set(int x, const Info& v) { set(1, 1, n, x, v); }
    void set(int p, int l, int r, int x, const Info& v) {
        if (l == r) {
            info[p] = v;
            tag[p] = Tag{};
            return;
        }
        push_down(p);
        int m = (l + r) >> 1;
        if (x <= m) set(p * 2, l, m, x, v);
        else set(p * 2 + 1, m + 1, r, x, v);
        push_up(p);
    }
    template<class F>
    int find_first(int l, int r, F pred) {
        Info pre{};
        return n && l <= r ? find_first(1, 1, n, l, r, pre, pred) : -1;
    }
    template<class F>
    int find_first(int p, int l, int r, int L, int R, Info& pre, F& pred) {
        if (r < L || R < l) return -1;
        if (L <= l && r <= R) {
            Info cur = pre + info[p];
            if (!pred(cur)) {
                pre = cur;
                return -1;
            }
            if (l == r) return l;
        }
        push_down(p);
        int m = (l + r) >> 1;
        int x = find_first(p * 2, l, m, L, R, pre, pred);
        if (x == -1) x = find_first(p * 2 + 1, m + 1, r, L, R, pre, pred);
        return x;
    }
    template<class F>
    int find_last(int l, int r, F pred) {
        Info suf{};
        return n && l <= r ? find_last(1, 1, n, l, r, suf, pred) : -1;
    }
    template<class F>
    int find_last(int p, int l, int r, int L, int R, Info& suf, F& pred) {
        if (r < L || R < l) return -1;
        if (L <= l && r <= R) {
            Info cur = info[p] + suf;
            if (!pred(cur)) {
                suf = cur;
                return -1;
            }
            if (l == r) return l;
        }
        push_down(p);
        int m = (l + r) >> 1;
        int x = find_last(p * 2 + 1, m + 1, r, L, R, suf, pred);
        if (x == -1) x = find_last(p * 2, l, m, L, R, suf, pred);
        return x;
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
    node&operator+=(tag t) {
        
        return *this;
    }
};