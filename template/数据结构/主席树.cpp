template<class Info>
struct PST {
    struct Node { int l = 0, r = 0; Info h{}; };
    int n = 0;
    std::vector<Node> tr;

    PST(int n = 0, std::size_t cap = 0) { init(n, cap); }

    void init(int m, std::size_t cap = 0) {
        assert(m >= 0);
        n = m;
        tr.clear();
        tr.reserve(std::max(std::size_t(1), cap));
        tr.emplace_back();
    }

    bool valid(int p) const { return p >= 0 && std::size_t(p) < tr.size(); }
    int clone(int p) {
        tr.push_back(tr[p]);
        return int(tr.size()) - 1;
    }
    void pull(int p) { tr[p].h = tr[tr[p].l].h + tr[tr[p].r].h; }

    // 单点变换：f(const Info&) -> Info；f 不得修改或重入此 PST。
    template<class F>
    int modify(int p, int x, F f) {
        assert(valid(p) && 1 <= x && x <= n);
        return modify(p, 1, n, x, f);
    }
    template<class F>
    int modify(int p, int l, int r, int x, F& f) {
        p = clone(p);
        if (l == r) tr[p].h = f(tr[p].h);
        else {
            int m = l + (r - l) / 2;
            if (x <= m) {
                int c = modify(tr[p].l, l, m, x, f);
                tr[p].l = c; // reacquire by index AFTER possible reallocation
            } else {
                int c = modify(tr[p].r, m + 1, r, x, f);
                tr[p].r = c;
            }
            pull(p);
        }
        return p;
    }

    // 赋值；或将叶子改成 old + v。两者都返回新根。
    int set(int p, int x, Info v) {
        return modify(p, x, [&](const Info&) { return v; });
    }
    int add(int p, int x, Info v) {
        return modify(p, x, [&](const Info& h) { return h + v; });
    }

    Info query(int p, int L, int R) const {
        assert(valid(p));
        if (L > R) return Info{};
        assert(1 <= L && R <= n);
        return query(p, 1, n, L, R);
    }
    Info query(int p, int l, int r, int L, int R) const {
        if (!p) return Info{};
        if (L <= l && r <= R) return tr[p].h;
        int m = l + (r - l) / 2;
        if (R <= m) return query(tr[p].l, l, m, L, R);
        if (L > m) return query(tr[p].r, m + 1, r, L, R);
        return query(tr[p].l, l, m, L, R) + query(tr[p].r, m + 1, r, L, R);
    }

    Info all(int p) const {
        assert(valid(p));
        return tr[p].h;
    }

    // ===== 以下均为可选扩展；不需要时整段删除 =====

    // 可选 1：可交换合并的快速 add；没有交换律时使用上面的 add。
    int add_fast(int root, int x, Info v) {
        assert(valid(root) && 1 <= x && x <= n);
        root = clone(root);
        int p = root, l = 1, r = n;
        while (true) {
            tr[p].h = tr[p].h + v;
            if (l == r) return root;
            int m = l + (r - l) / 2;
            bool left = x <= m;
            int c = clone(left ? tr[p].l : tr[p].r);
            if (left) { tr[p].l = c; r = m; }
            else { tr[p].r = c; l = m + 1; }
            p = c;
        }
    }

    // 可选 2：after-before 的第 k 小，返回坐标；无解为 -1。
    // weight 可加、weight(Info{})==0，各坐标频数差非负；计数运算需可表示。
    template<class F>
    int kth(int before, int after, long long k, F weight) const {
        assert(valid(before) && valid(after));
        if (!n || k <= 0 || k > weight(tr[after].h) - weight(tr[before].h))
            return -1;
        int l = 1, r = n;
        while (l < r) {
            int a = tr[before].l, b = tr[after].l;
            long long cnt = weight(tr[b].h) - weight(tr[a].h);
            int m = l + (r - l) / 2;
            if (k <= cnt) {
                before = a; after = b; r = m;
            } else {
                k -= cnt;
                before = tr[before].r; after = tr[after].r; l = m + 1;
            }
        }
        return l;
    }

    // 可选 3：从 a[1..n] 建立完整版本；不清空旧版本、不改变 n。
    template<class T>
    int build(const std::vector<T>& a) {
        assert(a.size() == std::size_t(n) + 1);
        return n ? build(1, n, a) : 0;
    }
    template<class T>
    int build(int l, int r, const std::vector<T>& a) {
        int p = clone(0);
        if (l == r) tr[p].h = Info(a[l]);
        else {
            int m = l + (r - l) / 2;
            int lc = build(l, m, a), rc = build(m + 1, r, a);
            tr[p].l = lc; tr[p].r = rc;
            pull(p);
        }
        return p;
    }

    // 可选 4：最小 x，使 pred(query(p,L,x)) 成立；无解为 -1。
    // pred(Info{})==false；前缀扩展时 false -> true；谓词只读且确定。
    template<class F>
    int find_first(int p, int L, int R, F pred) const {
        assert(valid(p));
        if (L > R) return -1;
        assert(1 <= L && R <= n);
        Info pre{};
        return find_first(p, 1, n, L, R, pre, pred);
    }
    template<class F>
    int find_first(int p, int l, int r, int L, int R, Info& pre, F& pred) const {
        if (!p || r < L || R < l) return -1;
        if (L <= l && r <= R) {
            Info cur = pre + tr[p].h;
            if (!pred(cur)) { pre = cur; return -1; }
            if (l == r) return l;
        }
        int m = l + (r - l) / 2;
        int x = find_first(tr[p].l, l, m, L, R, pre, pred);
        if (x == -1) x = find_first(tr[p].r, m + 1, r, L, R, pre, pred);
        return x;
    }

    // 可选 5：最大 x，使 pred(query(p,x,R)) 成立；无解为 -1。
    // pred(Info{})==false；后缀向左扩展时 false -> true；谓词只读且确定。
    template<class F>
    int find_last(int p, int L, int R, F pred) const {
        assert(valid(p));
        if (L > R) return -1;
        assert(1 <= L && R <= n);
        Info suf{};
        return find_last(p, 1, n, L, R, suf, pred);
    }
    template<class F>
    int find_last(int p, int l, int r, int L, int R, Info& suf, F& pred) const {
        if (!p || r < L || R < l) return -1;
        if (L <= l && r <= R) {
            Info cur = tr[p].h + suf;
            if (!pred(cur)) { suf = cur; return -1; }
            if (l == r) return l;
        }
        int m = l + (r - l) / 2;
        int x = find_last(tr[p].r, m + 1, r, L, R, suf, pred);
        if (x == -1) x = find_last(tr[p].l, l, m, L, R, suf, pred);
        return x;
    }

    // 可选 6：用 src[L..R] 覆盖 dst 的同坐标区间，返回新根。
    // 两根必须来自当前池；不支持平移复制；两个旧版本均保留。
    int copy(int dst, int src, int L, int R) {
        assert(valid(dst) && valid(src));
        if (L > R) return dst;
        assert(1 <= L && R <= n);
        return copy(dst, src, 1, n, L, R);
    }
    int copy(int a, int b, int l, int r, int L, int R) {
        if (a == b) return a;
        if (L <= l && r <= R) return b;
        int p = clone(a), m = l + (r - l) / 2;
        if (L <= m) {
            int c = copy(tr[a].l, tr[b].l, l, m, L, R);
            tr[p].l = c;
        }
        if (R > m) {
            int c = copy(tr[a].r, tr[b].r, m + 1, r, L, R);
            tr[p].r = c;
        }
        pull(p);
        return p;
    }

    // 可选 7：节点池辅助接口；nodes() 包括 0 号空节点。
    void reserve(std::size_t cap) { tr.reserve(cap); }
    std::size_t nodes() const { return tr.size(); }
    std::size_t capacity() const { return tr.capacity(); }
};
