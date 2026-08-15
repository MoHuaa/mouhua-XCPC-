template<class T>
struct Segtree {
    T o[1 << 20]; int L;
    void upt(int x) {
        o[x] = o[x << 1] + o[x << 1 | 1];
    }
    void init(int n,const vector<T>&w) {
        L = 2 << std::__lg(n + 1);
        for (int i = 1; i <= n; ++i) o[i + L] = w[i];
        for (int i = L; i >= 1; --i) upt(i);
    }
    void change(int p, T v) {
        for (o[p += L] += v; p >>= 1; upt(p));
    }
    T query(int l, int r) {
        if (l == r) return o[l + L];
        T le = o[l + L], ri = o[r + L];
        l = l+L, r = r+L;
        for (; l ^ r ^ 1; l >>= 1, r >>= 1) {
            if ((l & 1) == 0) le = le + o[l ^ 1];
            if ((r & 1) == 1) ri = o[r ^ 1] + ri;
        }
        return le + ri;
    }
};
struct S{
    ll x=0;
    friend S operator+(S a,S b){
        S res;
        res.x=a.x+b.x;
        return res;
    }
};