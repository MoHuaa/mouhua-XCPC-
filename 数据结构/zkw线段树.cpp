template<class T>
struct Segment {
    T o[1 << 20]; int L;
    void upt(int x) {
        o[x] = o[x << 1] + o[x << 1 | 1];
    }
    void init(int n, const vector<T>&w) {
        L = 2 << std::__lg(n + 1);
        for (int i = 1; i <= n; ++i) o[i + L] = w[i];
        for (int i = L; i >= 1; --i) upt(i);
    }
    void change(int p, T v) {
        for (o[p += L] = v; p >>= 1; upt(p));
    }
    T query(int l, int r) {
        l += L - 1, r += L + 1;
        T ans{};
        for (; l ^ r ^ 1; l >>= 1, r >>= 1) {
            if ((l & 1) == 0) ans =ans+ o[l ^ 1];
            if ((r & 1) == 1) ans =ans+ o[r ^ 1];
        }
        return ans;
    }
};
struct node {
    ll val = 0;
    node () {
    }
    friend  node operator+(node lhs, node rhs) {
        node now;
        if (lhs.val > rhs.val)now = lhs;
        else now = rhs;
        return now;
    }
};
Segment<node>seg;