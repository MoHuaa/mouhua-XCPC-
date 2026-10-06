// 依赖已 build 的 HLD，点 1..n；v 可重复、可为空。
// 返回 v：原点编号；fa：局部父下标，根 -1；父亲总在孩子前面。
// O(k log k + k log n)，只清理本次节点；边长为 dep[v[i]]-dep[v[fa[i]]]。
auto virtualTree(const HLD& h, vector<int> v) {
    struct Result { vector<int> v, fa; };
    auto cmp = [&](int x, int y) { return h.in[x] < h.in[y]; };
    sort(v.begin(), v.end(), cmp);
    v.erase(unique(v.begin(), v.end()), v.end());
    int k = v.size();
    for (int i = 1; i < k; ++i) v.pb(h.lca(v[i - 1], v[i]));
    sort(v.begin(), v.end(), cmp);
    v.erase(unique(v.begin(), v.end()), v.end());
    vector<int> fa(v.size(), -1), st;
    for (int i = 0; i < int(v.size()); ++i) {
        while (!st.empty() && h.in[v[st.back()]] + h.sz[v[st.back()]] <= h.in[v[i]])
            st.pop_back();
        if (!st.empty()) fa[i] = st.back();
        st.pb(i);
    }
    return Result{move(v), move(fa)};
}
