template<class Info>
struct Segment {
    int n, size;
    vector<Info> info;

    Segment(int n = 0) { init(n); }
    template<class T>
    Segment(const vector<T>& a) { init(a); }

    void init(int m) {
        n = m;
        size = bit_ceil(unsigned(n));
        info.assign(2 * size, Info{});
    }

    template<class T>
    void init(const vector<T>& a) {
        init(int(a.size()) - 1);
        for (int i = 1; i <= n; ++i)
            info[size + i - 1] = Info(a[i]);
        for (int p = size - 1; p; --p)
            pull(p);
    }

    void pull(int p) {
        info[p] = info[p * 2] + info[p * 2 + 1];
    }

    // Assign a complete leaf value, not an increment.
    void change(int p, Info v) {
        info[p += size - 1] = v;
        while (p >>= 1) pull(p);
    }

    Info query(int l, int r) const {
        Info left{}, right{};
        for (l += size - 1, r += size; l < r; l >>= 1, r >>= 1) {
            if (l & 1) left = left + info[l++];
            if (r & 1) right = info[--r] + right;
        }
        return left + right;
    }

    Info get(int p) const { return info[size + p - 1]; }
    Info all() const { return info[1]; }

    // ===== OPTIONAL: leaf increment / transform =====
    void add(int p, Info v) { change(p, get(p) + v); }

    template<class F>
    void modify(int p, F f) { change(p, f(get(p))); }

    // ===== OPTIONAL: bounded search =====
    // First x in [l,r] with pred(query(l,x)); -1 if none.
    // pred(Info{})=false; false -> true as the requested prefix grows.
    template<class F>
    int find_first(int l, int r, F pred) const {
        Info pre{};
        for (l += size - 1, r += size; l < r;) {
            int k = min(countr_zero(unsigned(l)),
                        int(bit_width(unsigned(r - l))) - 1);
            int p = l >> k;
            Info x = pre + info[p];
            if (pred(x)) {
                while (p < size) {
                    p *= 2;
                    x = pre + info[p];
                    if (!pred(x)) { pre = x; ++p; }
                }
                return p - size + 1;
            }
            pre = x;
            l += 1 << k;
        }
        return -1;
    }

    // Last x in [l,r] with pred(query(x,r)); -1 if none.
    // pred(Info{})=false; false -> true as the requested suffix grows leftward.
    template<class F>
    int find_last(int l, int r, F pred) const {
        Info suf{};
        for (l += size - 1, r += size; l < r;) {
            int k = min(countr_zero(unsigned(r)),
                        int(bit_width(unsigned(r - l))) - 1);
            int p = (r >> k) - 1;
            Info x = info[p] + suf;
            if (pred(x)) {
                while (p < size) {
                    p = p * 2 + 1;
                    x = info[p] + suf;
                    if (!pred(x)) { suf = x; --p; }
                }
                return p - size + 1;
            }
            suf = x;
            r -= 1 << k;
        }
        return -1;
    }

    // ===== OPTIONAL: stop if the aggregate is unchanged =====
    // Requires semantic equality on all fields used by future operations.
    void change_early(int p, Info v) {
        info[p += size - 1] = v;
        while (p >>= 1) {
            Info x = info[p * 2] + info[p * 2 + 1];
            if (info[p] == x) break;
            info[p] = x;
        }
    }
};