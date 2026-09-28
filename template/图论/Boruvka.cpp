struct Boruvka {
    int n, cnt;
    vector<int> p;
    Boruvka(int n) : n(n), cnt(n), p(n + 1, -1) {}
    // 根上存负的集合大小，其他位置存父亲。
    int find(int x) { return p[x] < 0 ? x : p[x] = find(p[x]); }
    ll build(auto update) {
        fill(p.begin(), p.end(), -1);
        cnt = n;
        ll ans = 0;
        vector<pair<ll, int>> best(n + 1, {0, -1});
        while (cnt > 1) {
            update(best);
            int old = cnt;
            for (int i = 1; i <= n; i++) {
                auto [w, v] = best[i];
                if (v == -1) continue;
                best[i].second = -1; // 候选读出即清空，不必下轮整表重置。
                int a = find(i), b = find(v);
                if (a == b) continue;
                if (p[a] > p[b]) swap(a, b);
                p[a] += p[b];
                p[b] = a;
                ans += w;
                --cnt;
            }
            if (cnt == old) break;
        }
        return ans;
    }
};
