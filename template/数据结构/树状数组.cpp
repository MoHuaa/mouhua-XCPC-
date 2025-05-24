template <typename T>
struct BIT {
    int n;
    std::vector<T> a;

    BIT(int n_ = 0) {
        init(n_);
    }

    void init(int n_) {
        n = n_;
        a.assign(n + 2, T{});
    }

    void add(int x, const T &v) {
        for (int i = x + 1; i <= n; i += i & -i) {
            a[i - 1] = a[i - 1] + v;
        }
    }

    T sum(int x) {
        T ans{};
        x++;
        for (int i = x; i > 0; i -= i & -i) {
            ans = ans + a[i - 1];
        }
        return ans;
    }

    T getSum(int l, int r) {
        if (l > r)swap(l, r);
        return sum(r) - sum(l - 1);
    }
    void modify(int l, int r, T x) {
        if (l > r)swap(l, r);
        add(l, x);
        add(r + 1, -x);
    }
    int select(const T &k) {
        int x = 0;
        T cur{};
        for (int i = 1 << std::__lg(n); i; i /= 2) {
            if (x + i <= n && cur + a[x + i - 1] <= k) {
                x += i;
                cur = cur + a[x - 1];
            }
        }
        return x;
    }
};