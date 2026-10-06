// 依赖已 build 的 HLD，无需改 HLD；change(u,+1/-1) 增删一个点。
// answer(u) 时维护状态恰为 u 的子树；调用前后状态均为空。
// 单点增删 O(1) 时总 O(n log n)，显式事件栈，不递归原树。
void dsuOnTree(const HLD& h, auto change, auto answer) {
    if (!h.n) return;
    vector<array<int, 3>> st{{h.root, 0, 0}};
    while (!st.empty()) {
        auto [u, phase, keep] = st.back(); st.pop_back();
        if (!phase) {
            st.pb({u, 1, keep});
            if (h.son[u]) st.pb({h.son[u], 0, 1});
            for (int v : h.e[u]) if (v != h.parent[u] && v != h.son[u]) st.pb({v, 0, 0});
        } else {
            change(u, 1);
            for (int v : h.e[u]) if (v != h.parent[u] && v != h.son[u])
                for (int i = h.in[v]; i < h.in[v] + h.sz[v]; ++i) change(h.seq[i], 1);
            answer(u);
            if (!keep)
                for (int i = h.in[u]; i < h.in[u] + h.sz[u]; ++i) change(h.seq[i], -1);
        }
    }
}
