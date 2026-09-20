template<class T, class Cmp = greater<T>>
struct RMQ {
    vector<T> a, win; // win[r]: 以 r 结尾、长度至多 64 的窗口极值
    vector<ull> mask;
    vector<vector<T>> st;
    static T op(T x, T y) { return Cmp{}(x, y) ? x : y; }
    static int lg(ull x) { return 63 - __builtin_clzll(x); }

    RMQ(const vector<T>& v) : a(v), win(v.size()), mask(v.size()) {
        int n = a.size(), m = n >> 6;
        ull s = 0;
        for (int i = 0; i < n; i++) {
            s <<= 1;
            while (s && !Cmp{}(a[i - __builtin_ctzll(s)], a[i]))
                s &= s - 1;
            mask[i] = s |= 1;
            win[i] = a[i - lg(s)];
        }

        vector<T> b(m);
        for (int i = 0; i < m; i++) b[i] = win[(i << 6) + 63];
        st.pb(move(b));
        for (int k = 1; (1 << k) <= m; k++) {
            st.emplace_back(m - (1 << k) + 1);
            for (int i = 0; i < int(st[k].size()); i++)
                st[k][i] = op(st[k - 1][i], st[k - 1][i + (1 << (k - 1))]);
        }
    }

    T operator()(int l, int r) const { // [l, r]
        if (l == r) return a[l];
        if (r - l < 64) {
            ull s = mask[r] & (~0ULL >> (63 - (r - l)));
            return a[r - lg(s)];
        }
        T res = op(win[l + 63], win[r]);
        int L = (l >> 6) + 1, R = (r >> 6) - 1;
        if (L <= R) {
            int k = lg(R - L + 1);
            res = op(res, op(st[k][L], st[k][R - (1 << k) + 1]));
        }
        return res;
    }
};