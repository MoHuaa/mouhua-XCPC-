template<int S = 26, int BASE = 'a'>
struct SAM {
    struct Node {
        int len = 0, link = 0;
        array<int, S> next{};
    };
    vector<Node> t;

    SAM(string_view s = "") { init(); add(s); }

    void init() {
        t.assign(2, {});
        t[0].len = -1;
        t[0].next.fill(1);
    }

    int size() const { return t.size(); }

    // Internal: an existing transition p --c--> q, c is an alphabet index.
    int split(int p, int c) {
        int q = t[p].next[c];
        if (t[q].len == t[p].len + 1) return q;
        int r = size();
        t.push_back(t[q]);
        t[r].len = t[p].len + 1;
        t[q].link = r;
        while (t[p].next[c] == q) {
            t[p].next[c] = r;
            p = t[p].link;
        }
        return r;
    }

    // p represents the complete prefix being extended; ch is a raw character.
    int extend(int p, unsigned char ch) {
        int c = int(ch) - BASE;
        if (t[p].next[c]) return split(p, c);
        int u = size();
        t.emplace_back();
        t[u].len = t[p].len + 1;
        while (!t[p].next[c]) {
            t[p].next[c] = u;
            p = t[p].link;
        }
        t[u].link = split(p, c);
        return u;
    }

    int add(string_view s, int p = 1) {
        for (unsigned char c : s) p = extend(p, c);
        return p;
    }

    int next(int p, unsigned char c) const {
        return t[p].next[int(c) - BASE];
    }

    // Missing substring -> 0. Empty string -> supplied start state.
    int find(string_view s, int p = 1) const {
        for (unsigned char c : s)
            if (!(p = next(p, c))) return 0;
        return p;
    }

    // ===== Optional: number of distinct nonempty substrings =====
    long long distinct() const {
        long long ans = 0;
        for (int u = 2; u < size(); ++u)
            ans += t[u].len - t[t[u].link].len;
        return ans;
    }

    // ===== Optional: order and suffix-link aggregation =====
    // All real states, root included, ascending len. No state renumbering.
    vector<int> order() const {
        vector<int> cnt(size()), q(size() - 1);
        for (int u = 1; u < size(); ++u) ++cnt[t[u].len];
        partial_sum(cnt.begin(), cnt.end(), cnt.begin());
        for (int u = size() - 1; u; --u) q[--cnt[t[u].len]] = u;
        return q;
    }

    template<class T>
    void pull(vector<T>& a) const {
        auto q = order();
        for (int i = int(q.size()) - 1; i > 0; --i) {
            int u = q[i];
            a[t[u].link] = a[t[u].link] + a[u];
        }
    }

    // s must be a source string previously inserted from the root.
    // This is NOT a query for occurrence counts in arbitrary new text.
    // Root total = |s|, not the number of empty-string occurrences.
    vector<long long> count(string_view s) const {
        vector<long long> a(size());
        int u = 1;
        for (unsigned char c : s) {
            u = next(u, c);
            ++a[u];
        }
        pull(a);
        return a;
    }

    // ===== Optional: streaming longest substring match =====
    // State is {SAM state, actual matched length}, initially {1,0}.
    pair<int, int> match(pair<int, int> state, unsigned char ch) const {
        auto [u, len] = state;
        int c = int(ch) - BASE;
        while (!t[u].next[c]) {
            u = t[u].link;
            len = t[u].len;
        }
        return {t[u].next[c], len + 1};
    }

    int lcs(string_view s) const {
        pair<int, int> state{1, 0};
        int ans = 0;
        for (unsigned char c : s) {
            state = match(state, c);
            ans = max(ans, state.second);
        }
        return ans;
    }

    // ===== Optional: lexicographic selection =====
    // w[0]=w[1]=0, nonnegative multiplicities, all sums fit long long.
    vector<long long> ways(const vector<long long>& w) const {
        auto dp = w;
        auto q = order();
        for (int u : q | views::reverse)
            for (int v : t[u].next)
                if (v) dp[u] += dp[v];
        return dp;
    }

    // 1-based k, guaranteed 1 <= k <= dp[1]; alphabet order = byte order.
    string kth(long long k, const vector<long long>& w,
               const vector<long long>& dp) const {
        string ans;
        int u = 1;
        while (k > w[u]) {
            k -= w[u];
            for (int c = 0; c < S; ++c) {
                int v = t[u].next[c];
                if (!v) continue;
                if (k > dp[v]) k -= dp[v];
                else {
                    ans += char(BASE + c);
                    u = v;
                    break;
                }
            }
        }
        return ans;
    }
};