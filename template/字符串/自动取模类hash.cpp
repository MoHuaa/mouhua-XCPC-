struct Hash {
    inline static const Z Seed = [] {
        std::mt19937_64 rng(
            std::chrono::steady_clock::now().time_since_epoch().count()
        );
        return Z(std::uniform_int_distribution<ull>(257, P - 257)(rng));
    }();
    inline static vector<Z> bas{Z(1)};
    static void HInit(int n) {
        int m = (int)bas.size();
        if (m > n) return;
        bas.resize(n + 1);
        for (int i = m; i <= n; ++i)
            bas[i] = bas[i - 1] * Seed;
    }
    struct Info {
        int len = 0;
        Z h = Z(0);
        friend Info operator+(const Info& a, const Info& b) {
            Hash::HInit(b.len);
            return {a.len + b.len, a.h * Hash::bas[b.len] + b.h};
        }
        Info& operator+=(const Info& b) { return *this = *this + b; }
        friend bool operator==(const Info& a, const Info& b) {
            return a.len == b.len && a.h == b.h;
        }
        friend bool operator!=(const Info& a, const Info& b) {
            return !(a == b);
        }
    };
    int n;
    vector<Z> sum;
    Hash(const string& s = "") { init(s); }
    void init(const string& s) {
        n = (int)s.size();
        HInit(n);
        sum.resize(n + 1);
        sum[0] = Z(0);
        for (int i = 1; i <= n; ++i)
            sum[i] = sum[i - 1] * Seed + Z((unsigned char)s[i - 1]);
    }

    Info getHash(int l, int r) const {
        return {r - l + 1,sum[r] - sum[l - 1] * bas[r - l + 1]};
    }
};