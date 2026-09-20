#include <bits/stdc++.h>
using namespace std;

// 状态 0 为根，字符范围 [BASE, BASE+S)。
// 非空模式全部插入后 build 一次，之后不再 add。
template<int S = 26, int BASE = 'a'>
struct AhoCorasick {
    vector<array<int, S>> tr;
    vector<int> link, q;

    AhoCorasick() {
        init();
    }

    void init() {
        tr.assign(1, {});
        link.clear();
        q.clear();
    }

    int size() const {
        return tr.size();
    }

    int add(string_view s) {
        int u = 0;

        for (unsigned char ch : s) {
            int c = ch - BASE;

            if (!tr[u][c]) {
                tr[u][c] = size();
                tr.emplace_back();
            }

            u = tr[u][c];
        }

        return u;
    }

    void build() {
        link.assign(size(), 0);
        q.resize(size());
        q[0] = 0;

        int tail = 1;
        for (int v : tr[0])
            if (v) q[tail++] = v;

        for (int i = 1; i < tail; ++i) {
            int u = q[i], f = link[u];

            for (int c = 0; c < S; ++c) {
                int v = tr[u][c];

                if (v) {
                    link[v] = tr[f][c];
                    q[tail++] = v;
                } else {
                    tr[u][c] = tr[f][c];
                }
            }
        }
    }

    int step(int u, unsigned char c) const {
        return tr[u][c - BASE];
    }

    // ===== 可选：扫描文本，返回最终状态 =====

    template<class F>
    int scan(string_view s, F visit, int u = 0) const {
        for (unsigned char c : s) {
            u = step(u, c);
            visit(u);
        }
        return u;
    }

    // ===== 可选：沿失配链接向上汇总 =====

    template<class T>
    void pull(vector<T>& a) const {
        for (int i = size() - 1; i > 0; --i) {
            int u = q[i];
            a[link[u]] = a[link[u]] + a[u];
        }
    }

    vector<long long> count(string_view s) const {
        vector<long long> cnt(size());

        scan(s, [&](int u) {
            ++cnt[u];
        });

        pull(cnt);
        return cnt;
    }

    // ===== 可选：继承后缀信息 =====

    template<class T>
    void inherit(vector<T>& a) const {
        for (int i = 1; i < size(); ++i) {
            int u = q[i];
            a[u] = a[link[u]] + a[u];
        }
    }

    // ===== 可选：失配树 =====

    vector<vector<int>> failTree() const {
        vector<vector<int>> g(size());

        for (int u = 1; u < size(); ++u)
            g[link[u]].push_back(u);

        return g;
    }
};