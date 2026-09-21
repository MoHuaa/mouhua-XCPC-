struct PollardRho {
    mt19937_64 rng;
    PollardRho(ull seed = chrono::steady_clock::now().time_since_epoch().count())
        : rng(seed) {}
    // 前十二个质数；37 不能删，前九个底数不能覆盖整个 ull。
    static constexpr int bases[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37};

    // 奇模数上的蒙哥马利运算：所有表示始终在 [0,n) 内。
    struct Mont {
        ull n, inv, r2, one;
        // r2 是 2^128 mod n；负号在 u128 中回绕，不是有符号溢出。
        Mont(ull n) : n(n), inv(n), r2(-u128(n) % n) {
            // 求 n 在模 2^64 下的逆元，ull 的回绕是这里有意使用的运算。
            for (int i = 0; i < 6; i++) inv *= 2 - n * inv;
            one = reduce(r2);
        }
        // 输入 t<n*2^64，返回 t 乘以 2^64 的模逆元，再对 n 取模。
        ull reduce(u128 t) const {
            ull h = t >> 64;
            ull q = (u128)(ull(t) * inv) * n >> 64;
            return h >= q ? h - q : n - (q - h);
        }
        ull mul(ull a, ull b) const { return reduce((u128)a * b); }
        ull add(ull a, ull b) const { return a >= n - b ? a - (n - b) : a + b; }
        // 普通底数 a<n 输入；返回 a^e 的蒙哥马利表示，不是普通余数。
        ull power(ull a, ull e) const {
            ull ans = one;
            a = mul(a, r2);
            for (; e; e >>= 1, a = mul(a, a))
                if (e & 1) ans = mul(ans, a);
            return ans;
        }
    };
    // 确定性判素；0、1 返回 false；不消耗随机数。
    static bool isPrime(ull n) {
        if (n < 2) return false;
        for (ull p : bases) if (n % p == 0) return n == p;
        if (n < 41 * 41) return true;
        Mont m(n);
        int s = __builtin_ctzll(n - 1);
        ull d = (n - 1) >> s;
        // 32 位只需这三个底数；更大时使用前十二个质数。
        static constexpr int small[] = {2, 7, 61};
        for (int i = 0; i < (n >> 32 ? 12 : 3); i++) {
            ull a = n >> 32 ? bases[i] : small[i];
            ull x = m.power(a, d);
            if (x == m.one || x == n - m.one) continue;
            int r = 1;
            for (; r < s; r++) {
                x = m.mul(x, x);
                if (x == n - m.one) break;
            }
            if (r == s) return false;
        }
        return true;
    }
    // n 为已判定的奇合数；仅返回真因子，失败就重选起点和常数。
    ull rho(ull n) {
        Mont m(n);
        while (true) {
            ull y = rng() % (n - 1) + 1, c = rng() % (n - 1) + 1;
            // 起点和常数各转换一次；后续全程留在蒙哥马利域。
            y = m.mul(y, m.r2);
            c = m.mul(c, m.r2);
            auto f = [&](ull x) { return m.add(m.mul(x, x), c); };
            ull x = 0, z = 0, g = 1;
            for (ull r = 1; g == 1; r *= 2) {
                x = y;
                for (ull i = 0; i < r; i++) y = f(y);
                // z 记块首；每块最多 128 项，q 只乘当前块。
                for (ull k = 0; k < r && g == 1; k += 128) {
                    z = y;
                    ull q = m.one;
                    for (ull i = 0; i < min(128ULL, r - k); i++) {
                        y = f(y);
                        q = m.mul(q, x > y ? x - y : y - x);
                    }
                    g = gcd(q, n); // 不必转换回来，乘可逆的 2^64 不改变最大公约数。
                }
            }
            // 整块乘积给出 n 时，从块首逐项重放，不丢掉已有进展。
            if (g == n) {
                do {
                    z = f(z);
                    g = gcd(x > z ? x - z : z - x, n);
                } while (g == 1);
            }
            if (g < n) return g; // 此时已知 g>1；g==n 则重选参数。
        }
    }
    void split(ull n, vector<ull>& ans) {
        if (n == 1) return;
        if (isPrime(n)) { ans.pb(n); return; }
        // 完全平方直接拆；开方只作初值，最终靠整数校正和确认。
        ull d = sqrtl((long double)n);
        while ((u128)d * d > n) d--;
        while ((u128)(d + 1) * (d + 1) <= n) d++;
        if ((u128)d * d != n) d = rho(n);
        split(d, ans);
        split(n / d, ans);
    }
    // 返回递增质因子，保留重复；factor(1) 为空；可连续调用。
    vector<ull> factor(ull n) {
        vector<ull> ans;
        for (ull p : bases)
            while (n % p == 0) { ans.pb(p); n /= p; }
        split(n, ans);
        sort(ans.begin(), ans.end());
        return ans;
    }
    vector<ull> divisors(ull n) {
    auto p = factor(n);
    vector<ull> ans{1};
    for (int i = 0; i < (int)p.size();) {
        int j = i, sz = ans.size();
        ull pw = 1;
        while (j < (int)p.size() && p[j] == p[i]) {
            pw *= p[j++];
            // sz 固定为处理当前质数前的长度，不能改成 ans.size()。
            for (int k = 0; k < sz; k++) ans.pb(ans[k] * pw);
        }
        i = j;
    }
    sort(ans.begin(), ans.end());
    return ans;
}
};