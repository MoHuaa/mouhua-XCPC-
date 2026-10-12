template<class T = long long, int B = 512>
struct Block {
    static_assert(B > 0);
    int n;
    vector<T> a, sum;
    Block(int n = 0) : n(n), a(n), sum(n / B + (n % B != 0)) {}
    Block(vector<T> v) { init(move(v)); }
    void init(vector<T> v) {
        a = move(v); n = a.size();
        sum.assign(n / B + (n % B != 0), T{});
        for (int i = 0; i < n; ++i) sum[i / B] += a[i];
    }
    void add(int x, T v) {
        a[x] += v;
        sum[x / B] += v;
    }
    void set(int x, T v) { add(x, v - a[x]); }
    T query(int l, int r) const {
        T ans{};
        if (l > r) return ans;
        int x = l / B + (l % B != 0), y = (r + 1) / B;
        if (x > y) {
            for (int i = l; i <= r; ++i) ans += a[i];
        } else {
            for (int i = l; i < x * B; ++i) ans += a[i];
            for (int b = x; b < y; ++b) ans += sum[b];
            for (int i = y * B; i <= r; ++i) ans += a[i];
        }
        return ans;
    }
    // [K] a[i]>=0。返回前缀和首次达到 k 的下标；k<=0 或不足 k 返回 -1。
    // a 为频数时是第 k 小值的编号；a 为出现标记时是第 k 小的不同值编号。
    int kth(T k) const {
        if (k <= T{}) return -1;
        int b = 0;
        while (b < int(sum.size()) && sum[b] < k) k -= sum[b++];
        if (b == int(sum.size())) return -1;
        int x = b * B;
        while (a[x] < k) k -= a[x++];
        return x;
    }
    // [Z] 仅适用于 a[i] 为 0/1 的出现标记，不可直接对频数调用。
    // 返回第一个 0 的下标；全为 1 返回 n。编号必须就是真实非负整数才是 mex。
    int mex() const {
        int b = 0;
        while (b < int(sum.size()) && sum[b] == min(B, n - b * B)) ++b;
        if (b == int(sum.size())) return n;
        int x = b * B;
        while (a[x] != T{}) ++x;
        return x;
    }
};